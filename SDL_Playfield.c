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

// Estructuras de texto
typedef struct Texto {
	char string[100];
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;
	SDL_Rect rect;
} Text;

typedef struct Pieza2 {
	char letter;
	uint8_t size;
	uint8_t matrix[4][4];
	int8_t x, y;
	SDL_Texture* texture;
	SDL_Rect rect;
} Shape2;

// Estructura de piezas
typedef struct Pieza {
	SDL_Color color;
	bool matrix[4][4];
	double x, y;
	int size;
	char letter;
	SDL_Texture* texture;
	SDL_Rect rect;
} Shape;

typedef struct {
	int matrix[23][10];
} Playfield;

// Variables globales
SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* backgrounds[4];
SDL_Texture* gameboard;

// Flags y contadores
Uint8 currBackground = 0; // Indice de background actual
Uint8 droped; // Flag de drop
bool running; // Flag loop game

// Variables de control de tiempo y frames
float FPS;
Uint64 countFrames = 0; // Contador de frames
Uint64 start_time, current_time, capTimer, frame_time; // Tiempos

// Estructuras inicializadas
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

Text* textFPS;
Text* textIntruc;

// Funcion que inicializa todo SDL y demas librerias usadas
bool InitSDL() { 
	// Inicializar toda la biblioteca de SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Error initializing SDL2: %s\n", SDL_GetError());
		return 0;
	}
	// Inicializar SDL_TTF
	if (TTF_Init() == -1) {
		printf("Error initializing SDL_TTF: %s\n", SDL_GetError());
		return 0;
	}
	// Inicializar SDL_image
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("Error initializing SDL_Image: %s\n", SDL_GetError());
		return 0;
	}
	// Calidad de escalado
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") == SDL_FALSE) {
		printf("Error assigning scaling hint: %s\n", SDL_GetError());
	}
	// Crear ventana
	window = SDL_CreateWindow("Intento de tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (window == NULL) {
		printf("Error creating window: %s\n", SDL_GetError());
		return 0;
	}
	SDL_Surface* icon = IMG_Load("assets/udec_icon.webp");
	if (icon == NULL) {
		printf("Error assigning window icon: %s\n", IMG_GetError());
	}
	SDL_SetWindowIcon(window, icon); // Poner icono a la ventana
	SDL_FreeSurface(icon);
	// Crear renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		printf("Error creating renderer: %s\n", SDL_GetError());
		return 0;
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Color fondo (Negro)

	return 1;
}

// Funcion que apaga y limpia todos los subsystemas de SDL usados.
void QuitSDL () {
	bool error = false;
	if (*SDL_GetError() != '\0') {
		printf("LAST SDL ERROR: %s\n", SDL_GetError());
		error = true;
	}
	if (*TTF_GetError() != '\0') {
		printf("LAST SDL_TTF ERROR: %s\n", TTF_GetError());
		error = true;
	}
	if (*IMG_GetError() != '\0') {
		printf("LAST SDL_IMG ERROR: %s\n", IMG_GetError());
		error = true;
	}
	if (error) SDL_Delay(3000);

	TTF_Quit();
	IMG_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
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
void drop(Shape *curr, Shape *next) {
	*curr = *next;
	*next = blocks[rand()%7];
	droped = 60;
}

// Funcion que renderiza pieza
void renderPiece(Shape *s, SDL_Renderer *renderer) {
	s->rect.w = s->rect.h = TILE_SIZE;
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			if(s->matrix[i][j]) {
				s->rect.x = ((s->x + j) * TILE_SIZE);
				s->rect.y = ((s->y + i) * TILE_SIZE);
				SDL_RenderCopy(renderer, s->texture, NULL, &s->rect);
			}
		}
	}
}

// Funcion que recibe el input del juego
void input(Shape* curr, Shape* next) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT) {
			running = false;
			break;
		}
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_UP:
					curr->y--;
					break;
				case SDLK_DOWN:
					curr->y++;
					break;
				case SDLK_RIGHT:
					curr->x++;
					break;
				case SDLK_LEFT:
					curr->x--;
					break;
				case SDLK_z:
					if (!e.key.repeat) {
						rotation(curr, COUNTER_CLOCKWISE);
					}
					break;
				case SDLK_x:
					if (!e.key.repeat) {
						rotation(curr, CLOCKWISE);
					}
					break;
				case SDLK_a:
					if (!e.key.repeat) {
						rotation(curr, CLOCKWISE);
						rotation(curr, CLOCKWISE);
					}
					break;
				case SDLK_c:
						// hold(curr, holder);
					break;
				case SDLK_SPACE:
					if (!e.key.repeat) {
						drop(curr, next);
					}
					break;
				case SDLK_f: {
					int aux = currBackground;
					do {
						currBackground = rand() % 4;
					} while(currBackground == aux);
					break;
				}
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
	*text = (Text) {	// Se castea a un dato tipo "Text", ya que como estamos inicializandolo desde un puntero tenemos que usar un literal compuesto (googlea "Compound literal")
		.string		= "", 									// String del texto (vacio por ahora)
		.font 		= TTF_OpenFont(font, size),				// Fuente (Cargada con ayuda de TTF_OpenFont("path del font", tamaño letra))
		.color 		= color,								// Color del texto
		.texture	= NULL,									// Textura (NULL ya que se crea y asigna posteriormente)
		.rect 		= {x, y, 0, 0}							// Rect del texto (Posicion/Tamaño), el tamaño se asigna posteriormente al crear la textura
	};
	strcpy(text->string, str);	// Copiar parametro string en la string de estructura
	return text;
}

void loadBlocksTextures() {
	blocks[0].texture = IMG_LoadTexture(renderer, "assets/blocks/L.png");
	blocks[1].texture = IMG_LoadTexture(renderer, "assets/blocks/Z.png");
	blocks[2].texture = IMG_LoadTexture(renderer, "assets/blocks/I.png");
	blocks[3].texture = IMG_LoadTexture(renderer, "assets/blocks/J.png");
	blocks[4].texture = IMG_LoadTexture(renderer, "assets/blocks/O.png");
	blocks[5].texture = IMG_LoadTexture(renderer, "assets/blocks/S.png");
	blocks[6].texture = IMG_LoadTexture(renderer, "assets/blocks/T.png");
}

void loadBackgroundsTexture() {
	backgrounds[0] = IMG_LoadTexture(renderer, "assets/backgrounds/lvl1.png");
	backgrounds[1] = IMG_LoadTexture(renderer, "assets/backgrounds/lvl2v2.png");
	backgrounds[2] = IMG_LoadTexture(renderer, "assets/backgrounds/lvl3.png");
	backgrounds[3] = IMG_LoadTexture(renderer, "assets/backgrounds/lvl4v2.png");
}

// Funcion que carga textura de texto
void loadTextTexture(SDL_Renderer *renderer, Text *text) {
	if (text->texture != NULL) SDL_DestroyTexture(text->texture);
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
	if (SDL_RenderCopy(renderer, text->texture, NULL, &text->rect) < 0) printf("ERROR RENDERIZANDO TEXTO: %s\n", SDL_GetError());
}

// Funcion que libera texto junto a su textura y fuente cargada
void freeText(Text *text) {
	SDL_DestroyTexture(text->texture);
	TTF_CloseFont(text->font);
	free(text);
}

// Renderizar texturas de fondo
void renderBackground(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *gameboard, Shape *next) {
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_RenderCopy(renderer, gameboard, NULL, NULL);
	next->rect.w = next->rect.h = TILE_SIZE;
	for(int i = 0; i < next->size; i++) {
		for(int j = 0; j < next->size; j++) {
			if(next->matrix[i][j]) {
				switch (next->letter) {
					case 'O':
						next->rect.x = ((18 + j) * TILE_SIZE);
						break;
					case 'I':
						next->rect.x = ((17 + j) * TILE_SIZE);
						break;
					default:
						next->rect.x = ((17.5 + j) * TILE_SIZE);
						break;
				}
				next->rect.y = ((5 + i) * TILE_SIZE);
				SDL_RenderCopy(renderer, next->texture, NULL, &next->rect);
			}
		}
	}
	// printf("Error renderizando pieza: %s\n", SDL_GetError());
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	InitSDL();

	textFPS = initText("FPS: ", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255,255,255,200}, 5, 1);
	textIntruc = initText("Press F: Change background", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255,255,255,200}, 0, 1);

	gameboard = IMG_LoadTexture(renderer, "assets/gameboards/gameboard1.png");
	loadTextTexture(renderer, textIntruc);
	textIntruc->rect.x = SCREEN_WIDTH - textIntruc->rect.w - 5;
	loadBlocksTextures();
	loadBackgroundsTexture();

	Shape curr = blocks[rand() % 7];
	Shape next = blocks[rand() % 7];
	currBackground = rand() % 4;

	// Iniciar gameloop
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

	/* INPUT ********************************************************************************************/
		input(&curr, &next);

	/* LOGICA Y CAMBIOS *********************************************************************************/
		// SoftDrop (Cada 48 frames baja 1 celda)
		if (countFrames % 48 == 0 && droped == 0 && countFrames > 48) {
			curr.y++;
		} else if (droped > 0) {
			droped--;
		}

		// Cargar texto de FPS actuales
		if (countFrames > 0) {
			snprintf(textFPS->string + 5, 5, "%.1f", FPS);
			loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		}

	/* RENDER **********************************************************************************************/	
		SDL_RenderClear(renderer);

		renderBackground(renderer, backgrounds[currBackground], gameboard, &next);
		renderPiece(&curr, renderer);
		renderText(renderer, textIntruc);
		if (countFrames > 0) renderText(renderer, textFPS);
        
		SDL_RenderPresent(renderer);

	/* CONTROL DE FRAMES Y TIEMPOS **************************************************************************/
		++countFrames; // Contar frames
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS) 
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame anterior
		if (frame_time < SCREEN_TICKS_PER_FRAME) SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
	}

	freeText(textIntruc);
	freeText(textFPS);
	QuitSDL();

	return 0;
}