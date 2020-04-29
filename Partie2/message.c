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
	for(i = 0; i<=lg; i++) {
        message[i] = motif;
	}
}

void afficher_message(char *message, int lg) {
	int i;
	printf("message construit : ");
	for(i = 0; i<lg; i++) printf("%c", message[i]);
	printf("\n");
}

void envoyer_message_emetteur(int i, int lg_lettre, int sock, int nb_lettre, char alphab, int num_recept){

    char msg_ent[lg_lettre];
    char * pmsg_e = msg_ent;
    int nb_entete = 5;
    int nb_tiret;
    char alphabet = alphab;
    char msg[lg_lettre];
    char pdu[lg_lettre] ;
    char * ppdu = pdu;
    char num_r[nb_entete];
    char nb_l[nb_entete];
    char lg_l[nb_entete];

    sprintf(num_r, "%d", num_recept);
    sprintf(nb_l, "%d", nb_lettre);
    sprintf(lg_l, "%d", lg_lettre);

    strcpy(pdu, "e ");
    strncat(pdu, num_r, strlen(num_r));
    strcat(pdu, " ");
    strncat(pdu,nb_l, strlen(nb_l));
    strcat(pdu, " ");
    strncat(pdu, lg_l, strlen(lg_l));

    printf("PDU : %s\n", pdu);

    //Incrémentation des lettres de l'alphabet
    if(alphabet == 'z') {
        alphabet = 'a';
    } else alphabet += (i-1)%26;

    //Réinitialisation de msg_ent
    memset(msg_ent, 0, sizeof(msg_ent));

    construire_message(msg, alphabet, lg_lettre);

    //Conversion du compteur en char
    char n_compteur[nb_entete];
    sprintf(n_compteur, "%d", i);

    nb_tiret = nb_entete - strlen(n_compteur);

    //Ecriture des tirets
    for(int j = 0; j<nb_tiret; j++) {
        msg_ent[j] = '-';
    }

    //Mise en place du message entier par concaténation
    strncat(msg_ent, n_compteur, strlen(n_compteur));
    strncat(msg_ent,msg, (lg_lettre - nb_entete));

    if(i == 1){
        int result_write = -1;

        //Ecriture du PDU sur le buffer
        while(result_write == -1){
            result_write = write(sock, ppdu, lg_lettre);
        }

        result_write = -1;

        //Ecriture du premier message sur le buffer
        while(result_write == -1){
            result_write = write(sock, pmsg_e, lg_lettre);
        }

        //Si l'envoi est réussi, on affiche les messages sur le terminal
        if (result_write!=-1){
            printf("EMISSION: Envoi lettre n°%d à destination du récepteur %d (%d)[%s]\n", num_recept, i, lg_lettre, msg_ent);
        }

    } else {

        int result_write = -1;

        //Ecriture du premier message sur le buffer
        while(result_write == -1){
            result_write = write(sock, pmsg_e, lg_lettre);
        }

        //Si l'envoi est réussi, on affiche les messages sur le terminal
        if (result_write!=-1){
            printf("EMISSION: Envoi lettre n°%d à destination du récepteur %d (%d)[%s]\n", i, num_recept, lg_lettre, msg_ent);
        }

    }

}

void envoyer_message_recepteur(int lg_lettre, int sock, int num_recept){

    char pdu[lg_lettre];
    char * ppdu = pdu;
    int nb_entete = 5;
    char num_r[nb_entete];

    sprintf(num_r, "%d", num_recept);

    strcpy(pdu, "r ");
    strncat(pdu, num_r, strlen(num_r));

    printf("PDU : %s\n", pdu);

    int result_write = -1;

    //Ecriture du PDU sur le buffer
    while(result_write == -1){
        result_write = write(sock, ppdu, lg_lettre);
    }
    //Si l'envoi est réussi, on affiche les messages sur le terminal
    if (result_write!=-1){
        printf("Envoi pdu réussi\n");
    }

}

void envoyer_message_bal(int num_lettre, int lg_lettre, int sock, int nb_lettre, char *pmsg, int num_recept){

    int result_write = -1;

    //Ecriture du premier message sur le buffer
    while(result_write == -1){
        result_write = write(sock, pmsg, lg_lettre);
    }

    //Si l'envoi est réussi, on affiche les messages sur le terminal
    if (result_write!=-1){
        printf("SERVEUR: Envoi lettre n°%d au récepteur %d (%d)[%s]\n", num_lettre, num_recept, lg_lettre, pmsg);
    }

    printf("envoie\n");

}