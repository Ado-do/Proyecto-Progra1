#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>

//! Constantes !//
#define SCREEN_WIDTH 870
#define SCREEN_HEIGHT 950
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000/SCREEN_FPS
#define TILE_SIZE 37
#define INITIAL_X 4
#define INITIAL_Y 1
#define BOARD_WIDTH 12 // (10 + 2 bordes laterales)
#define BOARD_HEIGHT 24 // (20 + 3 (superiores donde spawnea la pieza) + 1 (borde inferior)) // ? HACERLO 40 ALTURA
#define BOARD_X_ORIGIN 246 // Pos X de pixel de esquina izq superior del gameboard
#define BOARD_Y_ORIGIN 19 // Pos Y de pixel de esquina izq superior del gameboard

//! Enumeraciones !//
enum sense {COUNTER_CLOCKWISE = -1, CLOCKWISE = 1, DOUBLE_CLOCKWISE = 2};

//! Estructuras !//

typedef struct Texto {
	char string[100]; // String del texto
	TTF_Font* font; // Font del texto
	SDL_Color color; // Color del texto
	SDL_Texture* texture; // Textura del texto
	SDL_Rect rect; // Rect del texto (donde se renderiza el texto en pantalla)
	float size; // Escala del texto
} Text;	//* Estructuras de texto

typedef struct Fuentes {
	char* path; // Path del font
	Uint8 size; // Tamaño del font (Escala fija)
} Font;	//* Estructura de Fonts

typedef struct Piezas {
	char shape; // Forma
	Uint8 nShape; // Numero de pieza
	Uint8 size; // Tamaño
	bool matrix[4][4]; // Matriz representante
	Sint8 x, y; // Posicion en tablero
	SDL_Rect rects[4]; // Posicion y tamaño en pantalla
} Tetromino; //* Estructura de piezas

typedef struct Tablero {
	char matrix[BOARD_HEIGHT][BOARD_WIDTH]; // 24x12 (Total con los bordes y lineas superiores)
    // Tetromino curr;
	// Tetromino next;
    // Tetromino holder;
} Playfield; //* Estructura de tablero

// typedef struct Tetris {
//     Playfield playfield;
// 	Sint64 score;
// } Tetris;	//* Estructura que contiene datos de sesion actual

//! Estructuras inicializadas !//
//* Tablero/Grilla/Zona de juego
Playfield playfield;

//* Piezas principales
Tetromino curr, next, holder;

//* Fuentes utilizadas
Font upheavalFont = {"assets/fonts/upheaval.ttf", 20};

//* Arreglo de piezas
Tetromino tetrominoes[7] = { 
	// L BLOCK
	{'L', 0, 3,
	{{0,0,1,0} 
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X, INITIAL_Y},
	// Z BLOCK
	{'Z', 1, 3,
	{{1,1,0,0}
	,{0,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X, INITIAL_Y},
	// I BLOCK
	{'I', 2, 4,
	{{0,0,0,0}
	,{1,1,1,1}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X, INITIAL_Y},
	// J BLOCK
	{'J', 3, 3,
	{{1,0,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X, INITIAL_Y},
	// O BLOCK
	{'O', 4, 2,
	{{1,1,0,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X + 1, INITIAL_Y},
	// S BLOCK
	{'S', 5, 3,
	{{0,1,1,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X, INITIAL_Y},
	// T BLOCK
	{'T', 6, 3,
	{{0,1,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INITIAL_X, INITIAL_Y}
};

//* Textos
Text *textFPS, *textIntruc, *textScore, *textLevel, *textLines, *textRecord;

//! Variables globales ======================================================

//* Variables principales SDL
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event events;

//* Texturas globales
SDL_Texture *gameboardInt, *gameboardExt;
SDL_Texture *blockColors[7], *ghostBlock, *lockBlock;
SDL_Texture *backgrounds[4], *gameOverTextures[9];
SDL_Rect gameOverRect;

//* Indices para fondos
Uint8 currBackground, currGameOverTitle = 1; // Indice de background actual

//* Flags de input
bool up, right, left, softD, hardD, fall, hold; // Controles flags
Sint8 rotation; // Rotation flag

//* Flags y contadores de updateGame y logica tetris
Uint8 firstThreeDrops; // Primero 3 drops count
bool droped; // Drop flag
bool holded, firstHold; // Hold flags
Uint8 lastDropedRow, lastDropedSize; // Propiedades ultima pieza dropeada
Uint8 lastStackRow; // Ultima fila ocupada por stack (ALTURA)
bool lock_delayFRUNA; // Lock delay flag //TODO: RESETEAR DELAY SEGUN CASOS DE RESET DE TETRIS GUIDELINE
Uint32 fallDelay; // Contador de delay hardD

//* Flags y variable de dificultad y score
bool nextLevel; // Flag level up
bool newScore; // Flag score
Uint8 clearedLines; // Contador de lineas eliminadas
Uint8 level, combo;
Uint32 totalLines, currentLines;
Sint64 score;
float difficulty;

//* Flags principales gameloop
bool running; // Flag loop game
bool restart; // Flag restart
bool gameOver; // Flag game over

//* Variables de control de tiempo y frames
float FPS;
Uint64 totalFrames; // Contador de frames
Uint64 start_time, current_time, capTimer, frame_time, lock_timer; // Tiempos

//* Paths de assets
char* blockPaths[] = {
	"assets/blocks/L.png",
	"assets/blocks/Z.png",
	"assets/blocks/I.png",
	"assets/blocks/J.png",
	"assets/blocks/O.png",
	"assets/blocks/S.png",
	"assets/blocks/T.png",
	"assets/blocks/ghost.png",
	"assets/blocks/lock.png"
};
char* backgroundsPath[] = {
	"assets/backgrounds/lvl1.png",
	"assets/backgrounds/lvl2v2.png",
	"assets/backgrounds/lvl3v2.png",
	"assets/backgrounds/lvl4v2.png"
};
char* gameOverPath[] = {
	"assets/backgrounds/gameover/game-over.png",
	"assets/backgrounds/gameover/game-over1.png",
	"assets/backgrounds/gameover/game-over2.png",
	"assets/backgrounds/gameover/game-over3.png",
	"assets/backgrounds/gameover/game-over4.png",
	"assets/backgrounds/gameover/game-over5.png",
	"assets/backgrounds/gameover/game-over6.png",
	"assets/backgrounds/gameover/game-over7.png",
	"assets/backgrounds/gameover/game-over8.png"
};

//! Declarar funciones
//* Inicializar
bool initSDL(SDL_Window **ptrWindow, SDL_Renderer **ptrRenderer);
void initPlayfield(Playfield *playfield);
Text* initText(const char *str, Font *font, const SDL_Color color, const int x, const int y, const float size);

//* Cargar texturas
void loadTetrominoesTexture(SDL_Renderer *renderer);
void loadBackgroundsTexture(SDL_Renderer *renderer, SDL_Texture **backgrounds);
void loadGameOverTexture(SDL_Renderer *renderer, SDL_Texture **gameOverTextures);
void loadTextTexture(SDL_Renderer *renderer, Text *text);

//* Funciones tablero
void printPlayfield(Playfield *playfield);
void updatePlayfield(Playfield *playfield, Tetromino *curr, Tetromino *next);
void newTetromino(Tetromino *curr, Tetromino *next);

//* Funciones game loop
void tetrisGameScreen(Playfield *playfield, Tetromino *curr, Tetromino *next, Tetromino *holder);

void gameInput(Tetromino *curr, Tetromino *next, Playfield *playfield);
void gameUpdate(Playfield *playfield, Tetromino *curr, Tetromino *next, Tetromino *holder);
bool gameOverCheck(Playfield *playfield, Tetromino *curr);

void gameOverScreen();

//* Loyica de tetris
int checkLineState(Playfield *playfield, Uint8 row);
int deleteLines(Playfield *playfield);
bool collision(Playfield *playfield, Tetromino *curr);
void countStackHeight(Playfield *playfield);
void wallKickFRUNA(Playfield *playfield, Tetromino *curr, Sint8 rotation);
void rotateTetromino(Tetromino *tetro, const Sint8 sense);
void hardDropTetromino(Playfield *playfield, Tetromino *curr, Tetromino *next);
void softDropTetromino(Playfield *playfield, Tetromino *curr, Tetromino *next);
bool checkFallTime(Uint64 totalFrames);
void calculateScore(int linesCleared);
float calculateDifficulty(Uint8 level) ;

//* Renderizado
void renderText(SDL_Renderer *renderer, Text *text);
void renderFPS(SDL_Renderer *renderer, Text *textFPS);
void renderBackground(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *gameboardInt);
void renderGameOver(SDL_Renderer *renderer, SDL_Texture **gameOverTextures, Text *textRecord);
void renderNextHold(SDL_Renderer *renderer, Tetromino *next, Tetromino *holder);
void renderTetromino(SDL_Renderer *renderer, Tetromino *tetro);
void renderGhostTetromino(SDL_Renderer *renderer, Playfield *playfield, Tetromino *curr);
void renderPlayfield(SDL_Renderer *renderer, Playfield *playfield);
void framesControl();

//* Liberar y cerrar
void destroyText(Text *text);
void quitSDL(SDL_Window *window, SDL_Renderer *renderer);