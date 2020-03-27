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

int creer_socket(int protocole) {
    if (protocole == IPPROTO_UDP){
        return socket(AF_INET, SOCK_DGRAM, protocole );
    } else if (protocole == IPPROTO_TCP) {
        return socket(AF_INET, SOCK_STREAM, protocole );
    }
    return -1;
}