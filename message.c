/* librairie standard ... */
#include <stdlib.h>
/* pour les entrées/sorties */
#include <stdio.h>

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
