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
#define INICIAL_Y 1
#define BOARD_WIDTH 12 // (10 + 2 bordes laterales)
#define BOARD_HEIGHT 24 // (23 + 1 bordes inferior) // TODO: HACERLO 40 ALTURA
#define BOARD_X_ORIGIN 246
#define BOARD_Y_ORIGIN 19

// * ORIGEN PLAYFIELD X= 246 Y= 19
// * DIMENSIONES PLAYFIELD W= 381 H= 839

enum sense {COUNTER_CLOCKWISE = 1, CLOCKWISE = 2, DOUBLE_CLOCKWISE = 3};
enum shapes {L, Z, I, J, O, S, T};

// Estructuras de texto
typedef struct Texto {
	char string[100]; // String del texto
	TTF_Font* font; // Font del texto
	SDL_Color color; // Color del texto
	SDL_Texture* texture; // Textura del texto
	SDL_Rect rect; // Rect del texto (donde se renderiza el texto en pantalla)
	float size; // Escala del texto
} Text;

// Estructura de Fonts
typedef struct Font {
	char* path; // Path del font
	Uint8 size; // Tamaño del font (Escala fija)
} Font;

Font upheavalFont = {
	.path = "assets/fonts/upheaval.ttf",
	.size = 20
};

// Estructura de piezas
typedef struct Tetromino {
	char shape;
	Uint8 nShape;
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
	{'L', 0, 3, NULL,
	{{0,0,1,0} 
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// Z BLOCK
	{'Z', 1, 3, NULL,
	{{1,1,0,0}
	,{0,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// I BLOCK
	{'I', 2, 4, NULL,
	{{0,0,0,0}
	,{1,1,1,1}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// J BLOCK
	{'J', 3, 3, NULL,
	{{1,0,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// O BLOCK
	{'O', 4, 2, NULL,
	{{1,1,0,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X + 1, INICIAL_Y},
	// S BLOCK
	{'S', 5, 3, NULL,
	{{0,1,1,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y},
	// T BLOCK
	{'T', 6, 3, NULL,
	{{0,1,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}},
	INICIAL_X, INICIAL_Y}
};

//! Variables globales ======================================================

// Texturas globales
SDL_Texture *ghostTexture, *lockTexture;
SDL_Texture *gameboardInt, *gameboardExt;
SDL_Texture* backgrounds[4];

// Flags y contadores
Uint8 currBackground; // Indice de background actual
int deletedLines;
int lastRow, lastSize; // Propiedades ultima pieza dropeada

bool up, right, left, softD, hardD, fall, hold;  // Controles flags
Uint8 rotation; // Rotation flag

bool holded, firstHold; // Hold flags
bool firstDrop; // Drop flags
bool lock_delay; // Lock delay flag
Uint8 dropDelay; // Contador de delay hardD

bool running; // Flag loop game
bool restart; // Flag restart
bool gameOver; // Flag game over

// Variables de control de tiempo y frames
float FPS;
Uint64 countFrames; // Contador de frames
Uint64 start_time, current_time, capTimer, frame_time, lock_timer; // Tiempos

// Paths de assets
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

// Funcion que inicializa todo SDL y demas librerias usadas
bool initSDL(SDL_Window **ptrWindow, SDL_Renderer **ptrRenderer) { 
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
	*ptrWindow = SDL_CreateWindow("Intento de tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (*ptrWindow == NULL) {
		printf("Error creating window: %s\n", SDL_GetError());
		return 0;
	}
	SDL_Surface* icon = IMG_Load("assets/udec_icon.webp");
	if (icon == NULL) {
		printf("Error assigning window icon: %s\n", IMG_GetError());
	}
	SDL_SetWindowIcon(*ptrWindow, icon); // Poner icono a la ventana
	SDL_FreeSurface(icon);
	// Crear renderer
	*ptrRenderer = SDL_CreateRenderer(*ptrWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*ptrRenderer == NULL) {
		printf("Error creating renderer: %s\n", SDL_GetError());
		return 0;
	}
	SDL_SetRenderDrawColor(*ptrRenderer, 0, 0, 0, 255); // Color fondo (Negro)

	return 1;
}

void printPosTetromino(Tetromino *curr, Playfield *playfield) {
	printf("Pos actual: (%d ,%d), playfield=\"%c\"\n", curr->x, curr->y, playfield->matrix[curr->y][curr->x]);
}

// Imprimir grilla actual
void printPlayfield(Playfield *playfield) {
	int row = 0, column = 0;
	printf("La matriz de la grilla actual es:\n");
	for (int i = 0; i < BOARD_HEIGHT + 1; ++i) {
		if (i == 3) printf("   ---------------------------------------\n");
		if (i == BOARD_HEIGHT) printf("   ---------------------------------------\n");
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			if (j == 0 && i < BOARD_HEIGHT) printf("%2d | ", row++);
			if (i < BOARD_HEIGHT) printf("%2c ", playfield->matrix[i][j]);
			if (j == BOARD_WIDTH - 1 && i < BOARD_HEIGHT) printf("|\n");
			if (i == BOARD_HEIGHT) {
				if (j != 0) printf("%2d ", column++);
				else printf("     %2d ", column++);
			}
		}
	}
	printf("\n");
}

// Comprobar estado linea (-1 = borde, 0 = vacia, 1 = incompleta, 2 = completa)
int checkLineState(Playfield *playfield, Uint8 row) {
	int countBlocks = 0;
	for (int column = 1; column < BOARD_WIDTH - 1; column++) {
		if (playfield->matrix[row][column] == '#') return -1;
		if (playfield->matrix[row][column] != ' ') ++countBlocks;
	}
	if (countBlocks == 10) return 2;
	else if (countBlocks == 0) return 0;
	else return 1;
}

// Funcion que elimina lineas completas por la anterior pieza dropeada
int deleteLines(Playfield *playfield, Uint8 initialRow, Uint8 size) {
	int nLines = 0, rowInterval = initialRow + size;
	if (rowInterval > BOARD_HEIGHT - 2) rowInterval = BOARD_HEIGHT - 2;
	for (int row = initialRow; row <= rowInterval; ++row) {
		if (checkLineState(playfield, row) == 2) {
			for (int column = 1; column < BOARD_WIDTH - 1; column++) {
				playfield->matrix[row][column] = ' ';	
			}
			++nLines;
		}
	}
	return nLines;
}

// Funcion que revisa si el jugador perdio
bool gameOverCheck(Playfield *playfield, Tetromino *curr) {
	// TODO: MEJORAR DETECCION DE GAMEOVER
	bool loss = false;
	// Contar bloques de la pieza encima del tablero, si los 4 estan arriba pierdes
	int countBlocks = 0;
	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			int coordY = i + curr->y;
			if (curr->matrix[i][j] && coordY <= 2) ++countBlocks;
		}
	}
	switch (countBlocks) {
		case 3:
			if (!deleteLines(playfield, lastRow, lastSize)) loss = true;
			break;
		case 4:
			printf("GAME OVER!!!!\n");
			restart = true; // TODO: POR AHORA
			loss = true;
			break;
	}
	return loss;
}

// Funcion que inicializa objeto de la estructura Text
Text* initText(const char *str, Font *font, const SDL_Color color, const int x, const int y, const float size) {
	Text* text = malloc(sizeof(Text));
	*text = (Text) {	// Se castea a un dato tipo "Text", ya que como estamos inicializandolo desde un puntero tenemos que usar un literal compuesto (googlea "Compound literal")
		.string		= "", 									// String del texto (vacio por ahora)
		.font 		= TTF_OpenFont(font->path, font->size),	// Fuente (Cargada con ayuda de TTF_OpenFont("path del font", tamaño letra))
		.color 		= color,								// Color del texto
		.texture	= NULL,									// Textura (NULL ya que se crea y asigna posteriormente)
		.rect 		= {x, y, 0, 0},							// Rect del texto (Posicion/Tamaño), el tamaño se asigna posteriormente al crear la textura
		.size		= size
	};
	strcpy(text->string, str);	// Copiar parametro string en la string de estructura
	if (text->font == NULL) printf("ERROR INICIALIZANDO FONT: %s\n", TTF_GetError());
	return text;
}

// Reiniciar tablero
void initPlayfield(Playfield *playfield) {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (j == 0 || j == BOARD_WIDTH - 1) {
				playfield->matrix[i][j] = '#';
			} else if (i == BOARD_HEIGHT - 1) {
				playfield->matrix[i][j] = '#';
			} else {
				playfield->matrix[i][j] = ' ';
			}
		}
	}
	printPlayfield(playfield);
}

// Revisa colisiones de tetromino actual
bool collision(Playfield *playfield, Tetromino *curr) {
	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			if (curr->matrix[i][j] && playfield->matrix[i + curr->y][j + curr->x] != ' ') {
				return true;
			}
		}
	}
	return false;
}

// Generar nueva pieza pseudo aleatoria (FALTA AGREGAR 7-BAG)
void newTetromino(Playfield *playfield, Tetromino *curr, Tetromino *next) {
	*curr = *next;
	*next = tetrominoes[rand() % 7];
}

void dropTetromino(Playfield *playfield, Tetromino *curr, Tetromino *next) {
	if (!firstDrop) firstDrop = true;
	if (collision(playfield, curr)) curr->y--;

	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			if (curr->matrix[i][j]) {
				playfield->matrix[i + curr->y][j + curr->x] = curr->shape;
			}
		}
	}
	printPlayfield(playfield);
	lastRow = curr->y;
	lastSize = curr->size;
	printf("lastTetroRow: %d, lastTetroSize: %d!!!!!!!!!!\n", curr->y, curr->size);

	deletedLines = deleteLines(playfield, lastRow, lastSize); // TODO: EJECUTAR DSP DE DROPEAR 3 PIEZAS
	gameOverCheck(playfield, curr);

	newTetromino(playfield, curr, next);
}

// Funcion que rota piezas
void rotateTetromino(Tetromino *tetro, const Uint8 sense) {
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
			rotateTetromino(tetro, CLOCKWISE);
			break;
		default:
			break;
	}
}

// Funcion que se ejecuta cuando se dropea la pieza y esta pasa al stack (Se crea una pieza nueva)
void hardDropTetromino(Playfield *playfield, Tetromino *curr, Tetromino *next) {
	bool noCollision = false;
	while (!noCollision) {
		if (collision(playfield, curr)) {
			dropTetromino(playfield, curr, next);
			noCollision = true;
		} else curr->y++;
	}
	dropDelay = 60;
}

// TODO: MEJORAR SOFTDROP ****************************************************
void softDropTetromino(Playfield *playfield, Tetromino *curr, Tetromino *next) {
	curr->y++;
	if (collision(playfield, curr)) {
		dropTetromino(playfield, curr, next);
	}
}

// TODO: HACER VELOCIDAD DE CAIDA DEPENDIENTE DE DIFICULTAD ******************
bool fallSpeed(Uint64 *countFrames, Tetromino *curr) {
	// Fall (Cada 48 frames baja 1 celda)
	if (*countFrames % 48 == 0 && dropDelay == 0 && *countFrames > 48) {
		return true;
	} else if (dropDelay > 0) {
		dropDelay--;
	}
	return false;
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
									// rotation = COUNTER_CLOCKWISE; break;
				case SDLK_DOWN:		softD = true; break;
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
					if (!e.key.repeat) hardD = true;
					break;
				case SDLK_r:
					if (!e.key.repeat) restart = true;
					break;
				case SDLK_ESCAPE: running = false; break;
				default: break;
			}
		}
	}
}

// TODO: ORGANIZAR BIEN ***************************
// Game updater (revisa coliones antes de mover)
void gameUpdate(Playfield *playfield, Tetromino *curr, Tetromino *next, Tetromino *holder) {
	if (rotation) {
		rotateTetromino(curr, rotation);
		if (collision(playfield, curr)) {
			switch (rotation) {
				case CLOCKWISE: 		rotateTetromino(curr, COUNTER_CLOCKWISE); break;
				case COUNTER_CLOCKWISE: rotateTetromino(curr, CLOCKWISE); break;
				case DOUBLE_CLOCKWISE: 	rotateTetromino(curr, DOUBLE_CLOCKWISE); break;
			}
		}
		rotation = false;
	}
	if (hardD) {
		if (holded) holded = false;
		hardDropTetromino(playfield, curr, next);
		hardD = false;
	}
	if (softD) {
		// TODO: LOCK DELAY (MEDIO SEGUNDO EN CONTACTO CON SUELO ANTES DE CAER) **************************
		softDropTetromino(playfield, curr, next);
		softD = false;
	}
	if (hold) {
		if (!firstHold) {
			*holder = tetrominoes[curr->nShape];
			newTetromino(playfield, curr, next);
			firstHold = true;
			holded = true;
		} else {
			if (!holded) {
				int aux = curr->nShape;
				*curr = *holder;
				*holder = tetrominoes[aux];
				holded = true;
			}
		}
		hold = false;
	}
	// TODO: CREAR FUNCION ESPECIFICA PARA FALL
	if (fall) {
		curr->y++;
		if (collision(playfield, curr)) {
			curr->y--;
			if (!lock_delay) {
				lock_timer = countFrames + 30;
				lock_delay = true;
			} else {
				if (countFrames > lock_timer) {
					dropTetromino(playfield, curr, next);
				}
			}
		}
		fall = false;
	}
	if (right) {
		curr->x++;
		if (collision(playfield, curr)) curr->x--;
		right = false;
	}
	if (left) {
		curr->x--;
		if (collision(playfield, curr)) curr->x++;
		left = false;
	}
	if (restart) {
		if (currBackground < 3) ++currBackground;
		else currBackground = 0;
		initPlayfield(playfield);
		restart = false;
	}
}

// Funcion que actualiza tablero tras eliminar piezas //TODO: MEJORAR ALGORITMO DE ORDENAMIENTO DE PLAYFIELD
void playfieldUpdate(Playfield *playfield, int deletedLines) {
	printf("deletedLines inicial: %d\n", deletedLines);
	for (int princRow = BOARD_HEIGHT - 2; princRow > 0; princRow--) { //! MAAAAAL, ESTA RECORRIENDO TODO EL TABLERO, DEBERIA RECORRER HASTA QUE SE ACABE EL STACK NOMAIS
		if (checkLineState(playfield, princRow) == 0) {
			// deletedLines--;
			int secRow = princRow - 1;
			while (checkLineState(playfield, secRow) == 0) secRow--;
			for (int column = 1; column < BOARD_WIDTH - 1; column++) {
				playfield->matrix[princRow][column] = playfield->matrix[secRow][column];
				playfield->matrix[secRow][column] = ' ';
			}
			// printf("deletedLines actual: %d\n", deletedLines);
		}
	}
	// printf("deletedLines final: %d\n", deletedLines);
}

// Funcion que asigna texturas a tetrominos
void loadTetrominoesTexture(SDL_Renderer *renderer) {
	// Asignar texturas
	for (int shape = 0; shape < 9; shape++) {
		tetrominoes[shape].color = IMG_LoadTexture(renderer, blockPaths[shape]);
		if (shape == 7) ghostTexture = IMG_LoadTexture(renderer, blockPaths[shape]);
		if (shape == 8) lockTexture = IMG_LoadTexture(renderer, blockPaths[shape]);
	}
}

// Cargar fondos
void loadBackgroundsTexture(SDL_Renderer *renderer, SDL_Texture **backgrounds) {
	for (int i = 0; i < 4; i++) {
		backgrounds[i] = IMG_LoadTexture(renderer, backgroundsPath[i]);
	}
}

// Funcion que carga textura de texto
void loadTextTexture(SDL_Renderer *renderer, Text *text) {
	if (text->texture != NULL) SDL_DestroyTexture(text->texture); // Delete anterior textura
	SDL_Surface* textSurface = TTF_RenderText_Solid(text->font, text->string, text->color); //
	if (textSurface == NULL) {
		printf("Error al intentar crear textSurface: %s\n", TTF_GetError());
	} else {
		text->rect.w = textSurface->w * text->size;
		text->rect.h = textSurface->h * text->size;
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
	currRectNext = 0, currRectHolder = 0, nextX = 665, nextY = 200, holderX = 90, holderY = 200;
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
	if (firstHold) {
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
					if (holded) SDL_RenderCopy(renderer, lockTexture, NULL, &holder->rects[currRectHolder]);
					else SDL_RenderCopy(renderer, holder->color, NULL, &holder->rects[currRectHolder]);
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
				tetro->rects[currRect].w = tetro->rects[currRect].h = TILE_SIZE;
				// tetro->rects[currRect].w = tetro->rects[currRect].h = TILE_SIZE + 1;
				tetro->rects[currRect].x = ((tetro->x - 1 + j) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
				tetro->rects[currRect].y = ((tetro->y + i) * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
				SDL_RenderCopy(renderer, tetro->color, NULL, &tetro->rects[currRect]);
				currRect++;
			}
		}
	}
}

void renderGhostTetromino(SDL_Renderer *renderer, Playfield *playfield, Tetromino *curr) {
	bool noFitting = false;
	int currRect = 0;
	Tetromino currGhost = *curr;
	while (!noFitting) {
		if (collision(playfield, &currGhost)) {
			currGhost.y--;
			for(int i = 0; i < currGhost.size; i++) {
				for(int j = 0; j < currGhost.size; j++) {
					if(currGhost.matrix[i][j]) {
						currGhost.rects[currRect].w = currGhost.rects[currRect].h = TILE_SIZE;
						currGhost.rects[currRect].x = ((currGhost.x - 1 + j) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
						currGhost.rects[currRect].y = ((currGhost.y + i) * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
						SDL_RenderCopy(renderer, ghostTexture, NULL, &currGhost.rects[currRect]);
						currRect++;
					}
				}
			}
			noFitting = true;
		} else currGhost.y++;
	}
}

void renderPlayfield(SDL_Renderer *renderer, Playfield *playfield) {
	for (int y = BOARD_HEIGHT - 2; y >= 0; y--) {
		for (int x = 1; x < BOARD_WIDTH - 1; x++) {
			if (playfield->matrix[y][x] != ' ') {
				SDL_Rect tmpRect;
				int nColor;
				tmpRect.w = tmpRect.h = TILE_SIZE;
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
void QuitSDL (SDL_Window *window, SDL_Renderer *renderer) {
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

//! MAIN ====================================================================================================================================
int main(int argc, char *argv[]) {
	srand(time(NULL));

	SDL_Window* window;
	SDL_Renderer* renderer;
	if (!initSDL(&window, &renderer)) {
		printf("Error inicializando SDL: %s\n", SDL_GetError());
		SDL_Delay(3000);
		return -1;
	}

//! DECLARAR Y INICIALIZAR *******************************************************************************************************
	
	// Text* textFPS = NULL;
	Text* textIntruc = NULL;

	Playfield* playfield = malloc(sizeof(Playfield));
	initPlayfield(playfield);

	// textFPS = initText("FPS: ", &upheavalFont, (SDL_Color){255,255,255,200}, 10, 1, 1);
	textIntruc = initText("Press F to change background", &upheavalFont, (SDL_Color){255,255,255,200}, 0, 1, 1);

	loadBackgroundsTexture(renderer, backgrounds);
	loadTetrominoesTexture(renderer);
	gameboardExt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardExt.png");
	gameboardInt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardInt.png");
	loadTextTexture(renderer, textIntruc);
	textIntruc->rect.x = SCREEN_WIDTH - textIntruc->rect.w - 5;

	Tetromino curr = tetrominoes[rand()%7];
	Tetromino next = tetrominoes[rand()%7];
	Tetromino holder;


//! GAME LOOP *********************************************************************************************************************
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

	//! INPUT ======================================================================================
		gameInput(&curr, &next, playfield);

		fall = fallSpeed(&countFrames, &curr);
	//! LOGICA Y CAMBIOS ======================================================================================
		gameUpdate(playfield, &curr, &next, &holder);
		if (deletedLines > 0) playfieldUpdate(playfield, deletedLines);

		// TODO: CONTAR PUNTAJE
	//! RENDER ======================================================================================
		// Cargar texto de FPS actuales
		// if (countFrames > 0) {
		// 	snprintf(textFPS->string + 5, 6,"%.1f", FPS);
		// 	loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		// }

		SDL_RenderClear(renderer);
		// Fondo ================================================================
		renderBackground(renderer, backgrounds[currBackground], gameboardInt);
		renderNextHold(renderer, &next, &holder);
		// Texto ================================================================
		// if (countFrames > 0) renderText(renderer, textFPS);
		renderText(renderer, textIntruc);
		// Playfield ============================================================
		renderPlayfield(renderer, playfield);
		renderGhostTetromino(renderer, playfield, &curr);
		renderTetromino(renderer, &curr);
		// Interfaz superpuesta =================================================
		SDL_RenderCopy(renderer, gameboardExt, NULL, NULL);
        
		SDL_RenderPresent(renderer);

	//! CONTROL DE FRAMES Y TIEMPOS ======================================================================================
		++countFrames; // Contar frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame anterior
		if (frame_time < 1000 / 60) SDL_Delay(1000 / 60 - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS)
	}

//! CERRAR Y LIBERAR RECURSOS ****************************************************************************************************************************/
	freeText(textIntruc);
	// freeText(textFPS);
	QuitSDL(window, renderer);

	return 0;
}