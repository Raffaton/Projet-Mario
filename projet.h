#ifndef PROJET_H
#define PROJET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define FICHIER_SAUVEGARDE "sauvegarde.txt"

typedef struct {
    int positionX;
    int positionY;
    char nom[100];
    int en_saut;
    int score;
} Personnage;

void gerer_saut(FILE *fichier, Personnage *perso, int largeur, int direction);
void effacer_position(FILE *fichier, Personnage *perso, int largeur);
void mettre_position(FILE *fichier, Personnage *perso, int largeur);
void copierFichier(const char *source, const char *destination);
void deplacer_joueur(FILE *fichier, Personnage *perso, int largeur);
void caracterePaysage(char caractereActuel);
void afficherPaysage(FILE *fichier, int positionJoueur);
void menuPrincipal(const char *fichierTemp);
void jouer(const char *fichierTemp, Personnage *perso);
void sauvegarderPartie(Personnage *perso);
int chargerPartie(Personnage *perso);
void menuSauvegarde(Personnage *perso, const char *fichierTemp);
void afficherScores();

#endif