#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINES 100
#define MAX_COLS 100

//intialisation du SDL
int SDL_Init(Uint32 flags) {
    return 0;
}

//Screen dimension constants
int screenW = 400;
int screenH = 320;

//initialisation des tableaux
int table[MAX_LINES][MAX_COLS];
int revealedTable[MAX_LINES][MAX_COLS];
// game est un booléen
int game = 1;

//première coordonnée pour ne pas mettre de mine
int firstMoveLine, firstMoveCol;

// dimensions du champ de mines, des mines et drapeaux
int numCols, numLines, numMines, numFlags;

// chargement de tout les sprites
SDL_Surface* holy_hand = SDL_LoadBMP("sprite/holy_hand_grenade.bmp");
SDL_Surface* flag = SDL_LoadBMP("sprite/flag.bmp");
SDL_Surface* mine = SDL_LoadBMP("sprite/mine.bmp");
SDL_Surface* number1 = SDL_LoadBMP("sprite/number1.bmp");
SDL_Surface* number2 = SDL_LoadBMP("sprite/number2.bmp");
SDL_Surface* number3 = SDL_LoadBMP("sprite/number3.bmp");
SDL_Surface* number4 = SDL_LoadBMP("sprite/number4.bmp");
SDL_Surface* number5 = SDL_LoadBMP("sprite/number5.bmp");
SDL_Surface* number6 = SDL_LoadBMP("sprite/number6.bmp");
SDL_Surface* number7 = SDL_LoadBMP("sprite/number7.bmp");
SDL_Surface* number8 = SDL_LoadBMP("sprite/number8.bmp");
SDL_Surface* ground_up = SDL_LoadBMP("sprite/ground+.bmp");
SDL_Surface* ground_down = SDL_LoadBMP("sprite/ground-.bmp");
SDL_Surface* ground_reveal_up = SDL_LoadBMP("sprite/ground_reveal+.bmp");
SDL_Surface* ground_reveal_down = SDL_LoadBMP("sprite/ground_reveal-.bmp");

//afficher une question + vider la réponse en cas de mauvaise réponse
int askInt(int min, int max, const char* message) {
    int coord = 0;
    while (!(coord >= min && coord <= max)) {
        printf("%s (entre %d et %d):", message, min, max);
        scanf_s("%d", &coord);
        while (getchar() != '\n');
    }
    return coord;
}

//
int askButton(int x, int y, int w, int h) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h)
        return 1;
}

//vérifier la position des mine autour de la case séléctionné
int check(int l, int c) {
    return table[l - 1][c - 1] + table[l - 1][c] + table[l - 1][c + 1] +
        table[l][c - 1] + table[l][c + 1] + table[l + 1][c - 1] +
        table[l + 1][c] + table[l + 1][c + 1];
}

//initialiser un tableau vide
void emptyTable() {
    // init du plateau de jeu
    for (int i = 0; i < MAX_LINES; ++i)
        for (int j = 0; j < MAX_COLS; ++j)
        {
            table[i][j] = 0;
            revealedTable[i][j] = 0;
        }
}

//afficher en jeu le nombre de colonne
void printMinefieldHeader(int n) {
    for (int i = 0; i < n; i++)
        printf("%02d ", i + 1);
    printf("\n");
}

//afficher le tableau pendant le jeu
void display() {
    // https://symbl.cc/en/
    printf("🚩 = %d\n", numFlags);
    // affichage du plateau de jeu
    printMinefieldHeader(numCols);
    for (int i = 1; i < numLines + 1; ++i) {
        for (int j = 1; j < numCols + 1; ++j) {
            if (revealedTable[i][j] == 0) {
                printf("__|");
            }
            else if (revealedTable[i][j] == 1) {
                if (table[i][j] == 1) {
                    printf("🧨|");
                }
                else {
                    printf("_%d|", check(i, j));
                }
            }
            else if (revealedTable[i][j] == 2) {
                printf("🚩|");
            }
        }
        printf("%d\n", i);
    }
}

void displayGraphic() {

}

//afficher une version du tableau pour facilité le debugage
void displayDebug() {
    printf("débug: \n");
    printMinefieldHeader(numCols);
    // printf("%d\n",numCols+1);
    // https://symbl.cc/en/
    // affichage du plateau de jeu
    for (int i = 1; i < numLines + 1; ++i) {
        for (int j = 1; j < numCols + 1; ++j) {
            if (table[i][j] == 1) {
                printf("🧨|");
            }
            else {
                printf("_%d|", check(i, j));
                // printf("__|");
            }
        }
        printf("%d\n", i);
    }
}

//placer des mines de manière aléatoire
void addMines(int n) {
    srand(time(NULL));
    int l = 0, c = 0;
    while (n > 0) {
        int lastL = l, lastC = c;
        l = (rand() % numLines) + 1, c = (rand() % numCols) + 1;
        if (table[l][c] != 1) {
            table[l][c] = 1;
            n--;
        }
        if (check(firstMoveLine, firstMoveCol) != 0 || (l == firstMoveLine && c == firstMoveCol)) {
            // printf("here\n");
            table[l][c] = 0;
            n++;
        }
    }
}

//vérifier la condition de victoire
int checkVictory() {
    int freeSpaces = (numLines * numCols) - numMines;
    for (int i = 1; i < numLines + 1; i++) {
        for (int j = 1; j < numCols + 1; j++) {
            if (revealedTable[i][j] == 1 && table[i][j] == 0) {
                freeSpaces--;
            }
        }
    }
    if (freeSpaces == 0) {
        return 1;
    }
    // printf("free spaces = %d\n",freeSpaces);
    return 0;
}

//révéler la case séléctionné
void reveal(int action, int l, int c) {
    // si le joueur veut révéler une case
    if (action == 1) {
        revealedTable[l][c] = 1;
        // si le joueur révèle une mine
        if (table[l][c] == 1 && action == 1) {
            game = 0;
            for (int i = 1; i < numLines + 1; i++) {
                for (int j = 1; j < numCols + 1; j++) {
                    if (table[i][j] == 1) {
                        revealedTable[i][j] = 1;
                    }
                }
            }
            display();
            printf("perdu...\n");
            // sinon s'il révèle un 0, on répète l'opération
        }
        else if (check(l, c) == 0 && action == 1) {
            if (l > 1 && revealedTable[l - 1][c] == 0) {
                reveal(action, l - 1, c);
            }
            if (l > 1 && c > 1 && revealedTable[l - 1][c - 1] == 0) {
                reveal(action, l - 1, c - 1);
            }
            if (l > 1 && c < numCols && revealedTable[l - 1][c + 1] == 0) {
                reveal(action, l - 1, c + 1);
            }
            if (l < numLines && revealedTable[l + 1][c] == 0) {
                reveal(action, l + 1, c);
            }
            if (l < numLines && c > 1 && revealedTable[l + 1][c - 1] == 0) {
                reveal(action, l + 1, c - 1);
            }
            if (l < numLines && c < numCols && revealedTable[l + 1][c + 1] == 0) {
                reveal(action, l + 1, c + 1);
            }
            if (c > 1 && revealedTable[l][c - 1] == 0) {
                reveal(action, l, c - 1);
            }
            if (c < numCols && revealedTable[l][c + 1] == 0) {
                reveal(action, l, c + 1);
            }
        }
        // si le joueur veut placer un drapeau
    }
    else if (action == 2) {
        // on place un drapeau
        if (revealedTable[l][c] == 0) {
            revealedTable[l][c] = 2;
            numFlags--;
            // s'il s'agit d'un drapeau, on le retire
        }
        else if (revealedTable[l][c] == 2) {
            revealedTable[l][c] = 0;
            numFlags++;
        }
    }
}

// Demande au joueur de choisir le mode de jeu et modifie les valeurs du tableau et le nombre de mines.
void setGameMode() {
    int gameMode = askInt(1, 4, "Choisissez un mode de jeu (1 = FACILE | 2 = MOYEN | 3 = DIFFICILE | 4 = CUSTOM)");
    if (gameMode == 1) {
        numLines = 8;
        numCols = 10;
        numMines = 10;
    }
    else if (gameMode == 2) {
        numLines = 14;
        numCols = 18;
        numMines = 40;
    }
    else if (gameMode == 3) {
        numLines = 20;
        numCols = 24;
        numMines = 99;
    }
    else if (gameMode == 4) {
        numLines = askInt(1, MAX_LINES, "Choisissez le nombre de lignes du champ de mines");
        numCols = askInt(1, MAX_COLS, "Choisissez le nombre de colonnes du champ de mines");
        numMines = askInt(1, numLines * numCols - 9, "Combient de mines voulez-vous");
    }
    numFlags = numMines;
}

//lancer le jeu
int main(int argc, char* argv[]) {
    const char* SDL_GetError(void);

    //The surface contained by the window

    if (SDL_Init(SDL_INIT_VIDEO) < 0) //Si le SDL renvoir -1 c'est signe d'une erreur
    {
        printf("Erreur d'initialisation de la SDL : %s", SDL_GetError());//affichage de l'erreur
        return EXIT_FAILURE;
    }

    SDL_Surface* screenSurface = NULL;
    SDL_Surface* tile = NULL;
    SDL_Window* window = SDL_CreateWindow("Démineur", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_FULLSCREEN_DESKTOP);

    int fullscreenW;
    int fullscreenH;
    SDL_GetWindowSize(window, &fullscreenW, &fullscreenH);
    int offsetX = fullscreenW / 2 - (10 / 2) * 40; // la valeur "10" correspond à numCols
    int offsetY = fullscreenH / 2 - (8 / 2) * 40; // la valeur "8" correspond à numLines

    if (window == NULL)//Si la fenetre renvoie NULL c'est signe d'une erreur
    {
        printf("Erreur lors de la création de la fenêtre : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    //Update the surface
    SDL_UpdateWindowSurface(window);

    //Hack to get window to stay up
    SDL_Surface* testImage = NULL;
    testImage = SDL_LoadBMP("sprite/holy_hand_grenade.bmp");
    SDL_Rect dstRect;
    int terrain = 1;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            dstRect.x = 40 * i + offsetX;
            dstRect.y = 40 * j + offsetY;
            dstRect.w = 40;
            dstRect.h = 40;
            if (terrain == 1) {
                SDL_BlitSurface(ground_up, NULL, screenSurface, &dstRect);
            }
            else {
                SDL_BlitSurface(ground_down, NULL, screenSurface, &dstRect);
            }
            terrain = terrain * -1;
        }
        terrain = terrain * -1;
    }
    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool quit = false;
    while (quit == false) {
        SDL_Event* event;
        if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
            printf("mdr connard\n");
        // for (int i = 0; i < 10; i++)
        // for (int j = 0; j < 10; j++)
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }
    }

    if (game == 0) {
        SDL_DestroyWindow(window);
    }

    exit(0);
    // """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

    setGameMode();
    emptyTable();
    printf("Faite votre première action pour commencer\n");
    firstMoveLine = askInt(1, numLines, "choisissez une ligne");
    firstMoveCol = askInt(1, numCols, "choisissez une colonne");
    addMines(numMines);
    reveal(1, firstMoveLine, firstMoveCol);

    while (game == 1) {
        display();
        // displayDebug();

        int action = 0;
        int coordL = 0;
        int coordC = 0;
        action = askInt(1, 2,
            "Choisissez une action (1 pour révéler la case / 2 pour "
            "placer ou retirer un drapeau)");

        coordL = askInt(1, numLines, "Choisissez une ligne");

        coordC = askInt(1, numCols, "Choisissez une colonne");
        reveal(action, coordL, coordC);
        if (checkVictory() == 1) {
            display();
            printf("Victoire!");
            game = 0;
        }
    }
    return 0;
}