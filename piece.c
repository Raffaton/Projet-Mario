#include "projet.h"

void initialiser_pieces(Carte* carte, Tab_piece* tab_piece) {
    int nb_pieces = 0;
    for (int y = 0; y < carte->hauteur; y++) {
        for (int x = 0; x < carte->largeur; x++) {
            if (carte->carte[y][x] == 'c') {
                nb_pieces++;
            }
        }
    }
    
    tab_piece->capacity = nb_pieces + 10; 
    tab_piece->pieces = (Piece*)malloc(sizeof(Piece) * tab_piece->capacity);
    tab_piece->count = 0;
    
    if (tab_piece->pieces == NULL) {
        return;
    }
    
    for (int y = 0; y < carte->hauteur; y++) {
        for (int x = 0; x < carte->largeur; x++) {
            if (carte->carte[y][x] == 'c') {
                tab_piece->pieces[tab_piece->count].positionX = x;
                tab_piece->pieces[tab_piece->count].positionY = y;
                tab_piece->pieces[tab_piece->count].actif = 1;
                tab_piece->count++;
            }
        }
    }
}

void animer_pieces(Tab_piece* tab_piece) {
    if (coinAnimation != NULL) {
        updateAnimation(coinAnimation);
    }
}

void afficher_pieces(SDL_Renderer* renderer, Tab_piece* tab_piece, int positionJoueur, int debutX) {
    int largeurAffichage = 50;
    int finX = debutX + largeurAffichage;

    for (int i = 0; i < tab_piece->count; i++) {
        if (!tab_piece->pieces[i].actif) continue;
        
        int pieceX = tab_piece->pieces[i].positionX;
        int pieceY = tab_piece->pieces[i].positionY;
        
        if (pieceX >= debutX && pieceX < finX) {
            if (coinAnimation != NULL) {
                int screenX = (pieceX - debutX) * TILE_SIZE + (TILE_SIZE / 2);
                int screenY = pieceY * TILE_SIZE + (TILE_SIZE / 2);
                
                renderAnimation(renderer, coinAnimation, screenX, screenY, 0);
            } else {
                SDL_Rect tile = {
                    (pieceX - debutX) * TILE_SIZE,
                    pieceY * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };
                
                SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                SDL_RenderFillRect(renderer, &tile);
            }
        }
    }
}

void liberer_pieces(Tab_piece* tab_piece) {
    if (tab_piece->pieces != NULL) {
        free(tab_piece->pieces);
        tab_piece->pieces = NULL;
    }
    tab_piece->count = 0;
    tab_piece->capacity = 0;
    
    if (coinAnimation != NULL) {
        freeAnimation(coinAnimation);
        coinAnimation = NULL;
    }
}

void check_collect_piece(Carte* carte, Tab_piece* tab_piece, Personnage* perso) {
    for (int i = 0; i < tab_piece->count; i++) {
        if (tab_piece->pieces[i].actif && 
            tab_piece->pieces[i].positionX == perso->positionX && 
            tab_piece->pieces[i].positionY == perso->positionY) {
            
            tab_piece->pieces[i].actif = 0;
            perso->score += 1;
            playSoundEffect("asset/sound/coin.wav", 40);
            carte->carte[tab_piece->pieces[i].positionY][tab_piece->pieces[i].positionX] = 'M';
        }
    }
}

void initialiser_starcoins(Carte* carte, Tab_starcoins* tab_starcoins) {
    int nb_starcoins = 0;
    for (int y = 0; y < carte->hauteur; y++) {
        for (int x = 0; x < carte->largeur; x++) {
            if (carte->carte[y][x] == '*') { 
                nb_starcoins++;
            }
        }
    }
    
    tab_starcoins->capacity = nb_starcoins + 5; 
    tab_starcoins->starCoins = (StarCoin*)malloc(sizeof(StarCoin) * tab_starcoins->capacity);
    tab_starcoins->count = 0;
    
    if (tab_starcoins->starCoins == NULL) {
        return;
    }
    
    for (int y = 0; y < carte->hauteur; y++) {
        for (int x = 0; x < carte->largeur; x++) {
            if (carte->carte[y][x] == '*') {
                tab_starcoins->starCoins[tab_starcoins->count].positionX = x;
                tab_starcoins->starCoins[tab_starcoins->count].positionY = y;
                tab_starcoins->starCoins[tab_starcoins->count].actif = 1;
                tab_starcoins->count++;
            }
        }
    }
}

void animer_starcoins(Tab_starcoins* tab_starcoins) {
    if (starCoinAnimation != NULL) {
        updateAnimation(starCoinAnimation);
    }
}

void afficher_starcoins(SDL_Renderer* renderer, Tab_starcoins* tab_starcoins, int positionJoueur, int debutX) {
    int largeurAffichage = 50;
    int finX = debutX + largeurAffichage;

    for (int i = 0; i < tab_starcoins->count; i++) {
        if (!tab_starcoins->starCoins[i].actif) continue;
        
        int starX = tab_starcoins->starCoins[i].positionX;
        int starY = tab_starcoins->starCoins[i].positionY;
        
        if (starX >= debutX && starX < finX) {
            if (starCoinAnimation != NULL) {
                int screenX = (starX - debutX) * TILE_SIZE + (TILE_SIZE / 2);
                int screenY = starY * TILE_SIZE + (TILE_SIZE / 2);
                
                renderAnimation(renderer, starCoinAnimation, screenX, screenY, 0);
            } else {
                SDL_Rect tile = {
                    (starX - debutX) * TILE_SIZE,
                    starY * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };
                
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(renderer, &tile);
            }
        }
    }
}

void liberer_starcoins(Tab_starcoins* tab_starcoins) {
    if (tab_starcoins->starCoins != NULL) {
        free(tab_starcoins->starCoins);
        tab_starcoins->starCoins = NULL;
    }
    tab_starcoins->count = 0;
    tab_starcoins->capacity = 0;
    
    if (starCoinAnimation != NULL) {
        freeAnimation(starCoinAnimation);
        starCoinAnimation = NULL;
    }
}

void check_collect_starcoin(Carte* carte, Tab_starcoins* tab_starcoins, Personnage* perso)
{
    for (int i = 0; i < tab_starcoins->count; i++)
    {
        if (tab_starcoins->starCoins[i].actif && 
            perso->positionX == tab_starcoins->starCoins[i].positionX && 
            perso->positionY == tab_starcoins->starCoins[i].positionY)
        {
            tab_starcoins->starCoins[i].actif = 0;
            perso->score += 50; 
            
            playSoundEffect("asset/sound/piece_etoile.wav", 100);
            
            carte->carte[tab_starcoins->starCoins[i].positionY][tab_starcoins->starCoins[i].positionX] = 'M';
        }
    }
}