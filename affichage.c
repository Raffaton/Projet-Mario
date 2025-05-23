#include "projet.h"

void effacer_position(Carte *carte, Personnage *perso)
{
    carte->carte[perso->positionY][perso->positionX] = ' ';
}

void mettre_position(Carte *carte, Personnage *perso)
{
    carte->carte[perso->positionY][perso->positionX] = 'M';
}

void deplacerCurseur(int x, int y)
{
    COORD coord = {x, y};
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, coord);
}

void cacherCurseur()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.bVisible = FALSE;
    info.dwSize = 1;
    SetConsoleCursorInfo(console, &info);
}

TTF_Font *initFont()
{
    if (TTF_Init() == -1)
    {
        printf("Erreur d'initialisation de TTF: %s\n", TTF_GetError());
        return NULL;
    }

    const char *fontPaths[] = {
        "asset/font/PixelifySans-VariableFont_wght.ttf"};

    TTF_Font *font = NULL;
    for (int i = 0; i < 3 && font == NULL; i++)
    {
        font = TTF_OpenFont(fontPaths[i], 32);
        if (font != NULL)
        {
            break;
        }
    }

    if (font == NULL)
    {
        printf("Impossible de charger la police: %s\n", TTF_GetError());
    }

    return font;
}

SDL_Texture *loadHeartTexture(SDL_Renderer *renderer)
{
    const char *heartPaths = "asset/Tiles/coeur.png";

    SDL_Surface *surface = NULL;

    surface = IMG_Load(heartPaths);

    if (surface == NULL)
    {
        surface = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);
        if (surface != NULL)
        {
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 0, 255));
        }
        else
        {
            return NULL;
        }
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    return texture;
}

void freeHeartTexture()
{
    if (heartTexture != NULL)
    {
        SDL_DestroyTexture(heartTexture);
        heartTexture = NULL;
    }
}

void afficherVies(SDL_Renderer *renderer, int vies)
{
    if (heartTexture == NULL)
    {
        return;
    }

    int heartWidth, heartHeight;
    SDL_QueryTexture(heartTexture, NULL, NULL, &heartWidth, &heartHeight);

    int displayWidth = 40;
    int displayHeight = 40;

    int startX = 1100;
    int startY = 30;

    for (int i = 0; i < vies; i++)
    {
        SDL_Rect destRect = {startX + i * (displayWidth + 5), startY, displayWidth, displayHeight};
        SDL_RenderCopy(renderer, heartTexture, NULL, &destRect);
    }
}

void afficherScore(SDL_Renderer *renderer, int score, int vies)
{
    if (scoreFont == NULL)
    {
        return;
    }

    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);

    SDL_Color scoreColor = {250, 82, 85, 255};

    SDL_Surface *textSurface = TTF_RenderText_Solid(scoreFont, scoreText, scoreColor);
    if (textSurface == NULL)
    {
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL)
    {
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect destRect = {40, 30, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &destRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);

    afficherVies(renderer, vies);
}

void loadCoinAnimations(SDL_Renderer *renderer)
{
    if (coinAnimation == NULL)
    {
        const char *coinPaths = "asset/brackeys_platformer_assets/sprites/coin.png";

        coinAnimation = loadAnimation(renderer, coinPaths, 6, 16, 16, 100);
    }

    if (starCoinAnimation == NULL)
    {
        const char *starCoinPaths = "asset/brackeys_platformer_assets/sprites/starcoin.png";

        starCoinAnimation = loadAnimation(renderer, starCoinPaths, 9, 30, 30, 50);
    }
}

BackgroundTexture *loadBackgroundTexture(SDL_Renderer *renderer, const char *path)
{
    BackgroundTexture *bg = (BackgroundTexture *)malloc(sizeof(BackgroundTexture));
    if (bg == NULL)
    {
        return NULL;
    }

    SDL_Surface *surface = IMG_Load(path);
    if (surface == NULL)
    {
        printf("Erreur: Impossible de charger l'image %s: %s\n", path, IMG_GetError());
        free(bg);
        return NULL;
    }

    bg->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (bg->texture == NULL)
    {
        SDL_FreeSurface(surface);
        free(bg);
        return NULL;
    }

    bg->width = surface->w;
    bg->height = surface->h;

    SDL_FreeSurface(surface);

    return bg;
}

void freeBackgroundTexture(BackgroundTexture *bg)
{
    if (bg != NULL)
    {
        if (bg->texture != NULL)
        {
            SDL_DestroyTexture(bg->texture);
        }
        free(bg);
    }
}

void loadFlagAnimation(SDL_Renderer *renderer)
{
    if (flagAnimation == NULL)
    {
        const char *flagPaths[] = {
            "C:/Users/duber/cours L1/programation/mario_vscode/Projet-Mario/asset/Rocky Roads/Objects/flag.png",
            "asset/Rocky Roads/Objects/flag.png",
            "asset/sprit/objects/flag.png",
            "asset/flag.png"};

        for (int i = 0; i < 4 && flagAnimation == NULL; i++)
        {
            flagAnimation = loadAnimation(renderer, flagPaths[i], 4, 32, 32, 200);
            if (flagAnimation != NULL)
            {
                break;
            }
        }

        if (flagAnimation == NULL)
        {
            SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
            if (tempSurface != NULL)
            {
                SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 0, 100, 255));

                flagAnimation = (Animation *)malloc(sizeof(Animation));
                if (flagAnimation != NULL)
                {
                    flagAnimation->texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
                    flagAnimation->frameCount = 1;
                    flagAnimation->currentFrame = 0;
                    flagAnimation->frameWidth = 32;
                    flagAnimation->frameHeight = 32;
                    flagAnimation->frameDuration = 200;
                    flagAnimation->lastFrameTime = SDL_GetTicks();
                }
                SDL_FreeSurface(tempSurface);
            }
        }
    }
}
void loadSlimeAnimation(SDL_Renderer *renderer)
{
    
    if (slimeAnimation != NULL)
    {
        return;
    }

    const char *slimePaths[] = {
        "asset/Tiles/Mini_Slime_Walk.png",
        "C:/Users/duber/cours L1/programation/mario_vscode/Projet-Mario/asset/Tiles/Mini_Slime_Walk.png"
    };

    for (int i = 0; i < 2 && slimeAnimation == NULL; i++)
    {
        slimeAnimation = loadAnimation(renderer, slimePaths[i], 8, 32, 32, 150);
        if (slimeAnimation != NULL)
        {
            break;
        }
    }

    if (slimeAnimation == NULL)
    {
        SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
        if (tempSurface != NULL)
        {
            SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 128, 0, 128));

            slimeAnimation = (Animation *)malloc(sizeof(Animation));
            if (slimeAnimation != NULL)
            {
                slimeAnimation->texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
                slimeAnimation->frameCount = 1;
                slimeAnimation->currentFrame = 0;
                slimeAnimation->frameWidth = 32;
                slimeAnimation->frameHeight = 32;
                slimeAnimation->frameDuration = 150;
                slimeAnimation->lastFrameTime = SDL_GetTicks();
            }
            SDL_FreeSurface(tempSurface);
        }
    }
}

void freeSlimeAnimation()
{
    if (slimeAnimation != NULL)
    {
        freeAnimation(slimeAnimation);
        slimeAnimation = NULL;
    }
}

void afficherPaysageSDL(Carte *carte, int positionJoueur, SDL_Renderer *renderer, int playerMoving, Personnage *perso)
{
    int largeurAffichage = 50;
    int demiLargeur = largeurAffichage / 2;

    int debutX = positionJoueur - demiLargeur;
    int finX = positionJoueur + demiLargeur;

    if (debutX < 0)
    {
        finX -= debutX;
        debutX = 0;
    }

    if (finX > carte->largeur)
    {
        debutX -= (finX - carte->largeur);
        finX = carte->largeur;
    }

    if (debutX < 0)
    {
        debutX = 0;
    }

    if (globalBackground == NULL)
    {
        const char *bgPath = "asset/sprit/Mondstadt Tileset Platform - Basic/windrise-background.png";
        globalBackground = loadBackgroundTexture(renderer, bgPath);

        if (globalBackground == NULL)
        {
            bgPath = "asset/sprit/Mondstadt Tileset Platform - Basic/windrise-background.png";
            globalBackground = loadBackgroundTexture(renderer, bgPath);
        }
    }

    if (gameTextures == NULL)
    {
        gameTextures = loadGameTextures(renderer);
    }

    if (playerAnimations == NULL)
    {
        playerAnimations = loadPlayerAnimations(renderer);
    }

    loadCoinAnimations(renderer);
    loadFlagAnimation(renderer);
    loadCarnivoreAnimation(renderer);
    loadSlimeAnimation(renderer);

    if (heartTexture == NULL)
    {
        heartTexture = loadHeartTexture(renderer);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (globalBackground != NULL && globalBackground->texture != NULL)
    {
        int backgroundOffset = (debutX * 3) % globalBackground->width;

        SDL_Rect srcRect = {backgroundOffset, 0, globalBackground->width - backgroundOffset, globalBackground->height};
        SDL_Rect dstRect = {0, 0, largeurAffichage * TILE_SIZE, 24 * TILE_SIZE};

        SDL_RenderCopy(renderer, globalBackground->texture, &srcRect, &dstRect);

        if (backgroundOffset > 0)
        {
            srcRect.x = 0;
            srcRect.w = backgroundOffset;
            dstRect.x = (globalBackground->width - backgroundOffset) * (largeurAffichage * TILE_SIZE) / globalBackground->width;
            dstRect.w = largeurAffichage * TILE_SIZE - dstRect.x;
            SDL_RenderCopy(renderer, globalBackground->texture, &srcRect, &dstRect);
        }
    }

    for (int y = 0; y < carte->hauteur; y++)
    {
        for (int x = debutX; x < finX; x++)
        {
            if (x >= carte->largeur)
                continue;

            SDL_Rect tile = {
                (x - debutX) * TILE_SIZE,
                y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE};

            if (carte->carte[y][x] == 'M')
                continue;

            switch (carte->carte[y][x])
            {
            case 'w':
                if (gameTextures != NULL && gameTextures->brick != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->brick, NULL, &tile);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'n':
                if (gameTextures != NULL && gameTextures->nuage != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->nuage, NULL, &tile);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'Q':
                if (slimeAnimation != NULL)
                {
                    updateAnimation(slimeAnimation);
                    int screenX = (x - debutX) * TILE_SIZE + (TILE_SIZE / 2);
                    int screenY = y * TILE_SIZE + TILE_SIZE;
                    renderAnimation(renderer, slimeAnimation, screenX, screenY, 0);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;

                
                if (slimeAnimation != NULL)
                {
                    freeSlimeAnimation();
                }
            case ']':
                if (gameTextures != NULL && gameTextures->tuyau != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->tuyau, NULL, &tile);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'u':
                if (carnivoreAnimation != NULL)
                {
                    updateAnimation(carnivoreAnimation);
                    int screenX = (x - debutX) * TILE_SIZE + (TILE_SIZE / 2);
                    int screenY = y * TILE_SIZE + TILE_SIZE;
                    renderAnimation(renderer, carnivoreAnimation, screenX, screenY, 0);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case '|':
                if (gameTextures != NULL && gameTextures->plante != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->plante, NULL, &tile);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case '!':
                if (flagAnimation != NULL)
                {
                    updateAnimation(flagAnimation);
                    int screenX = (x - debutX) * TILE_SIZE + (TILE_SIZE / 2);
                    int screenY = y * TILE_SIZE + TILE_SIZE;
                    renderAnimation(renderer, flagAnimation, screenX, screenY, 0);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 25, 65, 199, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'd':
                if (gameTextures != NULL && gameTextures->terre != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->terre, NULL, &tile);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            
            case 'f':
                if (gameTextures != NULL && gameTextures->herbe5 != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->herbe5, NULL, &tile);
                }
                else
                {
                    
                    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); 
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'o':
                if (gameTextures != NULL && gameTextures->terre5 != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->terre5, NULL, &tile);
                }
                else
                {
                    
                    SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); 
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'e':
                if (gameTextures != NULL && gameTextures->herbe3 != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->herbe3, NULL, &tile);
                }
                else
                {
                    
                    SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); 
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'y':
                if (gameTextures != NULL && gameTextures->terre3 != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->terre3, NULL, &tile);
                }
                else
                {
                    
                    SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); 
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'z':
                if (gameTextures != NULL && gameTextures->herbe4 != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->herbe4, NULL, &tile);
                }
                else
                {
                    
                    SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); 
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            case 'g':
                if (gameTextures != NULL && gameTextures->terre4 != NULL)
                {
                    SDL_RenderCopy(renderer, gameTextures->terre4, NULL, &tile);
                }
                else
                {
                    
                    SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); 
                    SDL_RenderFillRect(renderer, &tile);
                }
                break;
            default:
                break;
            }
        }
    }

    animer_pieces(&tab_pieces);
    afficher_pieces(renderer, &tab_pieces, positionJoueur, debutX);

    animer_starcoins(&tab_starcoins);
    afficher_starcoins(renderer, &tab_starcoins, positionJoueur, debutX);

    for (int y = 0; y < carte->hauteur; y++)
    {
        for (int x = debutX; x < finX; x++)
        {
            if (x >= carte->largeur)
                continue;

            if (carte->carte[y][x] == 'M')
            {
                Animation *currentAnimation = NULL;

                if (playerAnimations != NULL)
                {

                    if (playerMoving && playerAnimations->run != NULL)
                    {
                        currentAnimation = playerAnimations->run;
                    }
                    else if (playerAnimations->idle != NULL)
                    {
                        currentAnimation = playerAnimations->idle;
                    }

                    if (currentAnimation != NULL)
                    {
                        updateAnimation(currentAnimation);

                        int screenX = (x - debutX) * TILE_SIZE + (TILE_SIZE / 2);
                        int screenY = y * TILE_SIZE + TILE_SIZE;

                        renderAnimation(renderer, currentAnimation, screenX, screenY, !playerAnimations->facingRight);
                    }
                }
                else
                {

                    SDL_Rect tile = {
                        (x - debutX) * TILE_SIZE,
                        y * TILE_SIZE,
                        TILE_SIZE,
                        TILE_SIZE};
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &tile);
                }
            }
        }
    }

    afficherScore(renderer, perso->score, perso->vie);
}
void reinitialiserAnimations() {
    
    slimeAnimation = NULL;
    flagAnimation = NULL;
    coinAnimation = NULL;
    starCoinAnimation = NULL;
    carnivoreAnimation = NULL;
    playerAnimations = NULL;
    gameTextures = NULL;
    globalBackground = NULL;
    heartTexture = NULL;
}
void freeFlagAnimation()
{
    if (flagAnimation != NULL)
    {
        freeAnimation(flagAnimation);
        flagAnimation = NULL;
    }
}

void nettoyerSDL(SDL_Window *window, SDL_Renderer *renderer)
{
    liberer_pieces(&tab_pieces);
    liberer_starcoins(&tab_starcoins);

    if (scoreFont != NULL)
    {
        TTF_CloseFont(scoreFont);
        scoreFont = NULL;
        TTF_Quit();
    }

    if (playerAnimations != NULL)
    {
        freePlayerAnimations(playerAnimations);
        playerAnimations = NULL;
    }

    if (gameTextures != NULL)
    {
        freeGameTextures(gameTextures);
        gameTextures = NULL;
    }

    if (globalBackground != NULL)
    {
        freeBackgroundTexture(globalBackground);
        globalBackground = NULL;
    }

    if (heartTexture != NULL)
    {
        freeHeartTexture();
    }

    if (flagAnimation != NULL)
    {
        freeAnimation(flagAnimation);
        flagAnimation = NULL;
    }

    if (coinAnimation != NULL)
    {
        freeAnimation(coinAnimation);
        coinAnimation = NULL;
    }

    if (starCoinAnimation != NULL)
    {
        freeAnimation(starCoinAnimation);
        starCoinAnimation = NULL;
    }

    if (carnivoreAnimation != NULL)
    {
        freeAnimation(carnivoreAnimation);
        carnivoreAnimation = NULL;
    }

    if (slimeAnimation != NULL)
    {
        freeSlimeAnimation();
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
    }

    
    reinitialiserAnimations();

    SDL_Quit();
}
