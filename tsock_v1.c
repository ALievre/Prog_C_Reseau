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

int make_socket(int protocole) {
	if (protocole == IPPROTO_UDP){	
		return socket(AF_INET, SOCK_DGRAM, protocole );
	} else if (protocole == IPPROTO_TCP) {
		return socket(AF_INET, SOCK_STREAM, protocole );
	}
	return -1;
}

int main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int taille_msg = -1; /* Taille du messages à envoyer ou à recevoir, par défaut : 30 */
	int source = -1; /* 0=puits, 1=source */
	int udp = 0; /* 0=tcp, 1=udp */
	int port = atoi(argv[argc-1]);
	char TP[4] ;
	char machine[sizeof(argv[argc-2])];

	strcpy(TP, "TCP");
	strcpy(machine, argv[argc-2]);	
	//port = htons(port);

	while ((c = getopt(argc, argv, "pn:sul:")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##][-u][nom de la machine][n° de port]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##][-u][nom de la machine][n° de port]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;
		
		case 'u':
			printf("on utilise le protocole UDP\n");
			udp = 1;
			strcpy(TP, "UDP");
			break;
		case 'l':
			taille_msg = atoi(optarg);
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##][-u][nom de la machine][n° de port]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##][-u][nom de la machine][n° de port]\n");
		exit(1) ;
	}

	
	if (taille_msg == -1) {
		taille_msg = 30;
		printf("taille du message = 30 par défaut\n");
	}
	

	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}

	
	char msg[taille_msg];
	int nb_entete = 5;
	int nb_tiret = 0;
	char entete[taille_msg];
	char * pmsg = msg;

	if (udp == 1) {
		int sock = make_socket(IPPROTO_UDP);
		if (source == 1 && sock != -1) {
			printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", taille_msg, port, nb_message, TP, machine);
	
			// BUILDING DISTANT ADDR
			struct hostent *hp;
			struct sockaddr_in adr_distant;

			memset((char * )& adr_distant,0,sizeof(adr_distant));
			adr_distant.sin_family = AF_INET;
			adr_distant.sin_port = port;

			if((hp = gethostbyname("localhost"))==NULL) {
				printf("Erreur gethostbyname\n");
				exit(1);
			} 
			memcpy((char *)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);
			char alphabet = 'a';
			int compteur = 0;

			//sending msg
			for(int i = 1; i<=nb_message; i++) {
				construire_message(msg, alphabet, taille_msg);
				compteur++;
				/*//char char_i[sizeof((char) i)];
				//itoa(i, char_i,10);
				nb_tiret = nb_entete - sizeof((char) i);
				for(int j = 0; j<nb_tiret; j++) {					
					entete[nb_entete-j-1] = '-';
				}
				sprintf(entete, "%d", i);
				//strcat(*entete, (char) i);
				strcat(entete,msg); */
				int ret = sendto(sock, pmsg, taille_msg, 0,(struct sockaddr*)&adr_distant, sizeof(adr_distant));
				if (ret!=-1){  
					printf("SOURCE: Envoi n°%d (%d)[%s]\n", i, taille_msg, msg);
				}
				if(alphabet == 'z') {
					alphabet = 'a';
				} else alphabet++;
			}
			
		} else if (sock != -1){ 
			printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", taille_msg, port, nb_message, TP);

			struct sockaddr_in adr_local;

			// distant address + its length
			struct sockaddr_in adr_em;
    			socklen_t lg_adr_em;
			lg_adr_em = sizeof(struct sockaddr_in);

			memset((char * )& adr_local,0,sizeof(adr_local));
			adr_local.sin_family = AF_INET;
			adr_local.sin_port = port;
			adr_local.sin_addr.s_addr= INADDR_ANY;

			if(bind(sock,(struct sockaddr *)&adr_local, sizeof(adr_local)) ==-1) {
				printf("Echec du bind\n");
				exit(1);
			}

			int ret;
			int i=1;
			while(1){
				ret = recvfrom(sock, pmsg, taille_msg, 0,(struct sockaddr *) &adr_em, &lg_adr_em);
				if (ret!=-1){  
					printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, pmsg);
					i++;
				}
			}

		} else {
			printf("Echec de la création du socket");
			exit(1);
		}
	} else {
		//TODO
	}

	return(0);
}
