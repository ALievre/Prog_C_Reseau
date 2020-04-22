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

#include "socket.h"

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

    return adr_local;
}