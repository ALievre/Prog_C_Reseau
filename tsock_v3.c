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
    int source = -1; /* 0=puits, 1=source */
    int udp = -1; /* 0=tcp, 1=udp */
    char TP[4] ;

    //port = htons(port);
    if(argc > 2) {
        //Lecture de la ligne de commande
        while ((c = getopt(argc, argv, "pn:sul:")) != -1) {
            switch (c) {
                case 'p':
                    if (source == 1) {
                        printf("usage: cmd [-p|-s][-n ##][-u][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    source = 0;
                    break;

                case 's':
                    if (source == 0) {
                        printf("usage: cmd [-p|-s][-n ##][-u][-l ##][nom de la machine][n° de port]\n");
                        exit(1) ;
                    }
                    source = 1;
                    break;

                case 'n':
                    nb_message = atoi(optarg);
                    break;

                case 'u':
                    udp = 1;
                    strcpy(TP, "UDP");
                    break;
                case 'l':
                    taille_msg = atoi(optarg);
                    break;

                default:
                    printf("usage: cmd [-p|-s][-n ##][-u][-l ##][nom de la machine][n° de port]\n");
                    break;
            }
        }
    } else {
        printf("usage: cmd [-p|-s][-n ##][-u][-l ##][nom de la machine][n° de port]\n");
        exit(1);
    }

    int port = atoi(argv[argc-1]);
    char machine[sizeof(argv[argc-2])];

    strcpy(TP, "TCP");
    strcpy(machine, argv[argc-2]);

    //Erreur si l'usager ne rentre pas le type de source
    if (source == -1) {
        printf("usage: cmd [-p|-s][-n ##][-u][-l ##][nom de la machine][n° de port]\n");
        exit(1) ;
    }

    //Si l'usager ne rentre pas de taille de msg, on met 30 par défaut
    if (taille_msg == -1) {
        taille_msg = 30;
    }

    //Si l'usager ne rentre pas de TP, on met TCP par défaut
    if (udp == -1) {
        udp = 0;
    }

    //Si l'usager ne rentre pas de nombre de msg, on met par défaut 10 à envoyer et infini à recevoir
    if ((nb_message == -1) && (source == 1)) {
        nb_message = 10 ;
    }

    //Usage de l'UDP
    if (udp == 1) {
        //Création d'un socket
        int sock = creer_socket(IPPROTO_UDP);

        //Si on est la source et que la création du socket à fonctionner
        if (source == 1 && sock != -1) {
            printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", taille_msg, port, nb_message, TP, machine);

            // Déclaration de l'adresse distante et construction de l'adresse du socket
            struct sockaddr_in adr_distant;
            adr_distant = construire_adresse_socket_source(port);

            //Envoie du message
            char alphabet = 'a';

            for(int i = 1; i<=nb_message; i++) {
                envoyer_message_udp(i, taille_msg, sock, nb_message, adr_distant, alphabet);
            }

            //Fin de l'envoi
            printf("SOURCE: fin\n");

            //Si on est le puits et que la création du socket à fonctionner
        } else if (sock != -1){

            //Déclaration de l'adresse locale et du message
            char msg[taille_msg];
            char * pmsg = msg;

            //Déclaration de l'adresse distante et de sa taille
            struct sockaddr_in adr_em;
            socklen_t lg_adr_em;
            lg_adr_em = sizeof(struct sockaddr_in);

            //Construction et association
            adr_em = construire_adresse_socket_puit(port, sock);

            int result_recv;
            int i=1;

            //Si le nombre de réception est infini
            if(nb_message == -1) {
                printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=infini, TP=%s\n", taille_msg, port, TP);

                //Boucle infinie
                while(1){
                    //Réception du message
                    result_recv = recvfrom(sock, pmsg, taille_msg, 0,(struct sockaddr *) &adr_em, &lg_adr_em);
                    msg[taille_msg] ='\0';

                    //Si la réception n'a pas échouée, on affiche le message sur le terminal
                    if (result_recv!=-1){
                        printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, msg);
                        i++;
                    }
                }

                //Si le nombre de réception est défini par l'usager
            } else {
                printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", taille_msg, port, nb_message, TP);

                //Boucle finie
                while(i<=nb_message){
                    //Réception du message
                    result_recv = recvfrom(sock, pmsg, taille_msg, 0,(struct sockaddr *) &adr_em, &lg_adr_em);
                    msg[taille_msg] ='\0';

                    //Si la réception n'a pas échouée, on affiche le message sur le terminal
                    if (result_recv != -1){
                        printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, msg);
                        i++;
                    }
                }
            }
            printf("PUITS: fin\n");

            //Si la création du socket a échoué, on arrête le programme
        } else {
            printf("Echec de la création du socket");
            exit(1);
        }

        //Usage du TCP
    } else {
        //Création d'un socket
        int sock = creer_socket(IPPROTO_TCP);

        //Si on est la source et que la création du socket a fonctionné
        if (source == 1 && sock != -1) {
            printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", taille_msg, port, nb_message, TP, machine);

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
            printf("SOURCE: fin\n");

            //Fermeture de la connexion
            int result_shutdwn = -1;

            while(result_shutdwn == -1){
                result_shutdwn = shutdown(sock,1);
            }

            //Si on est le puits et que la création du socket à fonctionner
        } else if (sock != -1){

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

            //Si le nombre de réception est infini
            if(nb_message == -1) {
                printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=infini, TP=%s\n", taille_msg, port, TP);
                //Boucle infinie
                while(1) {
                    //Acceptation de demande de connexion
                    int result_accept = -1;
                    while (result_accept == -1) {
                        result_accept = accept(sock, (struct sockaddr *) &adr_em, &lg_adr_em);
                    }

                    int result_read = -1;
                    int i = 1;
                    char *old_msg = (char *) malloc(taille_msg * sizeof(char));

                    //Lecture
                    result_read = -1;
                    while (result_read == -1) {
                        result_read = read(result_accept, pmsg, taille_msg);
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
                            result_read = read(result_accept, pmsg, taille_msg);
                        }
                    }
                }

                //Si le nombre de réception est défini par l'usager
            } else {
                printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", taille_msg, port, nb_message, TP);

                //Acceptation de demande de connexion
                int result_accept = -1;
                while (result_accept == -1){
                    result_accept = accept(sock, (struct sockaddr *)&adr_em, &lg_adr_em);
                }

                int result_read = -1;
                int i=1;

                //Boucle finie
                while(i<=nb_message){
                    result_read = -1;
                    //Lecture
                    while(result_read == -1){
                        result_read = read(result_accept, pmsg, taille_msg);
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

            //Si la création du socket a échoué, on arrête le programme
        } else {
            printf("Echec de la création du socket");
            exit(1);
        }
    }

    return(0);
}
