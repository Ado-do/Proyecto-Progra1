#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

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

// Variables globales
SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* fondo;
SDL_Texture* gameboard;
SDL_Texture* blocksTexture[7];

bool running; // Flag loop game
int droped; // Flag de drop
// Variables de control de tiempo y frames
float FPS;
uint64_t countFrames = 0; // Contador de frames
double start_time, current_time, capTimer, frame_time; // Tiempos

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
	SDL_Color color;
	bool matrix[4][4];
	double x, y;
	int size;
	char letter;
	SDL_Texture* texture;
} Shape;

// Arreglo de tetrominos
Shape blocks[7] = { 
	// L BLOCK
	{{255,127,0}, // Color Naranjo
	{{0,0,1,0} 
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X
	,INICIAL_Y + BOARD_Y
	, 3, 'L', NULL}
	// Z BLOCK
	,{{255,0,0}, // Color Rojo
	{{1,1,0,0}
	,{0,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X
	,INICIAL_Y + BOARD_Y
	, 3, 'Z', NULL}
	// I BLOCK
	,{{0,255,255}, // Color Celeste
	{{0,0,0,0}
	,{1,1,1,1}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X
	,INICIAL_Y + BOARD_Y
	, 4, 'I', NULL}
	// J BLOCK
	,{{0,0,255}, // Color Azul
	{{1,0,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X
	,INICIAL_Y + BOARD_Y
	, 3, 'J', NULL}
	// O BLOCK
	,{{255,255,0}, // Color Amarillo
	{{1,1,0,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X + 1
	,INICIAL_Y + BOARD_Y
	, 2, 'O', NULL}
	// S BLOCK
	,{{0,255,0}, // Color Verde
	{{0,1,1,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X
	,INICIAL_Y + BOARD_Y
	, 3, 'S', NULL}
	// T BLOCK
	,{{128,0,128}, // Color Morado
	{{0,1,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X
	,INICIAL_Y + BOARD_Y
	, 3, 'T', NULL}
};

// Funcion que inicializa todo SDL
void InitSDL() { 
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); // Calidad de escalado
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("Error inicializando SDL: %s\n", SDL_GetError()); // Inicializar toda la biblioteca de SDL
	if (TTF_Init() == -1) printf("Error al inicializar SDL_TTF: %s\n", SDL_GetError()); // Inicializar SDL_TTF
	// Crear ventana
	window = SDL_CreateWindow("Intento de tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Surface* icon = IMG_Load("assets/udec_icon.webp");
	if (icon == NULL) printf("Error al asignar icono: %s\n", SDL_GetError());
	SDL_SetWindowIcon(window, icon); // Poner icono a la ventana
	SDL_FreeSurface(icon);
	// Crear renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Color fondo (Negro)
}

// Funcion que rota piezas
void rotation(Shape* s, const int sense) {
	Shape copy = *s;
	switch (sense) {
		case CLOCKWISE:
			for(int i = 0; i < s->size; i++) {
				for(int j = 0; j < s->size; j++) {
					s->matrix[i][j] = copy.matrix[j][i];
				}
			}
			copy = *s;
			for(int i = 0; i < s->size/2; i++) {
				for(int j = 0; j < s->size; j++) {
					bool t = copy.matrix[i][j];
					s->matrix[i][j] = copy.matrix[s->size - i - 1][j];
					s->matrix[s->size - i - 1][j] = t;
				}
			}
			break;
		case COUNTER_CLOCKWISE:
			for(int i = 0; i < s->size; i++) {
				for(int j = 0; j < s->size; j++) {
					s->matrix[i][j] = copy.matrix[j][i];
				}
			}
			copy = *s;
			for(int i = 0; i < s->size; i++) {
				for(int j = 0; j < s->size/2; j++) {
					bool t = copy.matrix[i][j];
					s->matrix[i][j] = copy.matrix[i][s->size - j - 1];
					s->matrix[i][s->size - j - 1] = t;
				}
			}
			break;
		default:
			break;
	}
}

// Funcion que se ejecuta cuando se dropea la pieza y esta pasa al stack (Se crea una pieza nueva)
void drop(Shape* s) {
	*s = blocks[rand() % 7];
	droped = 60;
}

// Funcion que renderiza pieza
void renderPiece(const Shape *s, SDL_Rect *rect, SDL_Renderer *renderer) {
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			if(s->matrix[i][j]) {
				rect->x = ((s->x + j) * TILE_SIZE);
				rect->y = ((s->y + i) * TILE_SIZE);
				SDL_RenderCopy(renderer, s->texture, NULL, rect);
			}
		}
	}
}

// Funcion que recibe el input del juego
void input(Shape* cur) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT) {
			running = false;
			break;
		} 
		// if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_UP:
					cur->y--;
					break;
				case SDLK_DOWN:
					cur->y++;
					break;
				case SDLK_RIGHT:
					cur->x++;
					break;
				case SDLK_LEFT:
					cur->x--;
					break;
				case SDLK_z:
					if (!e.key.repeat) {
						rotation(cur, COUNTER_CLOCKWISE);
					}
					break;
				case SDLK_x:
					if (!e.key.repeat) {
						rotation(cur, CLOCKWISE);
					}
					break;
				case SDLK_a:
					if (!e.key.repeat) {
						rotation(cur, CLOCKWISE);
						rotation(cur, CLOCKWISE);
					}
					break;
				case SDLK_SPACE:
					if (!e.key.repeat) {
						drop(cur);
					}
					break;
				case SDLK_ESCAPE:
					running = false;
					break;
				default:
					break;
			}
		}
	}
}

// Funcion que inicializa objeto de la estructura Text
Text* initText(const char *str, const char *font, const int size, const SDL_Color color, const int x, const int y) {
	Text* text = malloc(sizeof(Text));
	*text = (Text){	// Se castea a un dato tipo "Text", ya que como estamos inicializandolo desde un puntero tenemos que usar un literal compuesto (googlea "Compound literal")
		.string		= "", 									// String del texto (vacio por ahora)
		.font 		= TTF_OpenFont(font, size),				// Fuente (Cargada con ayuda de TTF_OpenFont("path del font", tamaño letra))
		.color 		= color,								// Color del texto
		.texture	= NULL,									// Textura (NULL ya que se crea y asigna posteriormente)
		.rect 		= {x, y, 0, 0}							// Rect del texto (Posicion/Tamaño), el tamaño se asigna posteriormente al crear la textura
	};
	strcpy(text->string, str);	// Copiar parametro string en la string de estructura
	return text;
}

void loadBlocksTexture() {
	blocks[0].texture = IMG_LoadTexture(renderer, "assets/Blocks/L.png");
	blocks[1].texture = IMG_LoadTexture(renderer, "assets/Blocks/Z.png");
	blocks[2].texture = IMG_LoadTexture(renderer, "assets/Blocks/I.png");
	blocks[3].texture = IMG_LoadTexture(renderer, "assets/Blocks/J.png");
	blocks[4].texture = IMG_LoadTexture(renderer, "assets/Blocks/O.png");
	blocks[5].texture = IMG_LoadTexture(renderer, "assets/Blocks/S.png");
	blocks[6].texture = IMG_LoadTexture(renderer, "assets/Blocks/T.png");
}

// Funcion que carga textura de texto
void loadTextTexture(SDL_Renderer *renderer, Text *text) {
	SDL_Surface* textSurface = TTF_RenderText_Solid(text->font, text->string, text->color);
	if (textSurface == NULL) {
		printf("Error al intentar crear textSurface: %s\n", TTF_GetError());
	} else {
		text->rect.w = textSurface->w;
		text->rect.h = textSurface->h;
	}
	text->texture = SDL_CreateTextureFromSurface(renderer, textSurface); 
	if (text->texture == NULL) printf("Error al intentar crear textTexture: %s\n", SDL_GetError());
	SDL_FreeSurface(textSurface);
}

// Funcion que renderiza textura de texto
void renderText(SDL_Renderer *renderer, Text *text) {
	SDL_RenderCopy(renderer, text->texture, NULL, &text->rect);
}

// Funcion que libera texto junto a su textura y fuente cargada
void freeText(Text *text) {
	SDL_DestroyTexture(text->texture);
	TTF_CloseFont(text->font);
	free(text);
}

// Renderizar texturas de fondo
void renderBackground(SDL_Renderer* renderer, SDL_Texture* background, SDL_Texture* gameboard) {
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_RenderCopy(renderer, gameboard, NULL, NULL);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));

	InitSDL();
	Text* textFPS = initText("FPS: ", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255, 255, 255, 200}, 5, 1);
	fondo = IMG_LoadTexture(renderer, "assets/backgrounds/lvl1.png"); // Cargar Fondo
	gameboard = IMG_LoadTexture(renderer, "assets/gameboards/gameboard1.png");
	if (fondo == NULL || gameboard == NULL) printf("Error al crear textura: %s\n", SDL_GetError());
	
	loadBlocksTexture();
	Shape cur = blocks[rand() % 7];
	SDL_Rect rect;
	rect.w = TILE_SIZE;
	rect.h = TILE_SIZE;

	// Iniciar gameloop
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop
	while(running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

		input(&cur);

		// SoftDrop (Cada 48 frames baja 1 celda)
		if (countFrames % 48 == 0 && droped == 0 && countFrames > 48) {
			cur.y++;
		} else if (droped > 0) {
			droped--;
		}

		// Crear string de FPS y textura
		if (countFrames != 0) {
			snprintf(textFPS->string + 5, 5, "%.1f", FPS);
			loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		}

		SDL_RenderClear(renderer);

		renderBackground(renderer, fondo, gameboard);
		renderText(renderer, textFPS);
		renderPiece(&cur, &rect, renderer);
        
		SDL_RenderPresent(renderer);
		++countFrames; // Contar frames

		// Control de tiempo y frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame
		if (frame_time < SCREEN_TICKS_PER_FRAME) {
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		}
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS) 
	}

	freeText(textFPS);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}