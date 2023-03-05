
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINES 100
#define MAX_COLS 100

// initialisation des tableaux
int table[MAX_LINES][MAX_COLS];
int revealedTable[MAX_LINES][MAX_COLS];

typedef struct Coord
{
  int x;
  int y;
} Coord;


// game est un booléen
int game = 1;

// première coordonnée pour ne pas mettre de mine
int firstMoveLine, firstMoveCol;

// dimensions du champ de mines, des mines et drapeaux
int numCols, numLines, numMines, numFlags;

// afficher une question + vider la réponse en cas de mauvaise réponse
int askInt(int min, int max, char *message) {
  int coord = 0;
  while (!(coord >= min && coord <= max)) {
    printf("%s (entre %d et %d):", message, min, max);
    scanf("%d", &coord);
    while (getchar() != '\n');
  }
  return coord;
}

// vérifier la position des mine autour de la case séléctionné
int check(int l, int c) {
  return table[l - 1][c - 1] + table[l - 1][c] + table[l - 1][c + 1] +
         table[l][c - 1] + table[l][c + 1] + table[l + 1][c - 1] +
         table[l + 1][c] + table[l + 1][c + 1];
}

// initialiser un tableau vide
void emptyTable() {
  // init du plateau de jeu
  for (int i = 0; i < MAX_LINES; ++i)
    for (int j = 0; j < MAX_COLS; ++j)
    {
      table[i][j] = 0;
      revealedTable[i][j] = 0;
    }
}

// afficher en jeu le nombre de colonne
void printMinefieldHeader(int n){
  for(int i=0;i<n;i++)
    printf("%02d ", i+1);
  printf("\n");
}

// afficher le tableau pendant le jeu
void display() {
  // https://symbl.cc/en/
  printf("🚩 = %d\n", numFlags);
  // affichage du plateau de jeu
  printMinefieldHeader(numCols);
  for (int i = 1; i < numLines+1; ++i) {
    for (int j = 1; j < numCols+1; ++j) {
      if (revealedTable[i][j] == 0) {
        printf("__|");
      } else if (revealedTable[i][j] == 1) {
        if (table[i][j] == 1) {
          printf("🧨|");
        } else {
          printf("_%d|", check(i, j));
        }
      } else if (revealedTable[i][j] == 2) {
        printf("🚩|");
      }
    }
    printf("%d\n", i);
  }
}

// afficher une version du tableau avec les mines pour faciliter le debugage
void displayDebug() {
  printf("debug: \n");
  printMinefieldHeader(numCols);
  // printf("%d\n",numCols+1);
  // https://symbl.cc/en/
  // affichage du plateau de jeu
  for (int i = 1; i < numLines+1; ++i) {
    for (int j = 1; j < numCols+1; ++j) {
      if (table[i][j] == 1) {
        printf("🧨|");
      } else {
        printf("_%d|", check(i, j));
        // printf("__|");
      }
    }
    printf("%d\n", i);
  }
}

// place des mines aléatoirement
void addMines(int n) {
  srand(time(NULL));
  int l=0,c=0;
  while(n>0) {
    int lastL=l, lastC=c;
    l=(rand()%numLines)+1, c=(rand()%numCols)+1;
    if(table[l][c]!=1){
      table[l][c] = 1;
      n--;
    }
    if (check(firstMoveLine,firstMoveCol)!=0 || (l==firstMoveLine && c==firstMoveCol)){
      // printf("here\n");
      table[l][c]=0;
      n++;
    }
  }
}

// autre algorithme de placement de mine
void mineAlgo(int n) {
  srand(time(NULL));
  // on utilise des variable temporaire pour des questions de lisibilité
  int fl=firstMoveLine, fc=firstMoveCol;
  Coord options[numLines * numCols];
  // remplissage de la liste de coordonnées possibles
  while(n-->0){
    int len = 0;
    for(int i=1; i<numLines+1 ; i++){
      for(int j=1; j<numCols+1 ; j++){
        if( (i!=fl-1 && j!=fc-1) && (i!=fl-1 && j!=fc) && (i!=fl-1 && j!=fc+1) && 
            (i!=fl && j!=fc-1) && (i!=fl && j!=fc) && (i!=fl && j!=fc+1) && 
            (i!=fl+1 && j!=fc-1) && (i!=fl+1 && j!=fc) && (i!=fl+1 && j!=fc+1) && table[i][j]!=1 ){
          options[len].x = i;
          options[len++].y = j;
        }
      }
    }
    int randCoord = rand()%len;
    table[options[randCoord].x][options[randCoord].y] = 1;
  }  
}

// vérifier la condition de victoire
int checkVictory() {
  int freeSpaces = (numLines*numCols)-numMines;
  for(int i=1; i<numLines+1; i++){
    for(int j=1; j<numCols+1; j++){
      if (revealedTable[i][j] == 1 && table[i][j] == 0){
        freeSpaces--;
      }
    }
  }
  if(freeSpaces==0){
    return 1;
  }
  // printf("free spaces = %d\n",freeSpaces);
  return 0;
}

// révéler la case séléctionné
void reveal(int action, int l, int c) {
  // si le joueur veut révéler une case
  if (action == 1) {
    revealedTable[l][c] = 1;
    // si le joueur révèle une mine
    if (table[l][c] == 1 && action == 1) {
      game = 0;
      for (int i = 1; i < numLines+1; i++) {
        for (int j = 1; j < numCols+1; j++) {
          if (table[i][j] == 1) {
            revealedTable[i][j] = 1;
          }
        }
      }
      display();
      printf("perdu...\n");
      // sinon s'il révèle un 0, on répète l'opération
    } else if (check(l, c) == 0 && action == 1) {
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
  } else if (action == 2) {
    // on place un drapeau
    if (revealedTable[l][c] == 0) {
      revealedTable[l][c] = 2;
      numFlags--;
      // s'il s'agit d'un drapeau, on le retire
    } else if (revealedTable[l][c] == 2) {
      revealedTable[l][c] = 0;
      numFlags++;
    }
  }
}

// Demande au joueur de choisir le mode de jeu et modifie les valeurs du tableau et le nombre de mines.
void setGameMode(){
  int gameMode = askInt(1, 4, "Choisissez un mode de jeu (1 = FACILE | 2 = MOYEN | 3 = DIFFICILE | 4 = CUSTOM)");
  if (gameMode == 1) {
    numLines = 8;
    numCols = 10;
    numMines = 10;
  } else if (gameMode == 2) {
    numLines = 14;
    numCols = 18;
    numMines = 40;
  } else if (gameMode == 3) {
    numLines = 20;
    numCols = 24;
    numMines = 99;
  } else if (gameMode == 4){
    numLines = askInt(1, MAX_LINES, "Choisissez le nombre de lignes du champ de mines");
    numCols = askInt(1, MAX_COLS, "Choisissez le nombre de colonnes du champ de mines");
    numMines = askInt(1, numLines*numCols-9, "Combien de mines voulez-vous");
  }
  numFlags = numMines;
}

//lancer le jeu
int main() {
  setGameMode();
  emptyTable();
  printf("Faites votre première action pour commencer\n");
  firstMoveLine = askInt(1, numLines, "choisissez une ligne");
  firstMoveCol = askInt(1, numCols, "choisissez une colonne");
  mineAlgo(numMines);
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
}