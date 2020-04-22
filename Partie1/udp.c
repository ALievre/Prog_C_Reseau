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
#include "udp.h"


void envoyer_message_udp(int i, int taille_msg, int sock, int nb_message, struct sockaddr_in adr_distant,char alphab){
    char msg_ent[taille_msg];
    char * pmsg_e = msg_ent;
    int nb_entete = 5;
    int nb_tiret;
    char alphabet = alphab;
    char msg[taille_msg];

    //Incrémentation des lettres de l'alphabet
    if(alphabet == 'z') {
        alphabet = 'a';
    } else alphabet += (i-1)%26;

    //Réinitialisation de msg_ent
    memset(msg_ent, 0, sizeof(msg_ent));

    construire_message(msg, alphabet, taille_msg);

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
    strncat(msg_ent,msg, (taille_msg - nb_entete));

    //Envoi du message
    int result_send = sendto(sock, pmsg_e, taille_msg, 0,(struct sockaddr*)&adr_distant, sizeof(adr_distant));

    //Si l'envoi est réussi, on affiche les messages sur le terminal
    if (result_send!=-1){
        printf("SOURCE: Envoi n°%d (%d)[%s]\n", i, taille_msg, msg_ent);
    }
}

