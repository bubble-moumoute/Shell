/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>

#include <stddef.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "readcmd.h"
#include "csapp.h"


int main()
{   // fork ici pour éxécuter en arrière plan si demandé?
    int quit_bool=0;

	while (!quit_bool) {
		struct cmdline *l;
		int i, j;
		int p[2];
        char **argv = NULL;
        char **env = NULL;
        size_t count = 4096;
        env[0] = NULL;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in){
            printf("in: %s\n", l->in);
            int inr = Open(l->in, O_RDONLY, O_CLOEXEC);
            Dup2(STDIN_FILENO, inr);
            Close(inr);
		}
		if (l->out){
            printf("out: %s\n", l->out);
            int outr = Open(l->out,O_WRONLY, O_CLOEXEC);
            Dup2(STDOUT_FILENO, outr);
            close(outr);
		};

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char	**cmd = l->seq[i];

			if (!strcmp(cmd[0], "quit_bool")) {
               quit_bool=1;
			} else if (!quit_bool) {
			    size_t	arrayLength = sizeof(cmd) / sizeof(cmd[0]);		// Nombre d'éléments du tableau cmd
				pid_t pid;
                char **argvOrig = argv;
				// xrealloc(argv, sizeof(char **) *  arrayLength);	// Allocation du tableau des paramètres
                if(argv!=argvOrig){
                    free(argvOrig);
                }
				// Recopie des arguments dans un nouveau tableau argv
				//	Le tableau cmd comporte le nom de l'exécutable à lancer, ce qu'on ne veut pas dans le tableau des arguments

				else{
                    for (j=1; cmd[j]!=0; j++) {
                        argv[j-1] = malloc(sizeof(cmd[j]));				// Allocation de la mémoire pour copier l'argument courant
                        strcpy(argv[j-1], cmd[j]);							// Copie de la chaîne de caractères proprement dite
                    }
				}
                if(l->seq[i+1]){
                    if (pipe(p) == -1) {   // Connection des descripteurs de fichier
                        printf("Echec de la génération du tube");
                        exit(1);
                    }
                }
                pid=fork();     //  Création du fils
                if(pid==-1){
                    printf("Echec de la génération de processus fils");
                    exit(1);
                }
                else if(pid==0){     //  On éxecute dans le fils
                    if(l->seq[i+1]){        // Considération d'un pipe
                        Dup2(STDOUT_FILENO, p[0]);
                    }
                    //faut-il récuperer la sortie standard dans le cas où ceci est la derniere commande de la séquence?
                    Execve(cmd[0], &cmd[1], env);								// Exécution de la commande
                    perror("Echec de Execve");
                    exit(1);

                }
                else {
                        //on attend la mort du fils
                    int status = 0;
                    Wait(&status);
                    if(!WIFEXITED(status)){
                        strerror(WEXITSTATUS(status));
                    }
                    if(i>0){
                        while(read(p[0], p[1], count) != 0){
                        }
                        Dup2(STDIN_FILENO,p[1]);
                    }
                }
			}
		}
		if(i>1){        // Il y a plusieurs séquences, donc au moins un pipe
            close(p[1]);
            close(p[2]);
		}
		free(env);
	}
}
