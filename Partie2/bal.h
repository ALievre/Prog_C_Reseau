typedef struct Lettre Lettre;
struct Lettre {
    int numLettre;
    int lgLettre;
    char message[100];
    Lettre *next;

};

typedef struct L_Lettre L_Lettre;
struct L_Lettre {
    int nbLettres;
    Lettre *first;
    Lettre *last;
};

typedef struct Bal Bal;
struct Bal {
    int numBal;
    L_Lettre Lettres;
    Bal *next;
};

typedef struct L_Bal L_Bal;
struct L_Bal {
    int nbBal;
    Bal *first;
    Bal *last;
};

int Creer_Liste_Bal(L_Bal *liste);
int Creer_Liste_Lettre(L_Lettre *liste);
int Vide_Bal(L_Bal *liste);
int Vide_Lettre(L_Lettre *liste);
void Ajouter_Fin_Bal(L_Bal *liste_bal, int num_bal);
void Ajouter_Fin_Lettre(L_Lettre *liste_lettre, int num_lettre, char *pmsg, int lg_lettre);
int Supprimer_Premier_Bal(L_Bal *liste);
int Supprimer_Premier_Lettre(L_Lettre *liste);
//int Lire_Premiere_Lettre();
void Afficher_Lettres(L_Lettre *liste);