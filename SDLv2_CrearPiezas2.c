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
#define TILE_SIZE 40.1
#define BOARD_X 5.86
#define BOARD_Y 1.84
#define INICIAL_X 3
#define INICIAL_Y 0
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000/SCREEN_FPS

// Variables globales
SDL_Renderer* renderer;
SDL_Window* window;
bool running; // Flag loop game
int droped; // Flag de drop
// Variables de control de tiempo y frames
float FPS;
uint64_t countFrames = 0; // Contador de frames
float start_time, current_time, capTimer, frame_time; // Tiempos

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
} Shape;

// Arreglo de tetrominos
Shape blocks[7] = { 
	// L BLOCK
	{{255,127,0}, // Color Naranjo
	{{0,0,1,0} 
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X + BOARD_X, INICIAL_Y + BOARD_Y, 3, 'L'}
	// Z BLOCK
	,{{255,0,0}, // Color Rojo
	{{1,1,0,0}
	,{0,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X + BOARD_X, INICIAL_Y + BOARD_Y, 3, 'Z'}
	// I BLOCK
	,{{0,255,255}, // Color Celeste
	{{1,1,1,1}
	,{0,0,0,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X + BOARD_X, INICIAL_Y + BOARD_Y, 4, 'I'}
	// J BLOCK
	,{{0,0,255}, // Color Azul
	{{1,0,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X + BOARD_X, INICIAL_Y + BOARD_Y, 3, 'J'}
	// O BLOCK
	,{{255,255,0}, // Color Amarillo
	{{1,1,0,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X + BOARD_X + 1, INICIAL_Y + BOARD_Y, 2, 'O'}
	// S BLOCK
	,{{0,255,0}, // Color Verde
	{{0,1,1,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X + BOARD_X, INICIAL_Y + BOARD_Y, 3, 'S'}
	// T BLOCK
	,{{128,0,128}, // Color Morado
	{{0,1,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X + BOARD_X, INICIAL_Y + BOARD_Y, 3, 'T'}};

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

// Funciones que seran remplazadas por "clockWise"
Shape reverseCols(Shape s) {
	Shape tmp = s;
	for(int i=0; i < s.size; i++) {
		for(int j=0; j < s.size/2; j++) {
			bool t = s.matrix[i][j];
			tmp.matrix[i][j] = s.matrix[i][s.size - j - 1];
			tmp.matrix[i][s.size - j - 1] = t;
		}
	}
	return tmp;
}
Shape transpose(Shape* s) {
	Shape tmp = *s;
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			tmp.matrix[i][j] = s->matrix[j][i];
		}
	}
	return tmp;
}
void rotation(Shape* s, int clockwise) {
	// *s = transpose(s);
	switch (clockwise) {
		case 1:
			*s = reverseCols(transpose(s));
			return;
		case 2:
			*s = transpose(s);
			return;
		case 3:
			return;
	}
}

// Funcion que rota piezas
void clockWise(Shape* s, bool sense) {
	// Transponer figura
	Shape copy = *s;
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			s->matrix[i][j] = copy.matrix[j][i];
		}
	}
	copy = *s;
	// Hacer wea rara para que rote
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size/2; j++) {
			bool t = copy.matrix[i][j];
			s->matrix[i][j] = copy.matrix[i][s->size - j - 1];
			s->matrix[i][s->size - j - 1] = t;
		}
	}
}

// Funcion que se ejecuta cuando se dropea la pieza y esta pasa al stack (Se crea una pieza nueva)
void drop(Shape* s) {
	*s = blocks[rand() % 7];
	droped = 50;
}

// Dibujar pieza
void drawPiece(Shape* s, SDL_Rect* rect, SDL_Renderer* renderer) {
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			if(s->matrix[i][j]) {
				rect->x = (s->x + j) * TILE_SIZE;
				rect->y = (s->y + i) * TILE_SIZE;
				SDL_SetRenderDrawColor(renderer, s->color.r, s->color.g, s->color.b, 255); // Escoger color para cuadrados
				SDL_RenderFillRect(renderer, rect); // Pintar Cuadrados
				SDL_SetRenderDrawColor(renderer, 219, 219, 219, 255); // Escoger color para contorno (Gris)
				// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Escoger color para contorno (Negro)
				SDL_RenderDrawRect(renderer, rect); // Pintar contorno
			}
		}
	}
	// IMPRIMIR MATRIZ ACTUAL DE PIEZA
	// printf("La matriz de \"%c\" en drawPiece es:\n", s->letter);
	// for (int i = 0; i < 4; ++i) {
	// 	printf("| ");
	// 	for (int j = 0; j < 4; ++j) {
	// 		printf("%d ", s->matrix[i][j]);
	// 	}
	// 	printf("|\n");
	// }
}

// Funcion que recibe el input del juego
void input(Shape* cur) {
	// up = down = left = right = rotate = drop = 0;
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT) running = false;
		switch(e.type) {
			// case SDL_KEYUP: //DOWN
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym) {
					case SDLK_LEFT:
						// left = 1;
						cur->x--;
						break;
					case SDLK_RIGHT:
						// right = 1;
						cur->x++;
						break;
					case SDLK_UP:
						// up = 1;
						cur->y--;
						break;
					case SDLK_DOWN:
						// down = 1;
						cur->y++;
						break;
					case SDLK_z:
						// rotate = 1;
						clockWise(cur, 1);
						break;
					case SDLK_x:
						rotation(cur, 1);
						break;
					case SDLK_a:
						clockWise(cur, 1);
						clockWise(cur, 1);
						break;
					case SDLK_SPACE:
						drop(cur);
						break;
					case SDLK_ESCAPE:
						running = false;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
}

// Funcion que inicializa objeto de la estructura Text
Text* initFont(char *str, char *font, int size, SDL_Color color, int x, int y) {
	Text* text = malloc(sizeof(Text));
	*text = (Text){	// Se castea a un dato tipo "Text", ya que como estamos inicializandolo desde un puntero tenemos que usar un literal compuesto (googlea "Compound literal")
		.string		= "", 									// String del texto (vacio por ahora)
		.font 		= TTF_OpenFont(font, size),				// Fuente (Cargada con ayuda de TTF_OpenFont("path del font", tamaño letra))
		.color 		= color,								// Color del texto
		.texture	= NULL,									// Textura (NULL ya que se crea y asigna posteriormente)
		.rect 		= {x, y, 0, 0}							// Rect del texto (Posicion/Tamaño), el tamaño se asigna posteriormente al crear la textura
	};
	strcpy(text->string, str);	// Copiar string recibido en string de estructura
	return text;
}

// Funcion que carga textura de texto
void loadFontTexture(SDL_Renderer *renderer, Text *text) {
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
void renderFont(SDL_Renderer *renderer, Text *text) {
	SDL_RenderCopy(renderer, text->texture, NULL, &text->rect);
}

// Funcion que libera texto junto a su textura y fuente cargada
void freeFont(Text *text) {
	SDL_DestroyTexture(text->texture);
	TTF_CloseFont(text->font);
	free(text);
}

// Renderizar texturas de fondo
void renderBackground(SDL_Renderer* renderer, SDL_Texture* background) {
	SDL_RenderCopy(renderer, background, NULL, NULL);
}

int main(int argc, char *argv[]) {
	InitSDL();

	srand(time(NULL));
	Shape cur = blocks[4];
	// Shape cur = blocks[rand() % 7];
	SDL_Rect rect;
	rect.w = TILE_SIZE;
	rect.h = TILE_SIZE;

	Text* textFPS = initFont("FPS: ", "assets/Font.ttf", 20, (SDL_Color){0, 255, 0, 255}, 10, 10);
	SDL_Texture* fondo = IMG_LoadTexture(renderer, "assets/Fondos/FondoTest.png"); // Cargar Fondo
	if (fondo == NULL) printf("Error al crear textura fondo: %s\n", SDL_GetError());

	// Iniciar gameloop
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop
	while(running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

		input(&cur);

		// SoftDrop (Cada 48 frames baja 1 celda)
		if (countFrames % 48 == 0 && droped == 0) {
			cur.y++;
		} else if (droped > 0) {
			droped--;
		}

		// Crear string de FPS y textura
		if (countFrames != 0) {
			snprintf(textFPS->string + 5, 5, "%.1f", FPS);
			loadFontTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		}

		SDL_RenderClear(renderer);
		/* ************* ACTUALIZAR TEXTURAS DEL FRAME ACTUAL EN EL RENDER ************* */
		renderBackground(renderer, fondo);
		renderFont(renderer, textFPS);
		drawPiece(&cur, &rect, renderer);
		/* ***************************************************************************** */
		SDL_RenderPresent(renderer);
		++countFrames; // Contar frames

		// Control de tiempo y frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame
		if (frame_time < SCREEN_TICKS_PER_FRAME) SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS) 
		// printf("FPS: %.2f\n", FPS); // Mostrar fps en consola
	}

	freeFont(textFPS);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}