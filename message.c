/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <string.h>

#include "message.h"

void construire_message(char *message, char motif, int lg) {
	int i;
	for(i = 0; i<lg; i++) message[i] = motif; 
}

void afficher_message(char *message, int lg) {
	int i;
	printf("message construit : ");
	for(i = 0; i<lg; i++) printf("%c", message[i]);
	printf("\n");
}
