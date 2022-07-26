#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Constantes
#define SCREEN_WIDTH 870
#define SCREEN_HEIGHT 950
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000/SCREEN_FPS

// Estructuras de texto
typedef struct Texto {
	char str[20];
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;
    SDL_Rect rect;
} Text;

int main(int argc, char const *argv[]) {
    
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); // Calidad de escalado
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("Error inicializando SDL: %s\n", SDL_GetError());
    if (TTF_Init() == -1) printf("Error al inicializar SDL_TTF: %s\n", SDL_GetError());
	SDL_Window* win = SDL_CreateWindow("Intento de tetris", 
										SDL_WINDOWPOS_CENTERED, 
										SDL_WINDOWPOS_CENTERED, 
										SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	// Poner icono a la ventana
	SDL_Surface* icon = IMG_Load("assets/udec_icon.webp");
	if (icon == NULL) printf("Error al asignar icono: %s\n", SDL_GetError());
	SDL_SetWindowIcon(win, icon);
	SDL_FreeSurface(icon);
    SDL_Renderer* render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

    Text textFPS = {"FPS: 60", TTF_OpenFont("assets/Font.ttf", 20), {0,255,0,255}, NULL, {10, 10, 0, 0}};

    SDL_Surface* textSurface = TTF_RenderText_Solid(textFPS.font, textFPS.str, textFPS.color);
	if (textSurface == NULL) {
        printf("Error al intentar crear textSurface: %s\n", TTF_GetError());
    } else {
        textFPS.rect.w = textSurface->w;
        textFPS.rect.h = textSurface->h;
    }
	textFPS.texture = SDL_CreateTextureFromSurface(render, textSurface); 
	if (textFPS.texture == NULL) printf("Error al intentar crear textTexture: %s\n", SDL_GetError());
	SDL_FreeSurface(textSurface);

    // Frames y time
	float FPS;
	uint64_t countFrames = 0;
	uint64_t start_time, current_time, capTimer, frame_time;

    bool running = true;
	start_time = SDL_GetTicks64();
    while (running) {
        capTimer = SDL_GetTicks64();
		SDL_Event event;
		// Administracion de eventos
		while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Manejo del boton de cerrar
                printf("Total frames: %lld\n", countFrames);
                SDL_Delay(2000);
                running = false;
                break;
            } 
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                printf("Total frames: %lld\n", countFrames);
                SDL_Delay(2000);
                running = false;
				break;
            }
            if (!running) break;
		}
		SDL_RenderClear(render);
        SDL_RenderCopy(render, textFPS.texture, NULL, &textFPS.rect);
		SDL_RenderPresent(render);
		
		++countFrames; // Contar frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame
		if (frame_time < SCREEN_TICKS_PER_FRAME) SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time); 
		// Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		// FPS
		FPS = countFrames / (current_time / 1000.f); // Frames divididos segundos
		printf("FPS: %.2f\n", FPS);
    }
    
    
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
