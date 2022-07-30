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
#define TILE_SIZE 37
#define INICIAL_X 3
#define INICIAL_Y 0
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_X_ORIGIN 246
#define BOARD_Y_ORIGIN 19

// * ORIGEN PLAYFIELD X= 246 Y= 19
// * DIMENSIONES PLAYFIELD W= 381 H= 839

enum sense {COUNTER_CLOCKWISE, CLOCKWISE};
enum shapes {L, Z, I, J, O, S, T};

// Estructuras de texto
typedef struct Texto {
	char string[100];
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;
	SDL_Rect rect;
} Text;

// Estructura de piezas
typedef struct Tetromino {
	char shape;
	Uint8 size;
	SDL_Texture* color;
	bool matrix[4][4];
	Sint8 x, y;
	SDL_Rect rects[4];
} Tetromino;

typedef struct {
	char matrix[BOARD_HEIGHT + 3 + 1][BOARD_WIDTH + 1 + 1]; // 24x12 (Total con los bordes)
	// char matrix[BOARD_HEIGHT + 3][BOARD_WIDTH]; // 23x10
} Playfield;

// Estructuras inicializadas
Tetromino tetrominoes[7] = { 
	// L BLOCK
	{'L', 3, NULL,
	{{0,0,1,0} 
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// Z BLOCK
	{'Z', 3, NULL,
	{{1,1,0,0}
	,{0,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// I BLOCK
	{'I', 4, NULL,
	{{0,0,0,0}
	,{1,1,1,1}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// J BLOCK
	{'J', 3, NULL,
	{{1,0,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// O BLOCK
	{'O', 2, NULL,
	{{1,1,0,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X + 1, INICIAL_Y},
	// S BLOCK
	{'S', 3, NULL,
	{{0,1,1,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// T BLOCK
	{'T', 3, NULL,
	{{0,1,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y}
};
Playfield playfield = {0};

// Variables globales
SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* backgrounds[4];

// Flags y contadores
Uint8 currBackground = 0; // Indice de background actual
Uint8 droped; // Flag de drop
bool running; // Flag loop game

// Variables de control de tiempo y frames
float FPS;
Uint64 countFrames = 0; // Contador de frames
Uint64 start_time, current_time, capTimer, frame_time; // Tiempos

// Paths de assets
char* blockPaths[] = {
	"assets/blocks/L.png",
	"assets/blocks/Z.png",
	"assets/blocks/I.png",
	"assets/blocks/J.png",
	"assets/blocks/O.png",
	"assets/blocks/S.png",
	"assets/blocks/T.png"
};


// Funcion que inicializa todo SDL y demas librerias usadas
bool InitSDL() { 
	// Inicializar toda la biblioteca de SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Error initializing SDL2: %s\n", SDL_GetError());
		return 0;
	}
	// Inicializar SDL_TTF
	if (TTF_Init() == -1) {
		printf("Error initializing SDL_TTF: %s\n", TTF_GetError());
		return 0;
	}
	// Inicializar SDL_image
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("Error initializing SDL_Image: %s\n", IMG_GetError());
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

// Funcion que rota piezas
void rotation(Tetromino *tetro, const Uint8 sense) {
	Tetromino copy = *tetro;
	for(int i = 0; i < tetro->size; i++) {
			for(int j = 0; j < tetro->size; j++) {
				tetro->matrix[i][j] = copy.matrix[j][i];
			}
		}
	copy = *tetro;
	switch (sense) {
		case CLOCKWISE:
			for(int i = 0; i < tetro->size; i++) {
				for(int j = 0; j < tetro->size/2; j++) {
					bool t = copy.matrix[i][j];
					tetro->matrix[i][j] = copy.matrix[i][tetro->size - j - 1];
					tetro->matrix[i][tetro->size - j - 1] = t;
				}
			}
			break;
		case COUNTER_CLOCKWISE:
			for(int i = 0; i < tetro->size/2; i++) {
				for(int j = 0; j < tetro->size; j++) {
					bool t = copy.matrix[i][j];
					tetro->matrix[i][j] = copy.matrix[tetro->size - i - 1][j];
					tetro->matrix[tetro->size - i - 1][j] = t;
				}
			}
			break;
		default:
			break;
	}
}

// Funcion que se ejecuta cuando se dropea la pieza y esta pasa al stack (Se crea una pieza nueva)
void drop(Tetromino *curr, Tetromino *next) {
	*curr = *next;
	// *next = blocks[rand()%7];
	*next = tetrominoes[rand()%7];
	droped = 60;
}

void fit(const Playfield *playfield, Tetromino *curr) {
	for (int x = curr->x; x < (curr->x + curr->size); x++) {
		for (int y = curr->y; y < (curr->y + curr->size); y++) {
			
		}
	}
}

// Funcion que recibe el input del juego
void gameInput(Tetromino *curr, Tetromino *next) {
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

// Funcion que asigna texturas a tetrominos
void loadTetrominoesTexture() {
	// Asignar texturas
	for (int shape = 0; shape < 7; shape++) {
		tetrominoes[shape].color = IMG_LoadTexture(renderer, blockPaths[shape]);
	}
}

// Cargar fondos
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

// Renderizar texturas de fondo
void renderBackground(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *gameboard, Tetromino *next) {
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_RenderCopy(renderer, gameboard, NULL, NULL);
	int currRect, holdX, holdY;
	currRect = 0, holdX = 665, holdY = 185;

	switch (next->shape) {
		case 'O': holdX += 20; break;
		case 'I': holdX -= 20; holdY -= 5; break;
		default: break;
	}

	for(int i = 0; i < next->size; i++) {
		for(int j = 0; j < next->size; j++) {
			if(next->matrix[i][j]) {
				next->rects[currRect].w = next->rects[currRect].h = TILE_SIZE + 1;
				next->rects[currRect].x = (j * (TILE_SIZE + 1)) + holdX;
				next->rects[currRect].y = (i * (TILE_SIZE + 1)) + holdY;
				SDL_RenderCopy(renderer, next->color, NULL, &next->rects[currRect]);
				currRect++;
			}
		}
	}
}

void renderTetromino(SDL_Renderer *renderer, Tetromino *tetro) {
	int currRect = 0;
	for(int i = 0; i < tetro->size; i++) {
		for(int j = 0; j < tetro->size; j++) {
			if(tetro->matrix[i][j]) {
				tetro->rects[currRect].w = tetro->rects[currRect].h = TILE_SIZE + 1;
				tetro->rects[currRect].x = ((tetro->x + j) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
				tetro->rects[currRect].y = ((tetro->y + i) * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
				SDL_RenderCopy(renderer, tetro->color, NULL, &tetro->rects[currRect]);
				currRect++;
			}
		}
	}
}

// Funcion que libera texto junto a su textura y fuente cargada
void freeText(Text *text) {
	SDL_DestroyTexture(text->texture);
	TTF_CloseFont(text->font);
	free(text);
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

int main(int argc, char *argv[]) {
	srand(time(NULL));
	InitSDL();

/* DECLARAR Y INICIALIZAR *******************************************************************************************************/
	SDL_Texture* gameboard;
	Text* textFPS;
	Text* textIntruc;

	textFPS = initText("FPS: ", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255,255,255,200}, 5, 1);
	textIntruc = initText("Press F to change background", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255,255,255,200}, 0, 1);

	loadBackgroundsTexture(backgrounds);
	loadTetrominoesTexture();
	gameboard = IMG_LoadTexture(renderer, "assets/gameboards/gameboard1.png");
	loadTextTexture(renderer, textIntruc);
	textIntruc->rect.x = SCREEN_WIDTH - textIntruc->rect.w - 5;

	Tetromino curr = tetrominoes[rand() % 7];
	Tetromino next = tetrominoes[rand() % 7];
	// Tetromino hold;
	currBackground = rand() % 4;

/* GAME LOOP ************************************************************************************************************************/
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

	/* INPUT ********************************************************************************************/
		gameInput(&curr, &next);

	/* LOGICA Y CAMBIOS *********************************************************************************/
		// SoftDrop (Cada 48 frames baja 1 celda)
		// if (countFrames % 48 == 0 && droped == 0 && countFrames > 48) {
		// 	curr.y++;
		// } else if (droped > 0) {
		// 	droped--;
		// }

		// Cargar texto de FPS actuales
		if (countFrames > 0) {
			snprintf(textFPS->string + 5, 6,"%.1f", FPS);
			loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		}

	/* RENDER **********************************************************************************************/	
		SDL_RenderClear(renderer);

		renderBackground(renderer, backgrounds[currBackground], gameboard, &next);
		// renderPiece(&curr, renderer);
		renderTetromino(renderer, &curr);
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

/* CERRAR Y LIBERAR RECURSOS ****************************************************************************************************************************/
	freeText(textIntruc);
	freeText(textFPS);
	QuitSDL();

	return 0;
}