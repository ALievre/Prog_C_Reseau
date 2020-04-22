/* librairie standard ... */
#include <stdlib.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <string.h>

#include "bal.h"

int Creer_Liste_Bal(L_Bal *liste){

    liste->nbBal = 0;
    liste->first = NULL;
    liste->last = NULL;
    return 0;
 }

int Creer_Liste_Lettre(L_Lettre *liste){

    liste->nbLettres = 0;
    liste->first = NULL;
    liste->last = NULL;
    return 0;
}

int Vide_Bal(L_Bal *liste){
    return (liste->first == NULL);
}
int Vide_Lettre(L_Lettre *liste){
    return (liste->first == NULL);
}

void Ajouter_Fin_Bal(L_Bal *liste_bal, int num_bal){

    Bal * temp;

    //si la liste est vide, l'élément devient premier et dernier élément
    if(Vide_Bal(liste_bal)){
        liste_bal->last = (Bal *)malloc(sizeof(Bal));
        liste_bal->first = (Bal *)malloc(sizeof(Bal));
        liste_bal->last->numBal = num_bal;
        Creer_Liste_Lettre(&(liste_bal->last)->Lettres);
        liste_bal->last->next = NULL;
        liste_bal->first = liste_bal->last;
    }
        //si elle n'est pas vide, l'élément est bien le dernier élément
    else{
        temp = liste_bal->last;
        liste_bal->last = (Bal *)malloc(sizeof(Bal));
        liste_bal->last->numBal = num_bal;
        Creer_Liste_Lettre(&(liste_bal->last)->Lettres);
        liste_bal->last->next = NULL;
        temp->next = liste_bal->last;
    }
}

void Ajouter_Fin_Lettre(L_Lettre *liste_lettre, int num_lettre, char *pmsg){
    Lettre * temp;

    //si la liste est vide, l'élément devient premier et dernier élément
    if(Vide_Lettre(liste_lettre)){
        liste_lettre->last = (Lettre *)malloc(sizeof(Lettre));
        liste_lettre->first = (Lettre *)malloc(sizeof(Lettre));
        liste_lettre->last->numLettre = num_lettre;
        strcpy(&(liste_lettre->last)->message,pmsg);
        liste_lettre->last->next = NULL;
        liste_lettre->first = liste_lettre->last;
    }
        //si elle n'est pas vide, l'élément est bien le dernier élément
    else{
        temp = liste_lettre->last;
        liste_lettre->last = (Lettre *)malloc(sizeof(Lettre));
        liste_lettre->last->numLettre = num_lettre;
        strcpy(&(liste_lettre->last)->message,pmsg);
        liste_lettre->last->next = NULL;
        temp->next = liste_lettre->last;
    }
}

int Supprimer_Premier_Bal(L_Bal *liste){
    int resultat;
    Bal *aux;

    if(Vide_Bal(liste)){
        resultat = -1;
    } else {
        aux = liste->first;
        liste->first = liste->first->next;
        if(liste->first == NULL){
            liste->last = NULL;
        }

        free(aux);
        resultat = 0;
    }

    return resultat;
}
int Supprimer_Premier_Lettre(L_Lettre *liste){
    int resultat;
    Lettre *aux;

    if(Vide_Lettre(liste)){
        resultat = -1;
    } else {
        aux = liste->first;
        liste->first = liste->first->next;
        if(liste->first == NULL){
            liste->last = NULL;
        }

        free(aux);
        resultat = 0;
    }

    return resultat;
}


//int Lire_Premiere_Lettre(){}
