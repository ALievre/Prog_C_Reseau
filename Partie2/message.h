void construire_message(char *message, char motif, int lg);
void afficher_message(char *message, int lg);
void envoyer_message_emetteur(int i, int lg_lettre, int sock, int nb_lettre, char alphab, int num_recept);
void envoyer_message_recepteur(int lg_lettre, int sock, int num_recept);
void envoyer_message_bal(int num_lettre, int lg_lettre, int sock, int nb_lettre, char *pmsg, int num_recept);