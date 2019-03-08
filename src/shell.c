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
	{
	    int quit_bool=0;

		while (!quit_bool) {
			struct cmdline *l;
			int i;
			int p[2];
	        char **env = NULL;
	        env[0] = NULL;

			printf("shell> ");
            l = readcmd();
            pid_t pid = fork();

            if(pid==-1){
                printf("Echec de la génération de processus fils");
                exit(1);
            }
            else if (pid==0){       //  On éxécute dans le fils
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
                    Dup2(Open(l->in, O_RDONLY, O_CLOEXEC), STDIN_FILENO);
                }
                if (l->out){
                    printf("out: %s\n", l->out);
                    Dup2(Open(l->out,O_WRONLY, O_CLOEXEC),STDOUT_FILENO);
                };

                /* Display each command of the pipe */
                for (i=0; l->seq[i]!=0; i++) {
                    char	**cmd = l->seq[i];

                    if (!strcmp(cmd[0], "quit_bool")) {
                        quit_bool=1;
                    } else if (!quit_bool) {

                        if(l->seq[i+1]){
                            if (pipe(p) == -1) {   // Connection des descripteurs de fichier
                                printf("Echec de la génération du tube");
                                exit(1);
                            }
                        }

                        if(l->seq[i+1]){        // Considération d'un pipe
                            Dup2(p[0], STDOUT_FILENO);
                        }
                        else if (i>0) {     //  on arrive en fin de séquence et il y a au moins un pipe : réinitialisation de la sortie
                            Dup2(p[0],1);
                        }
                        Execve(cmd[0], &cmd[1], env);								// Exécution de la commande
                        perror("Echec de Execve");
                        exit(1);
                        if(l->seq[i+1]){
                            Dup2(p[1], STDIN_FILENO);
                        }
                    }
                }
                if(i>1){        // Il y a plusieurs séquences, donc au moins un pipe
                    Dup2(p[1],0);
                }
            }
            else{
                if(!l->en_fond){
                    int status = 0;
                    Wait(&status);
                    if(!WIFEXITED(status)){
                        strerror(WEXITSTATUS(status));
                    }
                }
            }
            free(env);
		}
	}
