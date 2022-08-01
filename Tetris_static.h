#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
} Text;		//* Estructuras de texto

typedef struct Fuentes {
	char* path; // Path del font
	Uint8 size; // Tamaño del font (Escala fija)
} Font;		//* Estructura de Fonts

typedef struct Piezas {
	char shape; // Forma
	Uint8 nShape; // Numero de pieza
	Uint8 size; // Tamaño
	bool matrix[4][4]; // Matriz representante
	Sint8 x, y; // Posicion en tablero
	SDL_Rect rects[4]; // Posicion y tamaño en pantalla
} Tetromino;	//* Estructura de piezas

typedef struct Tablero {
	char matrix[BOARD_HEIGHT][BOARD_WIDTH]; // 24x12 (Total con los bordes y lineas superiores)
} Playfield;	//* Estructura de tablero

typedef struct Tetris {
    Playfield playfield;
    Tetromino curr;
	Tetromino next;
    Tetromino holder;
} Tetris;	//* Estructura que contiene datos de sesion actual

//! Estructuras inicializadas !//
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

Text* textFPS;
Text* textIntruc;

//! Variables globales ======================================================
//* Variables SDL
SDL_Window* window;
SDL_Renderer* renderer;

//* Texturas globales
SDL_Texture *gameboardInt, *gameboardExt;
SDL_Texture *blockColors[7], *ghostBlock, *lockBlock;
SDL_Texture *backgrounds[4];

//* Flags y contadores (se inicializan en 0)
Uint8 currBackground; // Indice de background actual
//* Flags de input
bool up, right, left, softD, hardD, fall, hold;  // Controles flags
Sint8 rotation; // Rotation flag

int deletedLines; // Contador de lineas eliminadas
int lastRow, lastSize; // Propiedades ultima pieza dropeada
bool holded, firstHold; // Hold flags
bool firstDrop; // Drop flags
bool lock_delay; // Lock delay flag
Uint8 dropDelay; // Contador de delay hardD

bool running; // Flag loop game
bool restart; // Flag restart
bool gameOver; // Flag game over

//* Variables de control de tiempo y frames
float FPS;
Uint64 countFrames; // Contador de frames
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