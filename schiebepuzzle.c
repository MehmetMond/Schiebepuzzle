#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4 // zeilen (Horizontalen) = z
#define M 4 // spalten (Vertikalen)  = s
#define K 50

typedef struct {
  char chX;
  char chY;
} Position;

typedef struct {
  int ariPuzzle[N][M];
  Position stFreiePosition;
} Puzzle;

typedef struct ZuegeListe {
  Position stDelta;
  struct ZuegeListe *pstPrev;
} ZuegeListe;

// Design des Ladebalksen
void ladebalken(float prozent) {
  const float breite = 50; // Breite des Ladebalkens
  float gefuellt = (prozent)*breite / 100;
  printf("\r[");
  for (float i = 1; i < breite; i++) {
    if (i < gefuellt) {
      printf("#");
    } else {
      printf(" ");
    }
  }
  printf("] %.2f%% %f", prozent, gefuellt);
  fflush(stdout); // leert den Puffer
}
// Erstellt das Spielfeld
void InitPuzzle(Puzzle *pstAktPuzzle) {

  int count = 1;
  for (int z = 0; z < N; z++) {
    for (int s = 0; s < M; s++) {
      if (z == N - 1 && s == M - 1) {
        pstAktPuzzle->ariPuzzle[z][s] = 0;
        pstAktPuzzle->stFreiePosition.chX = s;
        pstAktPuzzle->stFreiePosition.chY = z;
      } else {
        pstAktPuzzle->ariPuzzle[z][s] = count++;
      }
    }
  }
}
// Laesst das Spielfeld darstellen
void PrintPuzzle(Puzzle *pstAktPuzzle) {

  char ObenLinks = '\xDA';
  char ObenRechts = '\xBF';
  char UntenLinks = '\xC0';
  char UntenRechts = '\xD9';
  char Horizontal = '\xC4';
  char Vertikal = '\xB3';
  char LinkesT = '\xC3';
  char RechtesT = '\xB4';
  char UnteresT = '\xC1';
  char OberesT = '\xC2';
  char DoppelObenLinks = '\xC9';
  char DoppelObenRechts = '\xBB';
  char DoppelUntenLinks = '\xC8';
  char DoppelUntenRechts = '\xBC';
  char DoppelHorizontal = '\xCD';
  char DoppelVertikal = '\xBA';
  char Kreuzung = '\xC5';

  // Obere haelfte des kasten
  printf("%c", ObenLinks);
  for (int s = 0; s < M; s++) {
    printf("%c%c%c", Horizontal, Horizontal, Horizontal);
    if (s < M - 1) {
      printf("%c", OberesT);
    }
  }
  printf("%c\n", ObenRechts);

  // Vertikale Fingens
  for (int z = 0; z < N; z++) {
    for (int s = 0; s < M; s++) {
      if (s == 0) {
        printf("%c", Vertikal);
      }
      if (pstAktPuzzle->ariPuzzle[z][s] == 0) {
        printf(" 0 "); // Mal schauen ob das mal probleme gibt falls ja
                       // <---------------------------hab ich weird gemacht (hab
                       // die Null)
      } else {
        printf("%2d ", pstAktPuzzle->ariPuzzle[z][s]);
      }
      printf("%c", Vertikal);
    }
    printf("\n");

    // Mittlere Grenze
    if (z < N - 1) {
      printf("%c", LinkesT);
      for (int s = 0; s < M; s++) {
        printf("%c%c%c", Horizontal, Horizontal, Horizontal);
        if (s < M - 1) {
          printf("%c", Kreuzung);
        }
      }
      printf("%c\n", RechtesT);
    }
  }
  // UntereGrenze
  printf("%c", UntenLinks);
  for (int s = 0; s < M; s++) {
    printf("%c%c%c", Horizontal, Horizontal, Horizontal);
    if (s < M - 1) {
      printf("%c", UnteresT);
    }
  }
  printf("%c\n", UntenRechts);
}

void UpdatePuzzle(Puzzle *pstAktPuzzle, Position *pstDPos) {
  int neuesX = pstAktPuzzle->stFreiePosition.chX + pstDPos->chX;
  int neuesY = pstAktPuzzle->stFreiePosition.chY + pstDPos->chY;

  if (neuesX >= 0 && neuesX < M && neuesY >= 0 && neuesY < N) {
    pstAktPuzzle->ariPuzzle[pstAktPuzzle->stFreiePosition.chY]
                           [pstAktPuzzle->stFreiePosition.chX] =
        pstAktPuzzle->ariPuzzle[neuesY][neuesX];
    pstAktPuzzle->ariPuzzle[neuesY][neuesX] = 0;
    pstAktPuzzle->stFreiePosition.chX = neuesX;
    pstAktPuzzle->stFreiePosition.chY = neuesY;
  }
}

Position ZufallsPosition(Puzzle *pstAktPuzzle) {
  Position result;
  int valid = 0;

  while (!valid) {
    int direction = rand() % 4;

    switch (direction) {
      // links
    case 0:
      if (pstAktPuzzle->stFreiePosition.chX < M - 1) {
        result.chX = 1;
        result.chY = 0;
        valid = 1;
      }
      break;
      // rechts
    case 1:
      if (pstAktPuzzle->stFreiePosition.chX > 0) {
        result.chX = -1;
        result.chY = 0;
        valid = 1;
      }
      break;
      // oben
    case 2:
      if (pstAktPuzzle->stFreiePosition.chY < N - 1) {
        result.chX = 0;
        result.chY = 1;
        valid = 1;
      }
      break;
      // unten
    case 3:
      if (pstAktPuzzle->stFreiePosition.chY > 0) {
        result.chX = 0;
        result.chY = -1;
        valid = 1;
      }
      break;
    }
  }

  return result;
}

// l
void AddZug(ZuegeListe **pstZuegeListe, Position stDPos) {
  ZuegeListe *newElement = (ZuegeListe *)malloc(sizeof(ZuegeListe)); // l1
  newElement->stDelta = stDPos;                                      // l2
  newElement->pstPrev = *pstZuegeListe;                              // l3
  *pstZuegeListe = newElement;                                       // l4
}

void LoesePuzzle(Puzzle *pstAktPuzzle, ZuegeListe **pstZuegeListe, int *mAnzahl,
                 int *m, int *wasdAnzahl, int *wasd) {

  // int eingabeScanf;
  // int anzahlgelesen;
  char eingabeLadebalken;
  float prozent = 0;
  int anzahl = (K * *mAnzahl) + *wasdAnzahl;

  while (*pstZuegeListe !=
         NULL) { // solang ein zug vorhanden ist laueft die schleife
    Position invMove;
    invMove.chX = -(*pstZuegeListe)->stDelta.chX;
    invMove.chY = -(*pstZuegeListe)->stDelta.chY;

    UpdatePuzzle(pstAktPuzzle, &invMove);

    ZuegeListe *temp = *pstZuegeListe;
    *pstZuegeListe =
        (*pstZuegeListe)->pstPrev; // zug wird aus der liste entfernt
    free(temp);                    // speicher freigegeben

    system("cls"); // bildschirm wird geloescht
    PrintPuzzle(pstAktPuzzle);
    ladebalken(prozent);
    printf("\nPuzzle wird gel\x94st");

    if (anzahl % K == 1) {
      eingabeLadebalken = _getch(); // FIX: Replaced getch() with _getch()
      if (eingabeLadebalken == 'l' || eingabeLadebalken == 'L') {
        prozent += 100.0F /
                   (*mAnzahl *
                    K); // Beachten Int und Float dividieren und multiplizieren
        if (prozent > 100)
          prozent = 100;

        if (*pstZuegeListe == NULL) {
          system("cls");
          PrintPuzzle(pstAktPuzzle);
          ladebalken(prozent);
          printf("\n\nPuzzle wurde gel\x94st!!!!\nDruecken Sie bitte die "
                 "Eingabe Taste: ");
          getchar();
          getchar();
          // anzahlgelesen = scanf("%d", &eingabeScanf);
        }
      }
    } else {
      eingabeLadebalken = _getch(); // FIX: Replaced getch() with _getch()
      if (eingabeLadebalken == 'l' || eingabeLadebalken == 'L') {
        prozent += 100.0F / (*wasdAnzahl + (K * *mAnzahl));
        if (prozent > 100)
          prozent = 100;

        if (*pstZuegeListe == NULL) {
          system("cls");
          PrintPuzzle(pstAktPuzzle);
          ladebalken(prozent);
          printf("\n\nPuzzle wurde gel\x94st!!!!\nDruecken Sie bitte die "
                 "Eingabe Taste: ");
          getchar();
          getchar();
          // anzahlgelesen = scanf("%d", &eingabeScanf);
        }
      }

    } // getch();
  }
  // setzt die noetigen werten yurueck
  *m = 1;
  *mAnzahl = 0;
  *wasd = 1;
  *wasdAnzahl = 1;
  anzahl = 0;
  prozent = 0;
}

void FreiZuegeListe(ZuegeListe **pstZuegeListe) {
  while (*pstZuegeListe != NULL) {
    ZuegeListe *temp = *pstZuegeListe;
    *pstZuegeListe = (*pstZuegeListe)->pstPrev;
    free(temp);
  }
}

int main() {
  Puzzle stAktPuzzle;
  Position stDPos;
  ZuegeListe *pstZuegeListe = NULL;
  int iUpdateDisplay = 1;
  char getch_char; // Renamed to avoid confusion with the function name
  int m = 1;
  int mAnzahl = 0;
  int wasd = 1;
  int wasdAnzahl = 0;

  srand((unsigned int)time(NULL)); // FIX: Cast time_t to unsigned int
  InitPuzzle(&stAktPuzzle);

  do {
    if (iUpdateDisplay) {
      system("cls"); // Bildschirm löschen (ChatGPT)
      PrintPuzzle(&stAktPuzzle);
      printf("v/V: Spiel verlassen\n" // ä = \x84 | ö = \x94 ---- ü = \x81
             "m/M: Puzzle Mischen\nl/L: Puzzle l\x94sen\n"
             "a/A: Tile nach links schieben\n"
             "d/D: Tile nach rechts schieben\n"
             "w/W: Tile nach oben schieben\n"
             "s/S: Tile nach unten schieben\n");
      iUpdateDisplay = 0;
    }

    getch_char = _getch();
    int iUpdatePuzzle = 0;

    switch (tolower(
        getch_char)) { // tolower = google
                  // (https://www.programiz.com/c-programming/library-function/ctype.h/tolower)
    case 'a':
      wasdAnzahl = wasd++;
      if (stAktPuzzle.stFreiePosition.chX < M - 1) {
        stDPos.chX = 1;
        stDPos.chY = 0;
        iUpdatePuzzle = 1;
      }
      break;
    case 'd':
      wasdAnzahl = wasd++;
      if (stAktPuzzle.stFreiePosition.chX > 0) {
        stDPos.chX = -1;
        stDPos.chY = 0;
        iUpdatePuzzle = 1;
      }
      break;
    case 'w':
      wasdAnzahl = wasd++;
      if (stAktPuzzle.stFreiePosition.chY < N - 1) {
        stDPos.chX = 0;
        stDPos.chY = 1;
        iUpdatePuzzle = 1;
      }
      break;
    case 's':
      wasdAnzahl = wasd++;
      if (stAktPuzzle.stFreiePosition.chY > 0) {
        stDPos.chX = 0;
        stDPos.chY = -1;
        iUpdatePuzzle = 1;
      }
      break;
    case 'm':
      mAnzahl = m++;
      for (int i = 0; i < K; i++) {
        Position randomMove = ZufallsPosition(&stAktPuzzle);
        UpdatePuzzle(&stAktPuzzle, &randomMove);
        AddZug(&pstZuegeListe, randomMove);
      }
      iUpdateDisplay = 1;
      break;
    case 'l':
      LoesePuzzle(&stAktPuzzle, &pstZuegeListe, &mAnzahl, &m, &wasdAnzahl,
                  &wasd);
      iUpdateDisplay = 1;
      break;
    }

    if (iUpdatePuzzle) {
      UpdatePuzzle(&stAktPuzzle, &stDPos);
      AddZug(&pstZuegeListe, stDPos);
      iUpdateDisplay = 1;
    }

  } while (tolower(getch_char) != 'v');

  FreiZuegeListe(&pstZuegeListe);
  return 0;
}