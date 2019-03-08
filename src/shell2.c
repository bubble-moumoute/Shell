/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

int main()
{
    int quit_bool=0;

	while (!quit_bool) {
		struct cmdline *l;
		int i, j;
        char **argv = NULL;
        char **env = NULL;
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
		}
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char	**cmd = l->seq[i];

			if (!strcmp(cmd[0], "quit_bool")) {
               quit_bool=1;
			} else if (!quit_bool) {
			    size_t	arrayLength = sizeof(cmd) / sizeof(cmd[0]);		// Nombre d'éléments du tableau cmd
				pid_t pid;
                char **argvOrig = argv;
				xrealloc(argv, sizeof(char **) *  arrayLength);	// Allocation du tableau des paramètres
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
                pid=fork();
                if(pid==0){
                    if(l->out){
                        open(l->out,O_WRONLY, O_CLOEXEC);
                    }
                    Execve(cmd[0], argv, env);								// Exécution de la commande
                }
                else if(pid==-1){
                    printf("Echec de la génération de processus fils");
                }
                else {
                    int status = 0;
                    Wait(&status);
                    if(!WIFEXITED(status)){
                        strerror(WEXITSTATUS(status));
                    }
                }
			}
		}
		free(env);
        freeseq(&argv);
		freecmd(l);		// Fonction disponible dans readcmd
	}
}
