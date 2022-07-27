#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constantes
#define SCREEN_WIDTH 870
#define SCREEN_HEIGHT 950
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000.5/SCREEN_FPS

// Estructuras de texto
typedef struct Texto {
	char string[100];
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;
	SDL_Rect rect;
} Text;

// Variables globales
SDL_Renderer* renderer;
SDL_Window* win;

// Funcion que inicializa todo SDL
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

// Funcion que carga textura de texto
void loadFontTexture(SDL_Renderer *renderer, Text *text) {
	SDL_Surface* textSurface = TTF_RenderText_Solid(text->font, text->string, text->color);	// Crear surface del texto
	if (textSurface == NULL) {
		printf("Error al intentar crear textSurface: %s\n", TTF_GetError());
	} else {
		text->rect.w = textSurface->w; // Copiar ancho del texto
		text->rect.h = textSurface->h; // Copiar alto del texto
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

int main(int argc, char const *argv[]) {
	
	InitSDL();
	// Arriba estan los parametros de initFont
	Text* textFPS = initFont("FPS: ", "assets/Font.ttf", 60, (SDL_Color){0, 255, 0, 255}, 10, 10); // Uso de un "Compound literal"

	// Frames y time
	float FPS;
	uint64_t countFrames = 0;
	uint64_t start_time, current_time, capTimer, frame_time;

	// Iniciar game loop
	bool running = true; // Flag para finalizar gameloop
	start_time = SDL_GetTicks64(); // Tiempo inicial de gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo inicial de frame actual
		SDL_Event event;
		// Administracion de eventos
		while (SDL_PollEvent(&event)) {
			// Presionar boton de cerrar
			if (event.type == SDL_QUIT) {
				printf("Total frames: %llu\n", countFrames);
				SDL_Delay(1000);
				running = false;
				break;
			}
			// Presionar ESCAPE
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				printf("Total frames: %llu\n", countFrames);
				SDL_Delay(1000);
				running = false;
				break;
			}
			if (!running) break;
		}
		// Crear string de FPS y textura
		// gcvt(FPS, 4, textFPS->string + 5); // gcvt convierte un float a string y lo copia en un puntero de tipo char (aqui lo copie en la posicion 5 del string)
		snprintf(textFPS->string + 5, 5, "%.1f", FPS);
		loadFontTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS

		SDL_RenderClear(renderer); // Limpiar pantalla del frame anterior
		/* ************* ACTUALIZAR TEXTURAS DEL FRAME ACTUAL EN EL RENDER ************* */
		renderFont(renderer, textFPS); // Funcion para renderizar texto (declarada mas arriba)
		/* ***************************************************************************** */
		SDL_RenderPresent(renderer); // Mostrar Frame actual en pantalla
		++countFrames; // Contar frames
		
		// Control de tiempo y frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame
		if (frame_time < SCREEN_TICKS_PER_FRAME) SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS) 
		printf("FPS: %.2f\n", FPS); // Mostrar fps en consola
	}
	
	freeFont(textFPS);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();
}
