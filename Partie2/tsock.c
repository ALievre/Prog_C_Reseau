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

#include "bal.h"
#include "tcp.h"
#include "socket.h"
#include "message.h"


int main (int argc, char **argv)
{
    int c;
    extern char *optarg;
    extern int optind;
    int nb_lettre = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    int lg_lettre = -1; /* Taille du messages à envoyer ou à recevoir, par défaut : 30 */
    int num_recepteur = 0;
    int fonction = -1; /* 0=recepteur, 1=emetteur, 2=bal */
    char TP[4] ;
    strcpy(TP, "TCP");

    //port = htons(port);
    if(argc > 2) {
        //Lecture de la ligne de commande
        while ((c = getopt(argc, argv, "be:r:n:sl:")) != -1) {
            switch (c) {
                case 'b':
                    if ((fonction == 0)||(fonction == 1)) {
                        printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    fonction = 2;
                    break;

                case 'e':
                    if ((fonction == 0)||(fonction == 2)) {
                        printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    fonction = 1;
                    num_recepteur = atoi(optarg);

                    break;

                case 'r':
                    if ((fonction == 2)||(fonction == 1)) {
                        printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
                        exit(1);
                    }
                    fonction = 0;
                    num_recepteur = atoi(optarg);
                    break;

                case 'n':
                    nb_lettre = atoi(optarg);
                    break;

                case 'l':
                    lg_lettre = atoi(optarg);
                    break;

                default:
                    printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
                    break;
            }
        }
    } else {
        printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
        exit(1);
    }

    int port = atoi(argv[argc-1]);

    char machine[sizeof(argv[argc-2])];
    strcpy(machine, argv[argc-2]);

    //Erreur si l'usager ne rentre pas le type de source
    if (fonction == -1) {
        printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
        exit(1) ;
    }

    //Si l'usager ne rentre pas de numéro de bal
    if (num_recepteur == -1) {
        printf("usage: cmd [-b|-e ## |-r ##][-n ##][-l ##][nom de la machine][n° de port]\n");
        exit(1) ;
    }

    //Si l'usager ne rentre pas de taille de msg, on met 30 par défaut
    if (lg_lettre == -1) {
        lg_lettre = 30;
    }

    //Si l'usager ne rentre pas de nombre de msg, on met par défaut 10 à envoyer et infini à recevoir
    if ((nb_lettre == -1) && (fonction == 1)) {
        nb_lettre = 10 ;
    }

    //Création d'un socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
    int sock_pdu = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );

    //Si on est récepteur et que la création du socket a fonctionné
    if((fonction == 0)&&(sock != -1)&&(sock_pdu != -1)){
        printf("RECEPTION: Demande de récupération de ses lettres par le récepteur %d, port=%d, TP=%s, dest=%s\n", num_recepteur, port, TP, machine);

        //Partie où on envoie le PDU
        // Déclaration de l'adresse distante et construction de l'adresse du socket
        struct sockaddr_in adr_distant;
        adr_distant = construire_adresse_socket_source(port);

        //Tentative de connexion
        int result_conn = -1;
        while (result_conn == -1) {
            result_conn = connect(sock_pdu, (struct sockaddr *) &adr_distant, sizeof(adr_distant));
        }

        //Ecriture et envoi du PDU
        for(int i = 1; i<=nb_lettre; i++) {
            envoyer_message_recepteur(lg_lettre, sock_pdu, adr_distant, num_recepteur);
        }

        close(sock_pdu);

        //Partie où on réceptionne les lettres
        //Déclaration de l'adresse locale et du message
        char msg[lg_lettre];
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
        if(nb_lettre == -1) {
            printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=infini, TP=%s\n", lg_lettre, port, TP);
            //Boucle infinie
            while(1) {
                //Acceptation de demande de connexion
                int result_accept = -1;
                while (result_accept == -1) {
                    result_accept = accept(sock, (struct sockaddr *) &adr_em, &lg_adr_em);
                }

                int result_read = -1;
                int i = 1;
                char *old_msg = (char *) malloc(lg_lettre * sizeof(char));

                //Lecture
                result_read = -1;
                while (result_read == -1) {
                    result_read = read(result_accept, pmsg, lg_lettre);
                    msg[lg_lettre] ='\0';
                }

                while (strcmp(old_msg, msg) != 0) {

                    //Si la réception n'a pas échouée, on affiche le message sur le terminal
                    if ((result_read != -1) && (strcmp(old_msg, msg) != 0)) {
                        printf("PUITS: Reception n°%d (%d)[%s]\n", i, lg_lettre, pmsg);
                        i++;
                        strcpy(old_msg, msg);
                    }

                    //Lecture
                    result_read = -1;
                    while (result_read == -1) {
                        result_read = read(result_accept, pmsg, lg_lettre);
                    }
                }
            }

            //Si le nombre de réception est défini par l'usager
        } else {
            printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", lg_lettre, port, nb_lettre, TP);

            //Acceptation de demande de connexion
            int result_accept = -1;
            while (result_accept == -1){
                result_accept = accept(sock, (struct sockaddr *)&adr_em, &lg_adr_em);
            }

            int result_read = -1;
            int i=1;

            //Boucle finie
            while(i<=nb_lettre){
                result_read = -1;
                //Lecture
                while(result_read == -1){
                    result_read = read(result_accept, pmsg, lg_lettre);
                    msg[lg_lettre] ='\0';
                }

                //Si la réception n'a pas échouée, on affiche le message sur le terminal
                if (result_read!=-1){
                    printf("PUITS: Reception n°%d (%d)[%s]\n", i, lg_lettre, pmsg);
                    i++;
                }
            }
            printf("PUITS: fin\n");
        }

        //Fermeture de la connexion
        int result_shutdwn = -1;

        while(result_shutdwn == -1){
            result_shutdwn = shutdown(sock,2);
        }

        //Si on est émetteur et que la création du socket a fonctionné
    } else if((fonction == 1)&&(sock != -1)){
        printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", lg_lettre, port, nb_lettre, TP, machine);

        // Déclaration de l'adresse distante et construction de l'adresse du socket
        struct sockaddr_in adr_distant;
        adr_distant = construire_adresse_socket_source(port);

        //Tentative de connexion
        int result_conn = -1;
        while (result_conn == -1) {
            result_conn = connect(sock, (struct sockaddr *) &adr_distant, sizeof(adr_distant));
        }

        //Ecriture et envoie du message
        char alphabet = 'a';
        for(int i = 1; i<=nb_lettre; i++) {
            envoyer_message_emetteur(i, lg_lettre, sock, nb_lettre, adr_distant, alphabet, num_recepteur);
        }

        //Fin de l'envoi
        printf("SOURCE: fin\n");

        //Fermeture de la connexion
        int result_shutdwn = -1;

        while(result_shutdwn == -1){
            result_shutdwn = shutdown(sock,2);
        }

        //Si on est bal et que la création du socket a fonctionné
    } else if((fonction == 2)&&(sock != -1)){

        printf("PUITS: port=%d, TP=%s\n", port, TP);

        //Déclaration de l'adresse locale et du PDU
        char pdu[lg_lettre];
        char * ppdu = pdu;

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

        //Acceptation de demande de connexion
        int result_accept = -1;
        while (result_accept == -1){
            result_accept = accept(sock, (struct sockaddr *)&adr_em, &lg_adr_em);
        }

        int result_read = -1;
        //Lecture
        while(result_read == -1){
            result_read = read(result_accept, ppdu, lg_lettre);
            pdu[lg_lettre] ='\0';
        }

        //Lecture du PDU
        int emetteur = -1;

        if (result_read!=-1){
            char *info = strtok(pdu, " ");
            printf("E : %s\n", info);
            if(strchr(info, 'e') != NULL){
                emetteur = 1;
                info = strtok(NULL, " ");
                printf("Num : %s\n", info);
                num_recepteur = atoi(info);
                info = strtok(NULL, " ");
                printf("Nb : %s\n", info);
                nb_lettre = atoi(info);
                info = strtok(NULL, " ");
                printf("Lg : %s\n", info);
                lg_lettre = atoi(info);
            } else if(strchr(info, 'r') != NULL){
                emetteur = 0;
                info = strtok(NULL, " ");
                num_recepteur = atoi(info);
            }
        }

        //Si la bal reçoit des messages
        if(emetteur == 1){
            printf("Infos PDU : e=%d, n°%d, nb lettres %d, lg lettre %d\n", emetteur, num_recepteur, nb_lettre, lg_lettre);

            //TODO

            //Si la bal envoie des messages
        } else if(emetteur == 0){
            printf("Infos PDU : e=%d, n°%d\n", emetteur, num_recepteur);

            //TODO

        }

        //Fermeture de la connexion
        int result_shutdwn = -1;

        while(result_shutdwn == -1){
            result_shutdwn = shutdown(sock,2);
        }

        //Si la création du socket a échoué, on arrête le programme
    } else {
        printf("Echec de la création du socket");
        exit(1);
    }

    return(0);
}
