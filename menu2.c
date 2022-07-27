#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define SCREEN_WIDTH 499
#define SCREEN_HEIGHT 545
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000.5/SCREEN_FPS
#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct Texto {
	char string[100];
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;
	SDL_Rect rect;
} Text;
SDL_Renderer* renderer;
SDL_Window* win;
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
}void loadFontTexture(SDL_Renderer *renderer, Text *text) {
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

int main(int argc, char const *argv[])
{
	InitSDL();
	bool running = true;
	SDL_Event event;
	int eleccion=0;
	SDL_Texture* texture = IMG_LoadTexture(renderer, "PLAY.png");
	while(running){

		while (SDL_PollEvent(&event)) {

			if(event.type == SDL_QUIT) {
				for (int i = 0; i < 5; ++i)
				{
					printf("SALIENDO!\n");
				}
				running=false;
				break;
			}
			else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN && eleccion <3)
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
					texture= IMG_LoadTexture(renderer, "PLAY.png");
				break;
				case 1:
					texture= IMG_LoadTexture(renderer, "HELP.png");
				break;
				case 3:
					texture= IMG_LoadTexture(renderer, "PLAY.png");
				break;

			}
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer,texture, NULL, NULL);
			SDL_RenderPresent(renderer);
	}
}
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();
}
