#include "projet.h"

char* creerNomFichierTemp(const char* nomJoueur) {
    char* fichierTemp = malloc(strlen(nomJoueur) + 9); 
    if (fichierTemp == NULL) {
        printf("Erreur d'allocation mémoire\n");
        exit(1);
    }
    sprintf(fichierTemp, "temp_%s.txt", nomJoueur);
    return fichierTemp;
}

void effacer_position(FILE *fichier, Personnage *perso, int largeur) {
    fseek(fichier, (perso->positionY) * largeur + perso->positionX, SEEK_SET);
    fputc(' ', fichier);
}

void mettre_position(FILE *fichier, Personnage *perso, int largeur) {
    fseek(fichier, (perso->positionY) * largeur + perso->positionX, SEEK_SET);
    fputc('M', fichier);
}

void cacherCurseur() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.bVisible = FALSE;
    info.dwSize = 1;
    SetConsoleCursorInfo(console, &info);
}

void gerer_saut(FILE *fichier, Personnage *perso, int largeur, int direction) {
    int hauteur_saut[8] = {-1, -1, -1, -1, 1, 1, 1, 1}; 
    
    for (int i = 0; i < 8; i++) {
        int nouv_x = perso->positionX + direction;
        int nouv_y = perso->positionY + hauteur_saut[i];
        
        fseek(fichier, nouv_y * largeur + nouv_x, SEEK_SET);
        char c = fgetc(fichier);
        if (c == 'c') {
            perso->score++;
            fseek(fichier, nouv_y * largeur + nouv_x, SEEK_SET);
            fputc(' ', fichier);
        }
        
        char caractere_destination;
        fseek(fichier, nouv_y * largeur + nouv_x, SEEK_SET);
        caractere_destination = fgetc(fichier);

        char caractere_lateral;
        fseek(fichier, perso->positionY * largeur + nouv_x, SEEK_SET);
        caractere_lateral = fgetc(fichier);
        
        if (nouv_x >= 0 && nouv_x < largeur && nouv_y >= 0 && 
            caractere_destination != 'w' && caractere_lateral != 'w' && perso->peut_monter) {
            effacer_position(fichier, perso, largeur);
            perso->positionX = nouv_x;
            perso->positionY = nouv_y;
            mettre_position(fichier, perso, largeur);
            
            system("cls");
            afficherPaysage(fichier, perso->positionX);
            Sleep(75); 
        }
    }
    
    perso->en_saut = 0;
}

int copierFichier(const char *source, const char *destination) {
    FILE *src = fopen(source, "r");
    if (src == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier source %s\n", source);
        return 0;
    }
    
    FILE *dest = fopen(destination, "w");
    if (dest == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier destination %s\n", destination);
        fclose(src);
        return 0;
    }

    char car;
    while ((car = fgetc(src)) != EOF) {
        fputc(car, dest);
    }

    fclose(src);
    fclose(dest);
    return 1;
}

void caracterePaysage(char caractereActuel) {
    switch (caractereActuel) {
        case 'w':
            printf("#");
            break;
        case 'c':
            printf("o");
            break;
        case 'M':
            printf("&");
            break;
        default:
            printf("%c", caractereActuel);
            break;
    }
}

void afficherPaysage(FILE *fichier, int positionJoueur) {
    char ligne[1024];

    int debutLecture = positionJoueur - 21;
    int finLecture = positionJoueur + 21;

    if (debutLecture < 0) {
        debutLecture = 0;
    }
    if (finLecture < 42) {
        finLecture = 42;
    }

    rewind(fichier);

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        int longueurLigne = strlen(ligne);

        if (ligne[longueurLigne - 1] == '\n') {
            ligne[--longueurLigne] = '\0';
        }

        for (int i = 0; i < longueurLigne; i++) {
            if (i >= debutLecture && i < finLecture) {
                caracterePaysage(ligne[i]);
            } 
        }
        printf("\n"); 
    }
}

void verifier_collision(FILE *fichier, Personnage* perso, int largeur) {
    char caractere_dessous;
    fseek(fichier, (perso->positionY + 1) * largeur + perso->positionX, SEEK_SET);
    caractere_dessous = fgetc(fichier);
    
    if (caractere_dessous == 'w') {
        perso->en_chute = 0;
    } else {
        perso->en_chute = 1;
    }

    char caractere_dessus;
    fseek(fichier, (perso->positionY - 1) * largeur + perso->positionX, SEEK_SET);
    caractere_dessus = fgetc(fichier);
    
    if (caractere_dessus == 'w') {
        perso->peut_monter = 0;
    } else {
        perso->peut_monter = 1;
    }

    char caractere_devant;
    fseek(fichier, (perso->positionY) * largeur + perso->positionX + 1, SEEK_SET);
    caractere_devant = fgetc(fichier);
    
    if (caractere_devant == 'w' || perso->positionX == largeur - 3) {
        perso->peut_avancer = 0;
    } else {
        perso->peut_avancer = 1;
    }

    char caractere_derriere;
    fseek(fichier, (perso->positionY) * largeur + perso->positionX - 1, SEEK_SET);
    caractere_derriere = fgetc(fichier);
    
    if (caractere_derriere == 'w' || perso->positionX == 0) {
        perso->peut_reculer = 0;
    } else {
        perso->peut_reculer = 1;
    }

    if (perso->positionX < 0) perso->positionX = 0;
    if (perso->positionX >= largeur) perso->positionX = largeur - 1;
    if (perso->positionY < 0) perso->positionY = 0;
}

void deplacer_joueur(FILE *fichier, Personnage* perso, int largeur) {
    if (perso->positionY >= 12) {
        return; 
    }

    int deplacement_x = 0;
    
    if (GetAsyncKeyState('D') & 0x8000) deplacement_x = 1;
    if (GetAsyncKeyState('Q') & 0x8000) deplacement_x = -1; 

    verifier_collision(fichier, perso, largeur);

    if (perso->en_chute) {
        int new_y = perso->positionY + 1;
        fseek(fichier, new_y * largeur + perso->positionX, SEEK_SET);
        char c = fgetc(fichier);
        if (c == 'c') {
            perso->score++;
            fseek(fichier, new_y * largeur + perso->positionX, SEEK_SET);
            fputc(' ', fichier);
        }
        effacer_position(fichier, perso, largeur);
        perso->positionY = new_y;
        mettre_position(fichier, perso, largeur);
        verifier_collision(fichier, perso, largeur);
    }

    if (deplacement_x != 0 && !perso->en_saut) {
        int new_x = perso->positionX + deplacement_x;
        fseek(fichier, perso->positionY * largeur + new_x, SEEK_SET);
        char c = fgetc(fichier);
        if (c == 'c') {
            perso->score++;
            fseek(fichier, perso->positionY * largeur + new_x, SEEK_SET);
            fputc(' ', fichier);
        }
        if ((deplacement_x > 0 && perso->peut_avancer && perso->positionX < largeur - 1) || 
            (deplacement_x < 0 && perso->peut_reculer && perso->positionX > 0)) {
            effacer_position(fichier, perso, largeur);
            perso->positionX = new_x;
            mettre_position(fichier, perso, largeur);
        }
    }

    if ((GetAsyncKeyState('Z') & 0x8000) && !perso->en_saut && !perso->en_chute) {
        perso->en_saut = 1;
        int direction = 0;
        
        if (deplacement_x > 0) direction = 1;
        else if (deplacement_x < 0) direction = -1;
        
        gerer_saut(fichier, perso, largeur, direction);
    }
      
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        while (_kbhit()) _getch();
        menuSauvegarde(perso, fichier);
    }
}

void menu_mort(Personnage *perso, const char *fichierTemp) {
    system("cls");
    if (perso->vie > 0) {
        printf("+---------------------------------------------+\n");
        printf("|                 VOUS ETES MORT!             |\n");
        printf("|               Vies restantes : %d            |\n", perso->vie);
        printf("|                                             |\n");
        printf("|                1. Recommencer               |\n");
        printf("|               2. Menu Principal             |\n");
        printf("+---------------------------------------------+\n");
        printf("Choix : ");
        while (_kbhit()) _getch(); 
        int choix;
        scanf("%d", &choix);
        
        if (choix == 1) {
            remove(fichierTemp);
            if (!copierFichier("Mario.txt", fichierTemp)) {
                printf("Erreur lors de la réinitialisation du niveau !\n");
                Sleep(1500);
                menuPrincipal("Mario.txt");
                return;
            }
            perso->positionX = 21;
            perso->positionY = 5;
            perso->score = 0;
            jouer(fichierTemp, perso);
        } else {
            menuPrincipal("Mario.txt");
        }
    } else {
        printf("+---------------------------------------------+\n");
        printf("|                 GAME OVER !                 |\n");
        printf("|                                             |\n");
        printf("|         Appuyez sur une touche pour         |\n");
        printf("|         retourner au menu principal         |\n");
        printf("+---------------------------------------------+\n");
        while (_kbhit()) _getch(); 
        menuPrincipal("Mario.txt");
    }
}

void menuPrincipal(const char *fichierOriginal) {
    Personnage perso = {21, 5, "", 0, 0, 0, 1, 1, 1, 3};
    int choix;
    char* fichierTemp = NULL;
    
    system("cls");
    printf("+---------------------------------------------+\n");
    printf("|     Bienvenue sur le meilleur jeu Mario     |\n");
    printf("|                                             |\n");  
    printf("|                   - Menu -                  |\n");  
    printf("|                                             |\n");  
    printf("|              1. Nouvelle Partie             |\n");  
    printf("|              2. Charger Partie              |\n");  
    printf("|              3. Score                       |\n"); 
    printf("|              4. Reset les scores            |\n");
    printf("|              5. Quitter                     |\n"); 
    printf("+---------------------------------------------+\n");
    printf("Choisissez une option : ");
    scanf("%d", &choix);

    if (choix < 1 || choix > 5) {
        printf("Choix invalide, veuillez reessayer\n");
        Sleep(1000);
        menuPrincipal(fichierOriginal);
        return;
    }
    
    system("cls");
    
    switch(choix) {
        case 1:
            printf("Entrez votre nom: ");
            scanf("%s", perso.nom);
            fichierTemp = creerNomFichierTemp(perso.nom);
            
            remove(fichierTemp);
            
            if (!copierFichier(fichierOriginal, fichierTemp)) {
                printf("Erreur lors de la copie du fichier original\n");
                Sleep(1500);
                free(fichierTemp);
                menuPrincipal(fichierOriginal);
                return;
            }
            
            jouer(fichierTemp, &perso);
            free(fichierTemp);
            break;
        case 2:
            if (chargerPartie(&perso)) {
                fichierTemp = creerNomFichierTemp(perso.nom);
                
                remove(fichierTemp);
                
                if (!copierFichier(fichierOriginal, fichierTemp)) {
                    printf("Erreur lors de la copie du fichier original\n");
                    Sleep(1500);
                    free(fichierTemp);
                    menuPrincipal(fichierOriginal);
                    return;
                }
                
                jouer(fichierTemp, &perso);
                free(fichierTemp);
            } else {
                menuPrincipal(fichierOriginal);
            }
            break;
        case 3:
            afficherScores();
            menuPrincipal(fichierOriginal);
            break;
        case 4:
            resetScores();
            menuPrincipal(fichierOriginal);
            break;
        case 5:
            exit(0);
    }
}

void sauvegarderPartie(Personnage *perso, FILE *fichier) {
    fclose(fichier);
    
    FILE *sauvegarde = fopen("sauvegarde.txt", "a");
    if (sauvegarde == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier de sauvegarde\n");
        Sleep(1500);
        return;
    }
    
    time_t now;
    time(&now);
    
    fprintf(sauvegarde, "===== Sauvegarde %s =====\n", ctime(&now));
    fprintf(sauvegarde, "Nom:%s\n", perso->nom);
    fprintf(sauvegarde, "PositionX:%d\n", perso->positionX);
    fprintf(sauvegarde, "PositionY:%d\n", perso->positionY);
    fprintf(sauvegarde, "Score:%d\n", perso->score);
    fprintf(sauvegarde, "Vie:%d\n\n", perso->vie);
    fclose(sauvegarde);
    
    printf("Partie sauvegardee avec succes!\n");
    Sleep(1000);
    
    menuPrincipal("Mario.txt");
}

int chargerPartie(Personnage *perso) {
    FILE *fichier = fopen("sauvegarde.txt", "r");
    if (fichier == NULL) {
        printf("Aucune sauvegarde trouvee !\n");
        Sleep(1000);
        return 0;
    }

    Sauvegarde sauvegardes[100];
    int nbSauvegardes = 0;
    char ligne[100], nom_temp[100], date_temp[100];
    int score_temp = 0;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strstr(ligne, "===== Sauvegarde") == ligne) {
            strcpy(date_temp, ligne + 16);
            date_temp[strlen(date_temp)-1] = '\0';
        }
        else if (sscanf(ligne, "Nom:%s", nom_temp) == 1) {
        }
        else if (sscanf(ligne, "Score:%d", &score_temp) == 1) {
            strcpy(sauvegardes[nbSauvegardes].nom, nom_temp);
            sauvegardes[nbSauvegardes].score = score_temp;
            strcpy(sauvegardes[nbSauvegardes].date, date_temp);
            nbSauvegardes++;
        }
        else if (sscanf(ligne, "Vie:%d", &sauvegardes[nbSauvegardes - 1].vie) == 1) {
        }
    }

    fclose(fichier);

    if (nbSauvegardes == 0) {
        printf("Aucune sauvegarde disponible.\n");
        Sleep(1000);
        return 0;
    }

    printf("+------+--------------------+--------+------+-----------------------------+\n");
    printf("| #    | Nom                | Score  | Vie  | Date                        |\n");
    printf("+------+--------------------+--------+------+-----------------------------+\n");

    for (int i = 0; i < nbSauvegardes; i++) {
        printf("| %3d  | %-18s | %6d | %4d | %-21s   |\n", i + 1, sauvegardes[i].nom, sauvegardes[i].score, sauvegardes[i].vie, sauvegardes[i].date);
    }
    printf("+------+--------------------+--------+------+-----------------------------+\n");

    int choix;
    printf("Entrez le numero de la sauvegarde a charger: ");
    scanf("%d", &choix);

    while (choix < 1 || choix > nbSauvegardes) {
        printf("Numero invalide. Entrez un numero valide: ");
        scanf("%d", &choix);
    }

    fichier = fopen("sauvegarde.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de sauvegarde !\n");
        return 0;
    }

    int currentSave = 0;
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strstr(ligne, "===== Sauvegarde") == ligne) {
            currentSave++;
        }
        if (currentSave == choix) {
            if (sscanf(ligne, "Nom:%s", perso->nom) == 1);
            else if (sscanf(ligne, "PositionX:%d", &perso->positionX) == 1); 
            else if (sscanf(ligne, "PositionY:%d", &perso->positionY) == 1); 
            else if (sscanf(ligne, "Score:%d", &perso->score) == 1); 
            else if (sscanf(ligne, "Vie:%d", &perso->vie) == 1);
        }
    }

    fclose(fichier);
    printf("Partie chargee avec succes !\n");
    printf("Bienvenue %s! Score: %d\n", perso->nom, perso->score);
    Sleep(1500);
    return 1;
}

void menuSauvegarde(Personnage *perso, FILE *fichier) {
    int choix;  
    char* nouveauFichierTemp;
    char* fichierTemp = creerNomFichierTemp(perso->nom);
    
    while (1) {  
        system("cls");  
        printf("+---------------------------------------------+\n");
        printf("|               Menu Sauvegarde               |\n");
        printf("|                                             |\n");
        printf("|           1. Sauvegarder la partie          |\n");
        printf("|           2. Retour au jeu                  |\n");
        printf("|           3. Menu Principal                 |\n");
        printf("+---------------------------------------------+\n");
        printf("Votre choix: ");
        while (_kbhit()) _getch(); 
        scanf("%d", &choix);
        
        switch (choix) {
            case 1:
                sauvegarderPartie(perso, fichier);
                break;
            case 2:  
                if (fichier == NULL) {
                    fichier = fopen(fichierTemp, "r+");
                    if (fichier == NULL) {
                        printf("Erreur: Impossible de réouvrir le fichier de jeu\n");
                        Sleep(1500);
                        free(fichierTemp);
                        menuPrincipal("Mario.txt");
                        return;
                    }
                }
                free(fichierTemp);
                return;  
            case 3:
                if (fichier != NULL) {
                    fclose(fichier);
                }
                remove(fichierTemp);
                free(fichierTemp);
                menuPrincipal("Mario.txt");
                return;
            default:
                printf("Choix invalide !\n");
                Sleep(1000);
                break;
        }
    }
}

void afficherScores() {
    FILE *fichier = fopen("sauvegarde.txt", "r");
    if (fichier == NULL) {
        printf("Aucune sauvegarde trouvee !\n");
        Sleep(1000);
        return;
    }

    Sauvegarde sauvegardes[100]; 
    int nbSauvegardes = 0;
    
    char ligne[100];
    char nom_temp[100];
    int score_temp = 0;
    char date_temp[100];

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strstr(ligne, "===== Sauvegarde") == ligne) {
            strcpy(date_temp, ligne + 16);
            date_temp[strlen(date_temp)-1] = '\0';
        }
        else if (sscanf(ligne, "Nom:%s", nom_temp) == 1) {
        }
        else if (sscanf(ligne, "Score:%d", &score_temp) == 1) {
            strcpy(sauvegardes[nbSauvegardes].nom, nom_temp);
            sauvegardes[nbSauvegardes].score = score_temp;
            strcpy(sauvegardes[nbSauvegardes].date, date_temp);
            nbSauvegardes++;
        }
        else if (sscanf(ligne, "Vie:%d", &sauvegardes[nbSauvegardes - 1].vie) == 1) {
        }
    }
    
    fclose(fichier);

    for (int i = 0; i < nbSauvegardes - 1; i++) {
        for (int j = 0; j < nbSauvegardes - i - 1; j++) {
            if (sauvegardes[j].score < sauvegardes[j + 1].score) {
                
                Sauvegarde temp = sauvegardes[j];
                sauvegardes[j] = sauvegardes[j + 1];
                sauvegardes[j + 1] = temp;
            }
        }
    }

    
    printf("+------+--------------------+--------+------+-----------------------------+\n");
    printf("| #    | Nom                | Score  | Vie  | Date                        |\n");
    printf("+------+--------------------+--------+------+-----------------------------+\n");

    for (int i = 0; i < nbSauvegardes && i < 10; i++) {  
        printf("| %3d  | %-18s | %6d | %4d | %-21s   |\n", i + 1, sauvegardes[i].nom, sauvegardes[i].score, sauvegardes[i].vie, sauvegardes[i].date);
    }
    printf("+------+--------------------+--------+------+-----------------------------+\n");

    printf("\nAppuyez sur une touche pour continuer...");
    _getch();
}

void resetScores() {
    printf("Voulez-vous vraiment supprimer toutes les sauvegardes ? (o/n) : ");
    char choix;
    scanf(" %c", &choix);
    
    if (choix == 'o' || choix == 'O') {
        FILE *fichier = fopen("sauvegarde.txt", "w");
        if (fichier != NULL) {
            fclose(fichier);
            printf("Les scores ont ete reinitialises avec succes !\n");
        } else {
            printf("Erreur: Impossible d'ouvrir le fichier de sauvegarde\n");
        }
        system("del temp_*.txt");
        printf("Les fichiers temporaires ont ete supprimes avec succes !\n");
        Sleep(1500);
    }
}

void jouer(const char *fichierTemp, Personnage* perso) {    
    FILE *fichier = fopen(fichierTemp, "r+");
    if (fichier == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", fichierTemp);
        Sleep(1500);
        return;
    }
    
    int largeur = 100;

    if (perso->positionX < 0 || perso->positionX >= largeur || perso->positionY < 0) {
        perso->positionX = 21;  
        perso->positionY = 5;   
    }

    fseek(fichier, (perso->positionY) * largeur + perso->positionX, SEEK_SET);
    fputc('M', fichier);
    fflush(fichier);

    cacherCurseur();

    while (1) {
        Sleep(50);
        system("cls");
        
        rewind(fichier);
        afficherPaysage(fichier, perso->positionX);
        printf("Score: %d | Nom: %s | Vies: %d\n", perso->score, perso->nom, perso->vie);
        
        deplacer_joueur(fichier, perso, largeur);
        fflush(fichier);

        if (perso->positionY >= 12) {
            perso->vie--;
            fclose(fichier);
            menu_mort(perso, fichierTemp);
            return;
        }
    }
}