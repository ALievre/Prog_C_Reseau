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
#include "tcp.h"

void envoyer_message_emetteur(int i, int lg_lettre, int sock, int nb_lettre, struct sockaddr_in adr_distant,char alphab, int num_recept){
    char msg_ent[lg_lettre];
    char * pmsg_e = msg_ent;
    int nb_entete = 5;
    int nb_tiret;
    char alphabet = alphab;
    char msg[lg_lettre];
    char msg_info[lg_lettre];
    char * pmsg_info = msg_info;
    char num_r[nb_entete];
    char nb_l[nb_entete];
    char lg_l[nb_entete];

    sprintf(num_r, "%d", num_recept);
    sprintf(nb_l, "%d", nb_lettre);
    sprintf(lg_l, "%d", lg_lettre);

    strcpy(msg_info, 'e ');
    strncat(msg_info, num_r, strlen(num_r));
    strcat(msg_info, ' ');
    strncat(msg_info,nb_l, strlen(nb_l));
    strcat(msg_info, ' ');
    strncat(msg_info, lg_l, strlen(lg_l));

    printf("PDU : %s\n", msg_info);

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
            result_write = write(sock, pmsg_info, lg_lettre);
        }

        result_write = -1;

        //Ecriture du premier message sur le buffer
        while(result_write == -1){
            result_write = write(sock, pmsg_e, lg_lettre);
        }

    } else {

        int result_write = -1;

        //Ecriture du premier message sur le buffer
        while(result_write == -1){
            result_write = write(sock, pmsg_e, lg_lettre);
        }

    }

    //Si l'envoi est réussi, on affiche les messages sur le terminal
    if (result_write!=-1){
        printf("SOURCE: Envoi lettre n°%d à destination du récepteur n°!!!! (%d)[%s]\n", i, lg_lettre, msg_ent);
    }

}