/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

int main()
{
    bool quit=false;

	while (!quit) {
		struct cmdline *l;
		int i, j;

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

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char	**cmd = l->seq[i];

			if (!strcmp(cmd[0], "quit")) {
               quit=true;
			} else if (!quit) {
				size_t	arrayLength = sizeof(cmd) / sizeof(cmd[0]);		// Nombre d'éléments du tableau cmd
				char **argv = xmalloc(sizeof(char **) * arrayLength);	// Allocation du tableau des paramètres
				char **env = xmalloc(sizeof(char **));					// Allocation du tableau des variables d'environnement
				env[0] = 0;												// Tableau vide

				// Recopie des arguments dans un nouveau tableau argv
				//	Le tableau cmd comporte le nom de l'exécutable à lancer, ce qu'on ne veut pas dans le tableau des arguments
                for (j=1; cmd[j]!=0; j++) {
					argv[j-1] = xmalloc(sizeof(cmd[j]));				// Allocation de la mémoire pour copier l'argument courant
					strcpy(argv[j-1], cmd[j]);							// Copie de la chaîne de caractères proprement dite
                    free(cmd[j]);										// Libération de l'argument courant
                }
				Execve(cmd[0], argv, env);								// Exécution de la commande
				free(l->seq[i])
			}
		}

		freecmd(l);		// Fonction disponible dans readcmd
	}
}
