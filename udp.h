struct sockaddr_in construire_adresse_socket_source(int port);
void envoyer_message(int i, int taille_msg, char * pmsg, int sock, int nb_message, struct sockaddr_in adr_distant, char alphab);
struct sockaddr_in construire_adresse_socket_puit(int port, int sockS);
