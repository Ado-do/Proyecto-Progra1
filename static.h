#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// Constantes
#define SCREEN_WIDTH 870
#define SCREEN_HEIGHT 950
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000/SCREEN_FPS
#define TILE_SIZE 38
#define BOARD_X 6.472
#define BOARD_Y 2.1
#define INICIAL_X 3
#define INICIAL_Y -1

enum sense {COUNTER_CLOCKWISE, CLOCKWISE};

// Estructuras de texto
typedef struct Texto {
	char string[100];
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;
	SDL_Rect rect;
} Text;

// Estructura de piezas
typedef struct Pieza {
	char letter;
	int size;
	bool matrix[4][4];
	int x, y;
	SDL_Color color;
	SDL_Texture* texture;
	SDL_Rect rect;
} Shape;

// Variables SDL globales
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* fondo;
SDL_Texture* gameboard;
