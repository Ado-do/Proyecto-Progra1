#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
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

SDL_Texture* tex;
SDL_Renderer* renderer;
SDL_Texture* boton1;
SDL_Texture* boton2;
SDL_Texture* boton3;
SDL_Window* win;
SDL_Window* window;
SDL_Texture* fondo;
SDL_Texture* gameboard;
// Flags
bool running; // Flag loop game
int droped; // Flag de drop
// Variables de control de tiempo y frames
float FPS;
uint64_t countFrames = 0; // Contador de frames
double start_time, currrent_time, capTimer, frame_time; // Tiempos

int i = 0;

void InitSDL() {
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); // Calidad de escalado
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("Error inicializando SDL: %s\n", SDL_GetError()); // Inicializar toda la biblioteca de SDL
	if (TTF_Init() == -1) printf("Error al inicializar SDL_TTF: %s\n", SDL_GetError()); // Inicializar SDL_TTF
	// Crear ventana
	win = SDL_CreateWindow("Intento de tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Surface* icon = IMG_Load("assets/udec_icon.webp");
	if (icon == NULL) printf("Error al asignar icono: %s\n", SDL_GetError());
	SDL_SetWindowIcon(win, icon); // Poner icono a la ventana
	SDL_FreeSurface(icon);
	// Crear renderer
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Color fondo (Negro)
}
// Funcion que inicializa objeto de la estructura Text
Text* initFont(char *str, char *font, int size, SDL_Color color, int x, int y) {
	Text* text = malloc(sizeof(Text));
	*text = (Text){	// Se castea a un dato tipo "Text", ya que como estamos inicializandolo desde un puntero tenemos que usar un literal compuesto (googlea "Compound literal")
		.string		= "", 									// String del texto (vacio por ahora)
		.font 		= TTF_OpenFont(font, size),				// Fuente (Cargada con ayuda de TTF_OpenFont("path del font", tamaño letra))
		.color 		= color,								// Color del texto
		.texture 	= NULL,									// Textura (NULL ya que se crea y asigna posteriormente)
		.rect 		= {x, y, 0, 0}							// Rect del texto (Posicion/Tamaño), el tamaño se asigna posteriormente al crear la textura
	};
	strcpy(text->string, str);	// Copiar string recibido en string de estructura
	return text;
}
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
void freeFont(Text *text) {
	SDL_DestroyTexture(text->texture);
	TTF_CloseFont(text->font);
	free(text);
}

// Funcion que renderiza textura de texto
void renderFont(SDL_Renderer *renderer, Text *text) {
	SDL_RenderCopy(renderer, text->texture, NULL, &text->rect);
}
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
				case SDLK_ESCAPE:
					
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

void loadBlocksTexture() {
	blocks[0].texture = IMG_LoadTexture(renderer, "L.png");
	blocks[1].texture = IMG_LoadTexture(renderer, "Z.png");
	blocks[2].texture = IMG_LoadTexture(renderer, "I.png");
	blocks[3].texture = IMG_LoadTexture(renderer, "J.png");
	blocks[4].texture = IMG_LoadTexture(renderer, "O.png");
	blocks[5].texture = IMG_LoadTexture(renderer, "S.png");
	blocks[6].texture = IMG_LoadTexture(renderer, "T.png");
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

int main(int argc, char const *argv[])
{
	InitSDL();
	bool running = true;
	SDL_Event event;
	int eleccion=0;
	fondo= IMG_LoadTexture(renderer, "StartMenu.png");
	boton2= IMG_LoadTexture(renderer, "boton2.png");

	while(running){

		while (SDL_PollEvent(&event)) {

			if(event.key.keysym.scancode == SDL_SCANCODE_DOWN && eleccion <3)
			{
				eleccion++;
				printf("%d\n",eleccion);
			}
			else if(event.key.keysym.scancode == SDL_SCANCODE_UP && eleccion >0){
				eleccion--;
				printf("%d\n",eleccion);

			}
			switch(eleccion){
				case 0:
					//tex= IMG_LoadTexture(renderer, "StartMenu.png");
				break;
				case 1:
					//tex= IMG_LoadTexture(renderer, "StartMenu.png");
				break;
				case 3:
					//tex= IMG_LoadTexture(renderer, "StartMenu.png");
				break;

			}
			int t=event.key.keysym.scancode;
			while(t==SDL_SCANCODE_SPACE && SDL_PollEvent(&event)){
				bool wait=true;
				if(eleccion==0){printf("JUGANDO JUGANDO JUGANDO JUGANDO JUGANDO\n");
				srand(time(NULL));
	

			}
				else if(eleccion==2 || eleccion==1){
					printf("CONTROLES CONTROLES CONTROLES CONTROLES\n");
					tex= IMG_LoadTexture(renderer, "boton2.png");
					SDL_RenderClear(renderer);
					SDL_RenderCopy(renderer,tex,NULL, NULL);
					SDL_RenderPresent(renderer);
					while(wait){
						while(SDL_PollEvent(&event)){
								if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
									
									eleccion=0;
									wait=false;
									break;
							}
						}
					}
				}

				else if(eleccion==3){
					printf("SALIR SALIR SALIR SALIR\n");
					running=false;
				}
				
			}

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer,tex,NULL, NULL);
			SDL_RenderPresent(renderer);

	}

	
}	SDL_DestroyTexture(boton2);
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();
}
