void construire_message(char *message, char motif, int lg);
void afficher_message(char *message, int lg);
void envoyer_message_emetteur(int i, int lg_lettre, int sock, int nb_lettre, struct sockaddr_in adr_distant, char alphab, int num_recept);
void envoyer_message_recepteur(int lg_lettre, int sock, struct sockaddr_in adr_distant, int num_recept);