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
#define INICIAL_X 4
#define INICIAL_Y 0
#define BOARD_WIDTH 12
#define BOARD_HEIGHT 24
#define BOARD_X_ORIGIN 246
#define BOARD_Y_ORIGIN 19

// * ORIGEN PLAYFIELD X= 246 Y= 19
// * DIMENSIONES PLAYFIELD W= 381 H= 839

enum sense {COUNTER_CLOCKWISE = 1, CLOCKWISE = 2, DOUBLE_CLOCKWISE = 3};
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
	char matrix[BOARD_HEIGHT][BOARD_WIDTH]; // 24x12 (Total con los bordes)
	// char matrix[BOARD_HEIGHT + 3 + 1][BOARD_WIDTH + 1 + 1]; // 24x12 (Total con los bordes)
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

// Variables globales
SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* backgrounds[4];

// Flags y contadores
Uint8 currBackground = 0; // Indice de background actual
bool up, right, left, softDrop, hardDrop, fall, hold, holded, preHolded;
Uint8 rotation;
Uint8 dropDelay; // Flag de drop
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

void printPosTetromino(Tetromino *curr, Playfield *playfield) {
	printf("Pos actual: (%d ,%d), playfield=\"%c\"\n", curr->x, curr->y, playfield->matrix[curr->y][curr->x]);
}

// Imprimir grilla actual
void printPlayfield(Playfield *playfield) {
	printf("La matriz de la grilla actual es:\n");
	// for (int i = 0; i < BOARD_HEIGHT + 4; ++i) {
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		printf("| ");
		// for (int j = 0; j < BOARD_WIDTH + 2; ++j) {
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			printf("%c ", playfield->matrix[i][j]);
		}
		printf("|\n");
	}
}

void initPlayfield(Playfield *playfield) {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (j == 0 || j == BOARD_WIDTH - 1) {
				playfield->matrix[i][j] = 'X';
			} else if (i == BOARD_HEIGHT - 1) {
				playfield->matrix[i][j] = 'X';
			} else {
				playfield->matrix[i][j] = ' ';
			}
		}
	}
	printPlayfield(playfield);
}

Tetromino newTetromino() {
	return tetrominoes[rand() % 7];
}

// Revisa colisiones de tetromino actual
bool fit(Playfield *playfield, Tetromino *curr) {
	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			if (curr->matrix[i][j] && playfield->matrix[i + curr->y][j + curr->x] != ' ') {
				// printf("x=%d, y=%d, playfield=%c\n", j + curr->x, i + curr->y, playfield->matrix[i + curr->y][j + curr->x]);
				printf("COLISION!!!!! WAAAAAAAAAAAAAA\n");
				return false;
			}
		}
	}
	return true;
}

// Funcion que rota piezas
void rotate(Tetromino *tetro, const Uint8 sense) {
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
		case DOUBLE_CLOCKWISE:
			for(int i = 0; i < tetro->size; i++) {
				for(int j = 0; j < tetro->size/2; j++) {
					bool t = copy.matrix[i][j];
					tetro->matrix[i][j] = copy.matrix[i][tetro->size - j - 1];
					tetro->matrix[i][tetro->size - j - 1] = t;
				}
			}
			rotate(tetro, CLOCKWISE);
			break;
		default:
			break;
	}
}

// Funcion que se ejecuta cuando se dropea la pieza y esta pasa al stack (Se crea una pieza nueva)
void drop(Playfield *playfield, Tetromino *curr, Tetromino *next) {
	bool dropeado = false;
	Tetromino tmp = *curr;
	/* DROPEAR PIEZA Y GRABARLA EN EL PLAYFIELD */
	while (!dropeado) {
		if (!fit(playfield, &tmp)) {
			tmp.y--;
			for (int i = 0; i < tmp.size; i++) {
				for (int j = 0; j < tmp.size; j++) {
					if (tmp.matrix[i][j]) {
						playfield->matrix[i + tmp.y][j + tmp.x] = tmp.shape;
					}
				}
			}
			dropeado = true;
		}
		else tmp.y++;
	}
	// SDL_DestroyTexture(tmp.color);
	// SDL_DestroyTexture(curr->color);
	*curr = *next;
	// SDL_DestroyTexture(next->color);
	*next = tetrominoes[rand()%7];
	dropDelay = 60;
}

// Funcion que recibe el input del juego
void gameInput(Tetromino *curr, Tetromino *next, Playfield *playfield) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT) {
			running = false;
			break;
		}
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_UP: 		curr->y--; break;
				case SDLK_DOWN:		softDrop = true; break;
				case SDLK_RIGHT:	right = true; break;
				case SDLK_LEFT:		left = true; break;
				case SDLK_z:
					if (!e.key.repeat) rotation = COUNTER_CLOCKWISE;
					break;
				case SDLK_x:
					if (!e.key.repeat) rotation = CLOCKWISE;
					break;
				case SDLK_a:
					if (!e.key.repeat) rotation = DOUBLE_CLOCKWISE;
					break;
				case SDLK_c: hold = true; break;
				case SDLK_SPACE:
					if (!e.key.repeat) hardDrop = true;
					break;
				case SDLK_f: {
					printPlayfield(playfield);
					int aux = currBackground;
					do {
						currBackground = rand() % 4;
					} while(currBackground == aux);
					break;
				}
				case SDLK_ESCAPE: running = false; break;
				default: break;
			}
		}
	}
}

// Game updater (revisa coliones antes de mover)
void gameUpdate(Playfield *playfield, Tetromino *curr, Tetromino *next, Tetromino *holder) {
	if (rotation) {
		rotate(curr, rotation);
		if (!fit(playfield, curr)) {
			switch (rotation) {
				case CLOCKWISE: rotate(curr, COUNTER_CLOCKWISE); break;
				case COUNTER_CLOCKWISE: rotate(curr, CLOCKWISE); break;
				case DOUBLE_CLOCKWISE: rotate(curr, DOUBLE_CLOCKWISE); break;
			}
		}
		rotation = 0;
	}
	if (hardDrop) {
		if (holded) holded = false;
		drop(playfield, curr, next);
		hardDrop = false;
	}
	if (softDrop) {
		curr->y++;
		if (!fit(playfield, curr)) curr->y--;
		softDrop = false;
	}
	if (hold) {
		if (!preHolded) preHolded = true;
		if (!holded) {
			*holder = *curr;
			*curr = *next;
			*next = newTetromino();
			holded = true;
		}
		hold = false;
	}
	if(right) {
		curr->x++;
		if (!fit(playfield, curr)) curr->x--;
		right = false;
	}
	if(left) {
		curr->x--;
		if (!fit(playfield, curr)) curr->x++;
		left = 0;
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
void renderBackground(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *gameboardInt) {
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_RenderCopy(renderer, gameboardInt, NULL, NULL);
}

void renderNextHold(SDL_Renderer *renderer, Tetromino *next, Tetromino *holder) {
	int currRectNext, currRectHolder, nextX, nextY, holderX, holderY;
	currRectNext = 0, currRectHolder = 0, nextX = 665, nextY = 200, holderX = 100, holderY = 200;
	switch (next->shape) {
		case 'O': nextX += 20; break;
		case 'I': nextX -= 20; nextY -= 5; break;
		default: break;
	}
	for(int i = 0; i < next->size; i++) {
		for(int j = 0; j < next->size; j++) {
			if(next->matrix[i][j]) {
				next->rects[currRectNext].w = next->rects[currRectNext].h = TILE_SIZE + 1;
				next->rects[currRectNext].x = (j * (TILE_SIZE + 1)) + nextX;
				next->rects[currRectNext].y = (i * (TILE_SIZE + 1)) + nextY;
				SDL_RenderCopy(renderer, next->color, NULL, &next->rects[currRectNext]);
				currRectNext++;
			}
		}
	}
	if (preHolded) {
		switch (holder->shape) {
			case 'O': holderX += 20; break;
			case 'I': holderX -= 20; holderY -= 5; break;
			default: break;
		}
		for(int i = 0; i < holder->size; i++) {
			for(int j = 0; j < holder->size; j++) {
				if(holder->matrix[i][j]) {
					holder->rects[currRectHolder].w = holder->rects[currRectHolder].h = TILE_SIZE + 1;
					holder->rects[currRectHolder].x = (j * (TILE_SIZE + 1)) + holderX;
					holder->rects[currRectHolder].y = (i * (TILE_SIZE + 1)) + holderY;
					SDL_RenderCopy(renderer, holder->color, NULL, &holder->rects[currRectHolder]);
					currRectHolder++;
				}
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
				tetro->rects[currRect].x = ((tetro->x - 1 + j) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
				tetro->rects[currRect].y = ((tetro->y + i) * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
				SDL_RenderCopy(renderer, tetro->color, NULL, &tetro->rects[currRect]);
				currRect++;
			}
		}
	}
}

void renderPlayfield(SDL_Renderer *renderer, Playfield *playfield) {
	for (int y = BOARD_HEIGHT - 2; y >= 0; y--) {
		for (int x = 1; x < BOARD_WIDTH - 1; x++) {
			if (playfield->matrix[y][x] != ' ') {
				SDL_Rect tmpRect;
				int nColor;
				tmpRect.w = tmpRect.h = TILE_SIZE + 1;
				tmpRect.x = ((x - 1) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
				tmpRect.y = (y * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
				switch (playfield->matrix[y][x]) {
					case 'L': nColor = 0; break;
					case 'Z': nColor = 1; break;
					case 'I': nColor = 2; break;
					case 'J': nColor = 3; break;
					case 'O': nColor = 4; break;
					case 'S': nColor = 5; break;
					case 'T': nColor = 6; break;
					default: break;
				}
				if (SDL_RenderCopy(renderer, tetrominoes[nColor].color, NULL, &tmpRect) < 0) {
					printf("ERROR RENDER PLAYFIELD: %s\n", SDL_GetError());
				}
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
	up = right = left = softDrop = hardDrop = fall = hold = holded = preHolded = false;
	
	SDL_Texture *gameboardInt, *gameboardExt;
	Text* textFPS;
	Text* textIntruc;
	Playfield* playfield = malloc(sizeof(Playfield));

	initPlayfield(playfield);

	textFPS = initText("FPS: ", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255,255,255,200}, 5, 1);
	textIntruc = initText("Press F to change background", "assets/fonts/upheaval.ttf", 20, (SDL_Color){255,255,255,200}, 0, 1);

	loadBackgroundsTexture(backgrounds);
	loadTetrominoesTexture(&playfield);
	gameboardExt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardExt.png");
	gameboardInt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardInt.png");
	loadTextTexture(renderer, textIntruc);
	textIntruc->rect.x = SCREEN_WIDTH - textIntruc->rect.w - 5;

	Tetromino curr = newTetromino();
	Tetromino next = newTetromino();
	Tetromino holder;
	currBackground = rand() % 4;

/* GAME LOOP ************************************************************************************************************************/
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

	/* INPUT ********************************************************************************************/
		gameInput(&curr, &next, playfield);

	/* LOGICA Y CAMBIOS *********************************************************************************/
		// SoftDrop (Cada 48 frames baja 1 celda)
		if (countFrames % 48 == 0 && dropDelay == 0 && countFrames > 48) {
			// fall = true;
			softDrop = true;
		} else if (dropDelay > 0) {
			dropDelay--;
		}
		// Colisiones?
		gameUpdate(playfield, &curr, &next, &holder);

		// Cargar texto de FPS actuales
		if (countFrames > 0) {
			snprintf(textFPS->string + 5, 6,"%.1f", FPS);
			loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		}

	/* RENDER **********************************************************************************************/	
		SDL_RenderClear(renderer);
		// Fondo ================================================================
		renderBackground(renderer, backgrounds[currBackground], gameboardInt);
		renderNextHold(renderer, &next, &holder);
		// Texto ================================================================
		if (countFrames > 0) renderText(renderer, textFPS);
		renderText(renderer, textIntruc);
		// Playfield ============================================================
		renderPlayfield(renderer, playfield);
		renderTetromino(renderer, &curr);
		// Interfaz superpuesta =================================================
		SDL_RenderCopy(renderer, gameboardExt, NULL, NULL);
        
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