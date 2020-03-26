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
//#include "udp.h"
//#include "tcp.h"

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
	int udp = -1; /* 0=tcp, 1=udp */
	int port = atoi(argv[argc-1]);
	char TP[4] ;
	char machine[sizeof(argv[argc-2])];

	strcpy(TP, "TCP");
	strcpy(machine, argv[argc-2]);	
	//port = htons(port);
	
	//Lecture de la ligne de commande
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
	
	//Erreur si l'usager ne rentre pas le type de source
	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##][-u][nom de la machine][n° de port]\n");
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
		int sock = make_socket(IPPROTO_UDP);

		//Si on est la source et que la création du socket à fonctionner
		if (source == 1 && sock != -1) {
			printf("SOURCE: lg_mesg_emis=%d, port=%d, nb_envois=%d, TP=%s, dest=%s\n", taille_msg, port, nb_message, TP, machine);
	
			// Déclaration de l'adresse distante
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

			char alphabet = 'a';
			int nb_entete = 5;

			//Envoie du message
			for(int i = 1; i<=nb_message; i++) {

				int nb_tiret;
				char msg[taille_msg];
				char msg_ent[taille_msg];
				char * pmsg = msg_ent;
				
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
				int result_send = sendto(sock, pmsg, taille_msg, 0,(struct sockaddr*)&adr_distant, sizeof(adr_distant));

				//Si l'envoi est réussi, on affiche les messages sur le terminal
				if (result_send!=-1){  
					printf("SOURCE: Envoi n°%d (%d)[%s]\n", i, taille_msg, msg_ent);
				}

				//Incrémentation des lettres de l'alphabet
				if(alphabet == 'z') {
					alphabet = 'a';
				} else alphabet++;
			}

			//Fin de l'envoi
			printf("SOURCE: fin\n");
			
		//Si on est le puits et que la création du socket à fonctionner
		} else if (sock != -1){ 
			//Si le nombre de réception est infini
			if(nb_message == -1) {
				printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=infini, TP=%s\n", taille_msg, port, TP);
				
				//Déclaration de l'adresse locale et du message
				struct sockaddr_in adr_local;
				char msg[taille_msg];
				char * pmsg = msg;

				//Déclaration de l'adresse distante et de sa taille
				struct sockaddr_in adr_em;
		    		socklen_t lg_adr_em;
				lg_adr_em = sizeof(struct sockaddr_in);

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

				int result_recv;
				int i=1;
					
				//Boucle infinie
				while(1){
					//Réception du message
					result_recv = recvfrom(sock, pmsg, taille_msg, 0,(struct sockaddr *) &adr_em, &lg_adr_em);

					//Si la réception n'a pas échouée, on affiche le message sur le terminal
					if (result_recv!=-1){  
						printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, msg);
						i++;
					}
				}
	
			//Si le nombre de réception est défini par l'usager
			} else {
				printf("PUITS: lg_mesg_lu=%d, port=%d, nb_receptions=%d, TP=%s\n", taille_msg, port, nb_message, TP);

				//Déclaration de l'adresse locale et du message
				struct sockaddr_in adr_local;
				char msg[taille_msg];
				char * pmsg = msg;

				//Déclaration de l'adresse distante et de sa taille
				struct sockaddr_in adr_em;
		    		socklen_t lg_adr_em;
				lg_adr_em = sizeof(struct sockaddr_in);

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

				int result_recv;
				int i = 1;
					
				//Boucle infinie
				while(i<=nb_message){
					//Réception du message
					result_recv = recvfrom(sock, pmsg, taille_msg, 0,(struct sockaddr *) &adr_em, &lg_adr_em);

					//Si la réception n'a pas échouée, on affiche le message sur le terminal
					if (result_recv != -1){  
						printf("PUITS: Reception n°%d (%d)[%s]\n", i, taille_msg, msg);
						i++;
					}
				}

			}
		
		//Si la création du socket a échoué, on arrête le programme
		} else {
			printf("Echec de la création du socket");
			exit(1);
		}

	//Usage du TCP
	} else {
		//TO DO
	}

	return(0);
}
