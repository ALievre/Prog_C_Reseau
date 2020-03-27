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

struct sockaddr_in construire_adresse_socket_source(int port){
    struct hostent *hp;
    struct sockaddr_in adr_distant;

    //Affectation du domaine et du n° de port
    memset((char * )& adr_distant,0,sizeof(adr_distant));
    adr_distant.sin_family = AF_INET;
    adr_distant.sin_port = port;

    //Affectation de l'adresse IP
    if((hp = gethostbyname("localhost"))==NULL) {
        printf("Erreur gethostbyname\n");
        exit(1);
    }
    memcpy((char *)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

    return adr_distant;
}

void envoyer_message(int i, int taille_msg, char * pmsg, int sock, int nb_message, struct sockaddr_in adr_distant,char alphab){
    int nb_entete = 5;
    int nb_tiret;
    char alphabet = alphab;
    char msg[taille_msg];
    char msg_ent[taille_msg];
    char * pmsg_e = msg_ent;

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

    *pmsg = *pmsg_e;
}

struct sockaddr_in construire_adresse_socket_puit(int port, int sock){
    struct sockaddr_in adr_local;

    //Construction de l'adresse du socket, du n° de port et de l'adresse IP
    memset((char * )& adr_local,0,sizeof(adr_local));
    adr_local.sin_family = AF_INET;
    adr_local.sin_port = port;
    adr_local.sin_addr.s_addr= INADDR_ANY;

    //Association de l'adresse du socket et de sa représentation interne
    if(bind(sock,(struct sockaddr *)&adr_local, sizeof(adr_local)) ==-1) {
        printf("Echec du bind\n");
        exit(1);
    }
}