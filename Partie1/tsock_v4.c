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

#include "udp.h"
#include "tcp.h"
#include "socket.h"
#include "message.h"


int main (int argc, char **argv)
{
    int c;
    extern char *optarg;
    extern int optind;
    int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    int taille_msg = -1; /* Taille du messages à envoyer ou à recevoir, par défaut : 30 */
    int client = -1; /* 0=serveur, 1=client */
    int recept = -1; /* 0=emetteur, 1=recepteur */
    char TP[4];
    strcpy(TP, "TCP");

    //port = htons(port);
    if(argc > 2) {
        //Lecture de la ligne de commande
        while ((c = getopt(argc, argv, "n:scerl:")) != -1) {
            switch (c) {
                case 's':
                    if (client == 1) {
                        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    client = 0;
                    break;

                case 'c':
                    if (client == 0) {
                        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    client = 1;
                    break;

                case 'n':
                    nb_message = atoi(optarg);
                    break;

                case 'e':
                    if (recept == 1) {
                        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    recept = 0;
                    break;

                case 'r':
                    if (recept == 0) {
                        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    recept = 1;
                    break;

                case 'l':
                    taille_msg = atoi(optarg);
                    break;

                default:
                    printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
                    break;
            }
        }
    } else {
        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
        exit(1);
    }

    int port = atoi(argv[argc-1]);
    char machine[sizeof(argv[argc-2])];

    strcpy(machine, argv[argc-2]);

    //Erreur si l'usager ne rentre pas le type : client ou serveur
    if (client == -1) {
        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
        exit(1);
    }

    //Si l'usager ne rentre pas de taille de msg, on met 30 par défaut
    if (taille_msg == -1) {
        taille_msg = 30;
    }

    //Si l'usager ne rentre pas de fonction alors qu'il est client
    if (client && (recept == -1)) {
        printf("usage: cmd [-s|-c][-e|-r][-n ##][-l ##][nom de la machine][n° de port]\n");
        exit(1);
    }

    //Si l'usager ne rentre pas de nombre de msg, on met par défaut 10 à envoyer et infini à recevoir
    if ((nb_message == -1) && (client == 1)) {
        nb_message = 10 ;
    }

    //Création d'un socket
    int sock = creer_socket(IPPROTO_TCP);

    //Si on est la client et que la création du socket a fonctionné
    if (client == 1 && sock != -1) {
        if(recept == 0){
            printf("CLIENT EMETTEUR: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", taille_msg, port, nb_message, TP, machine);

            // Déclaration de l'adresse distante et construction de l'adresse du socket
            struct sockaddr_in adr_distant;
            adr_distant = construire_adresse_socket_source(port);

            //Tentative de connexion
            int result_conn = -1;
            while (result_conn == -1) {
                result_conn = connect(sock, (struct sockaddr *) &adr_distant, sizeof(adr_distant));
            }

            //Envoie du message
            char alphabet = 'a';

            //Ecriture du message
            for(int i = 1; i<=nb_message; i++) {
                envoyer_message_tcp(i, taille_msg, sock, nb_message, adr_distant, alphabet);
            }

            //Fin de l'envoi
            printf("CLIENT EMETTEUR: fin\n");

            //Fermeture de la connexion
            int result_shutdwn = -1;

            while(result_shutdwn == -1){
                result_shutdwn = shutdown(sock,1);
            }
        } else if(recept == 1){
            //Déclaration de l'adresse locale et du message
            char msg[taille_msg];
            char * pmsg = msg;

            //Déclaration de l'adresse distante et de sa taille
            struct sockaddr_in adr_em;
            socklen_t lg_adr_em;
            lg_adr_em = sizeof(struct sockaddr_in);

            //Construction et association
            adr_em = construire_adresse_socket_puit(port, sock);

            //Dimensionnement de la file d'attente de demandes de connexions
            int result_listen = -1;
            while(result_listen == -1){
                result_listen = listen(sock, 5);
            }

            int sock_bis;

            //Si le nombre de réception est infini
            if(nb_message == -1) {
                printf("CLIENT RECEPTEUR: lg_mesg_lu=%d, port=%d, nb_receptions=infini, TP=%s\n", taille_msg, port, TP);

                //Boucle infinie
                while(1) {

                    //Acceptation de demande de connexion
                    int sock_bis = -1;
                    while (sock_bis == -1) {
                        sock_bis = accept(sock, (struct sockaddr *) &adr_em, &lg_adr_em);
                    }

                    int result_read = -1;
                    int i = 1;
                    char *old_msg = (char *) malloc(taille_msg * sizeof(char));

                    //Lecture
                    result_read = -1;
                    while (result_read == -1) {
                        result_read = read(sock_bis, pmsg, taille_msg);
                        msg[taille_msg] ='\0';
                    }

                    while (strcmp(old_msg, msg) != 0) {

                        //Si la réception n'a pas échouée, on affiche le message sur le terminal
                        if ((result_read != -1) && (strcmp(old_msg, msg) != 0)) {
                            printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, pmsg);
                            i++;
                            strcpy(old_msg, msg);
                        }

                        //Lecture
                        result_read = -1;
                        while (result_read == -1) {
                            result_read = read(sock_bis, pmsg, taille_msg);
                        }
                    }
                }

                //Si le nombre de réception est défini par l'usager
            } else {
                printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", taille_msg, port, nb_message, TP);

                //Acceptation de demande de connexion
                int sock_bis = -1;
                while (sock_bis == -1){
                    sock_bis = accept(sock, (struct sockaddr *)&adr_em, &lg_adr_em);
                }

                int result_read = -1;
                int i=1;

                //Boucle finie
                while(i<=nb_message){
                    result_read = -1;
                    //Lecture
                    while(result_read == -1){
                        result_read = read(sock_bis, pmsg, taille_msg);
                        msg[taille_msg] ='\0';
                    }

                    //Si la réception n'a pas échouée, on affiche le message sur le terminal
                    if (result_read!=-1){
                        printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, pmsg);
                        i++;
                    }
                }
                printf("PUITS: fin\n");
            }

            //Fermeture de la connexion
            int result_shutdwn = -1;

            while(result_shutdwn == -1){
                result_shutdwn = shutdown(sock,0);
            }
        }

        //Si on est le serveur et que la création du socket à fonctionner
    } else if ((client == 0) && (sock != -1)) {
        //TODO

    }
        //Si la création du socket a échoué, on arrête le programme
    } else {
        printf("Echec de la création du socket");
        exit(1);
    }

    return(0);
}
