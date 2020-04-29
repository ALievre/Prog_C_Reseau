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


    L_Bal La_Bal;
    Creer_Liste_Bal(&La_Bal);

    //Si on est récepteur et que la création du socket a fonctionné
    if((fonction == 0)&&(sock != -1)){
        printf("RECEPTION: Demande de récupération de ses lettres par le récepteur %d, port=%d, TP=%s, dest=%s\n", num_recepteur, port, TP, machine);

        //Partie où on envoie le PDU
        // Déclaration de l'adresse distante et construction de l'adresse du socket
        struct sockaddr_in adr;
        adr = construire_adresse_socket_source(port);

        //Tentative de connexion
        int result_conn = -1;
        while (result_conn == -1) {
            result_conn = connect(sock, (struct sockaddr *) &adr, sizeof(adr));
        }

        printf("Je suis connecté\n");

        //Ecriture et envoi du PDU
        envoyer_message_recepteur(lg_lettre, sock, num_recepteur);

        printf("J'ai envoyé le pdu\n");

        //Partie où on réceptionne les lettres
        //Déclaration de l'adresse locale et du message
        char msg[lg_lettre];
        char * pmsg = msg;

        //Déclaration de l'adresse distante et de sa taille
        //struct sockaddr_in adr_em;
        socklen_t lg_adr;
        lg_adr = sizeof(struct sockaddr_in);
/*
        //Construction et association
        //adr = construire_adresse_socket_puit(port, sock);

        //Dimensionnement de la file d'attente de demandes de connexions
        int result_listen = -1;
        while(result_listen == -1){
            result_listen = listen(sock, 5);
        }
*/
        //Si le nombre de réception est infini
        if(nb_lettre == -1) {
            //printf("RECEPTION: lg_mesg_lu=%d, port=%d, nb_receptions=infini, TP=%s\n", lg_lettre, port, TP);
            int first = 1;

            //Boucle infinie
            while(1) {

                if(first){

                    int result_read = -1;
                    int i = 1;
                    char *old_msg = (char *) malloc(lg_lettre * sizeof(char));

                    //Lecture
                    result_read = -1;
                    while (result_read == -1) {
                        result_read = read(sock, pmsg, lg_lettre);
                        msg[lg_lettre] ='\0';
                    }

                    while (strcmp(old_msg, msg) != 0) {

                        //Si la réception n'a pas échouée, on affiche le message sur le terminal
                        if ((result_read != -1) && (strcmp(old_msg, msg) != 0)) {
                            printf("RECEPTION: Reception n°%d (%d)[%s]\n", i, lg_lettre, pmsg);
                            i++;
                            strcpy(old_msg, msg);
                        }

                        //Lecture
                        result_read = -1;
                        while (result_read == -1) {
                            result_read = read(sock, pmsg, lg_lettre);
                        }
                    }

                    //Fermeture de la connexion
                    int result_shutdwn = -1;

                    while(result_shutdwn == -1){
                        result_shutdwn = shutdown(sock,2);
                    }

                    first = 0;
                } else {
                    //Acceptation de demande de connexion
                    int result_accept = -1;
                    while (result_accept == -1) {
                        result_accept = accept(sock, (struct sockaddr *) &adr, &lg_adr);
                    }

                    int result_read = -1;
                    int i = 1;
                    char *old_msg = (char *) malloc(lg_lettre * sizeof(char));

                    //Lecture
                    result_read = -1;
                    while (result_read == -1) {
                        result_read = read(sock, pmsg, lg_lettre);
                        msg[lg_lettre] ='\0';
                    }

                    while (strcmp(old_msg, msg) != 0) {

                        //Si la réception n'a pas échouée, on affiche le message sur le terminal
                        if ((result_read != -1) && (strcmp(old_msg, msg) != 0)) {
                            printf("RECEPTION: Reception n°%d (%d)[%s]\n", i, lg_lettre, pmsg);
                            i++;
                            strcpy(old_msg, msg);
                        }

                        //Lecture
                        result_read = -1;
                        while (result_read == -1) {
                            result_read = read(sock, pmsg, lg_lettre);
                        }
                    }

                    //Fermeture de la connexion
                    int result_shutdwn = -1;

                    while(result_shutdwn == -1){
                        result_shutdwn = shutdown(sock,2);
                    }
                }

            }

            //Si le nombre de réception est défini par l'usager
        } else {
            //printf("RECEPTION: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", lg_lettre, port, nb_lettre, TP);
/*
            //Acceptation de demande de connexion
            int result_accept = -1;
            while (result_accept == -1){
                result_accept = accept(sock, (struct sockaddr *)&adr, &lg_adr);
            }*/

            int result_read = -1;
            int i=1;

            //Boucle finie
            while(i<=nb_lettre){
                result_read = -1;
                //Lecture
                while(result_read == -1){
                    result_read = read(sock, pmsg, lg_lettre);
                    msg[lg_lettre] ='\0';
                }

                //Si la réception n'a pas échouée, on affiche le message sur le terminal
                if (result_read!=-1){
                    printf("RECEPTION: Reception n°%d (%d)[%s]\n", i, lg_lettre, pmsg);
                    i++;
                }
            }
            printf("RECEPTION: fin\n");
        }

        printf("pre shutdown\n");

        //Fermeture de la connexion
        int result_shutdwn = -1;

        while(result_shutdwn == -1){
            result_shutdwn = shutdown(sock,2);
        }

        printf("post shutdown\n");

        //Si on est émetteur et que la création du socket a fonctionné
    } else if((fonction == 1)&&(sock != -1)){
        printf("EMISSION: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", lg_lettre, port, nb_lettre, TP, machine);

        // Déclaration de l'adresse distante et construction de l'adresse du socket
        struct sockaddr_in adr;
        adr = construire_adresse_socket_source(port);

        //Tentative de connexion
        int result_conn = -1;
        while (result_conn == -1) {
            result_conn = connect(sock, (struct sockaddr *) &adr, sizeof(adr));
        }

        //Ecriture et envoie du message
        char alphabet = 'a';
        for(int i = 1; i<=nb_lettre; i++) {
            envoyer_message_emetteur(i, lg_lettre, sock, nb_lettre, alphabet, num_recepteur);
        }

        //Fin de l'envoi
        printf("EMISSION: fin\n");

        //Fermeture de la connexion
        int result_shutdwn = -1;

        while(result_shutdwn == -1){
           result_shutdwn = shutdown(sock,1);
        }

        printf("J'ai shutdown\n");

        //Si on est bal et que la création du socket a fonctionné
    } else if((fonction == 2)&&(sock != -1)){

        printf("SERVEUR: port=%d, TP=%s\n", port, TP);

        //Déclaration de l'adresse distante et de sa taille
        struct sockaddr_in adr;
        socklen_t lg_adr;
        lg_adr = sizeof(struct sockaddr_in);

        //Construction et association
        adr = construire_adresse_socket_puit(port, sock);

        while(1){

            //Déclaration de l'adresse locale et du PDU
            char pdu[lg_lettre];
            char * ppdu = pdu;

            //Réinitialisation du pdu
            memset(pdu, 0, sizeof(pdu));

            //Dimensionnement de la file d'attente de demandes de connexions
            int result_listen = -1;
            while(result_listen == -1){
                result_listen = listen(sock, 5);
            }
            printf("J'ai listen\n");

            //Acceptation de demande de connexion
            int result_accept = -1;
            while (result_accept == -1){
                result_accept = accept(sock, (struct sockaddr *)&adr, &lg_adr);
            }
            printf("J'ai accept\n");
            //Lecture
            int result_read = -1;
            while(result_read == -1){
                result_read = read(result_accept, ppdu, lg_lettre);
                pdu[lg_lettre] ='\0';
            }
            printf("J'ai lu le pdu\n");
            //Lecture du PDU
            int emetteur = -1;

            if (result_read!=-1){
                char *info = strtok(pdu, " ");
                printf("E : %s !\n", info);
                if(strchr(info, 'e') != NULL){
                    emetteur = 1;
                    info = strtok(NULL, " ");
                    printf("Num : %s !\n", info);
                    num_recepteur = atoi(info);
                    info = strtok(NULL, " ");
                    printf("Nb : %s !\n", info);
                    nb_lettre = atoi(info);
                    info = strtok(NULL, " ");
                    printf("Lg : %s !\n", info);
                    lg_lettre = atoi(info);
                } else if(strchr(info, 'r') != NULL){
                    emetteur = 0;
                    info = strtok(NULL, " ");
                    num_recepteur = atoi(info);
                }
            }

            //Si la bal reçoit des messages
            if(emetteur == 1){

                //Partie où on réceptionne les lettres
                //Déclaration de l'adresse locale et du message
                char msg[lg_lettre];
                char * pmsg = msg;

                printf("Infos PDU : e=%d, n°%d, nb lettres %d, lg lettre %d\n", emetteur, num_recepteur, nb_lettre, lg_lettre);

                int result_read = -1;
                int i=1;

                //Gestion de la boîte aux lettres
                Bal *tmp = La_Bal.first;
                int trouve = 0;

                //Ajoute la Bal si elle n'existe pas déjà
                if(Vide_Bal(&La_Bal) == 0){
                    while((tmp != NULL) && (trouve == 0)){
                        if(tmp->numBal != num_recepteur){
                            tmp = tmp->next;
                        } else {
                            trouve = 1;
                        }
                    }
                }

                if(trouve == 0){
                    Ajouter_Fin_Bal(&La_Bal, num_recepteur);
                    printf("j'ai ajouté une bal \n");
                }

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
                        printf("SERVEUR: Reception et stockage lettre n°%d pour le récepteur n°%d [%s]\n", i, num_recepteur, pmsg);

                        //Ajoute la lettre
                        if(trouve == 0){
                            Ajouter_Fin_Lettre(&La_Bal.last->Lettres, i, pmsg, lg_lettre);
                        } else {
                            Ajouter_Fin_Lettre(&tmp->Lettres, i, pmsg, lg_lettre);
                        }

                        i++;
                    }

                }
                printf("il y a %d bal\n", La_Bal.nbBal);
                if(trouve == 0){
                    printf("Lecture de la bal n°%d : \n", La_Bal.last->numBal);
                    Afficher_Lettres(&La_Bal.last->Lettres);
                } else {
                    printf("Lecture de la bal n°%d : \n", tmp->numBal);
                    Afficher_Lettres(&tmp->Lettres);
                }

                //Fermeture de la connexion
                int result_shutdwn = -1;

                while(result_shutdwn == -1){
                    result_shutdwn = shutdown(sock,0);
                }

                //Si la bal envoie des messages
            } else if(emetteur == 0){
                printf("Infos PDU : e=%d, n°%d\n", emetteur, num_recepteur);

                //On cherche la bonne bal
                Bal *tmp = La_Bal.first;
                int trouve = 0;

                if(Vide_Bal(&La_Bal) == 0){
                    while((tmp != NULL) && (trouve == 0)){
                        if(tmp->numBal != num_recepteur){
                            tmp = tmp->next;
                        } else {
                            trouve = 1;
                        }
                    }
                }

                printf("trouve = %d\n", trouve);

                if((trouve == 0)||(Vide_Lettre(&(tmp)->Lettres) == 1)){
                    char *pmsg = "La boîte aux lettres est vide.\n";
                    printf("La boîte aux lettres est vide.\n");
                    envoyer_message_bal(0, 30, result_accept, 0, pmsg, num_recepteur);
                } else if(trouve == 1){

                    Lettre *actuel = tmp->Lettres.first;
                    printf("début envoi sg\n");
                    while(actuel != NULL){
                        //Envoie du message
                        //for(int i = 1; i<=tmp->Lettres.nbLettres; i++) {
                            char * pmsg = actuel->message;
                            envoyer_message_bal(actuel->numLettre, actuel->lgLettre, result_accept, tmp->Lettres.nbLettres, pmsg, num_recepteur);
                        //}
                        printf("suppression msg\n");
                        if(Supprimer_Premier_Lettre(&(tmp)->Lettres) == 0){
                            actuel = tmp->Lettres.first;
                        }
                    }
                    printf("fin envoi \n");
                }

                printf("debut shutdown \n");
                //Fermeture de la connexion
                int result_shutdwn = -1;

                while(result_shutdwn == -1){
                    result_shutdwn = shutdown(sock,0);
                }

                printf("fin shutdown\n");
            }

        }

        //Si la création du socket a échoué, on arrête le programme
    } else {
        printf("Echec de la création du socket");
        exit(1);
    }
    printf("FIN FICHIER");
    return(0);
}
