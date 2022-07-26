#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
// Para funciones de inicializacion y apagado
#include <SDL2/SDL.h>
// Para renderizar imagenes y graficos en pantalla
#include <SDL2/SDL_image.h>
// Para usar funciones basadas en tiempo como SDL_Delay()
#include <SDL2/SDL_timer.h>
// Para renderizar texto
#include <SDL2/SDL_ttf.h>

// Constantes
#define SCREEN_WIDTH 870
#define SCREEN_HEIGHT 950
#define BLOCK_LEN 73
#define BLOCK_MV 36
#define DROP_TIMING 48
const int SCREEN_FPS = 62;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("Error inicializando SDL: %s\n", SDL_GetError());
	
	SDL_Window* win = SDL_CreateWindow("Intento de tetris", 
										SDL_WINDOWPOS_CENTERED, 
										SDL_WINDOWPOS_CENTERED, 
										SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	// Poner icono a la ventana
	SDL_Surface* icon = IMG_Load("assets/udec_icon.webp");
	if (icon == NULL) printf("Error al asignar icono: %s\n", SDL_GetError());
	SDL_SetWindowIcon(win, icon);
	SDL_FreeSurface(icon);
	 
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	// Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    SDL_Texture* fondo = IMG_LoadTexture(rend, "assets/Fondo.png");
	SDL_Texture* bloque = IMG_LoadTexture(rend, "assets/block.webp");
    if (bloque == NULL || fondo == NULL) printf("Error al crear texturas: %s\n", SDL_GetError());


	SDL_Rect dest;
	SDL_QueryTexture(bloque, NULL, NULL, &dest.w, &dest.h);

	// Tamaño de cada 4 casillas
	dest.w = BLOCK_LEN + 6;
	dest.h = BLOCK_LEN;
	printf("dest.w inicial: %d\n", dest.w);
	printf("dest.h inicial: %d\n", dest.h);

	// Asignar posicion x inicial del objeto
	// dest.x = (SCREEN_WIDTH - dest.w) / 2;
	dest.x = 393;

	// Asignar posicion y inicial del objeto
	// dest.y = (SCREEN_HEIGHT - dest.h) / 2;
	dest.y = 189;

	// Controlar loop game
	bool close = 0;

	// Frames y time
	float FPS;
	uint64_t countFrames = 0;
	uint64_t start_time, current_time, capTimer, frame_time;
	start_time = SDL_GetTicks64();

	while (!close) {
		capTimer = SDL_GetTicks64();
		SDL_Event event;
		// Administracion de eventos
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					// Manejo del boton de cerrar
					printf("Total frames: %d\n", countFrames);
					SDL_Delay(2000);
					close = 1;
					break;
				case SDL_KEYDOWN:
					// API de teclado para teclas presionadas
					switch (event.key.keysym.scancode) {
						case SDL_SCANCODE_W:
							// softdrop = 0;
							dest.y -= 1;
							printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_UP:
							// softdrop = 0;
							dest.y -= BLOCK_MV;
							// printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_A:
							// softdrop = 0;
							dest.x -= 1;
							printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_LEFT:
							// softdrop = 0;
							dest.x -= BLOCK_MV + 2;
							// printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_S:
							// softdrop = 0;
							dest.y += 1;
							printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_DOWN:
							// softdrop = 0;
							dest.y += BLOCK_MV;
							// printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_D:
							// softdrop = 0;
							dest.x += 1;
							printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_RIGHT:
							// softdrop = 0;
							dest.x += BLOCK_MV + 2;
							// printf("x: %d, y: %d\n", dest.x, dest.y);
							break;
						case SDL_SCANCODE_ESCAPE:
							// softdrop = 0;
							close = 1;
							printf("Total frames: %d\n", countFrames);
							SDL_Delay(2000);
							break;
						default:
							break;
					}
			}
			// printf("Poll Event: %d\n", SDL_PollEvent(&event));
            if (close) break;
		}

		if (countFrames % 48 == 0) dest.y += BLOCK_MV;

		// Perimetro derecho
		if (dest.x > 544) {
			dest.x = 544;
			printf("BLOCKEAO. dest.x: %d\n", dest.x);
		}
		// Perimetro izquierdo
		if (dest.x < 240) {
			dest.x = 240;
			printf("BLOCKEAO. dest.x: %d\n", dest.x);
		}
		// Perimetro inferior
		if (dest.y > 689) {
			dest.y = 689;
			printf("BLOCKEAO. dest.y: %d\n", dest.y);
		}
		// Perimetro superior
		if (dest.y < 189) {
			dest.y = 189;
			printf("BLOCKEAO. dest.y: %d\n", dest.y);
		}
		// Limpiar pantalla
		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, fondo, NULL, NULL);
		SDL_RenderCopy(rend, bloque, NULL, &dest);

		// Provoca de el "double buffers", para renderizado multiple
		SDL_RenderPresent(rend);
		
		++countFrames; // Contar frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame
		if (frame_time < SCREEN_TICKS_PER_FRAME) SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time); 
		// Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		// FPS
		FPS = countFrames / (current_time / 1000.f); // Frames divididos segundos
		printf("FPS: %.2f\n", FPS);
	}

	// Destrozar textura
	SDL_DestroyTexture(bloque);
    SDL_DestroyTexture(fondo);

	// Destrozar renderizador
	SDL_DestroyRenderer(rend);

	// Destrozar ventana
	SDL_DestroyWindow(win);

	// Cerrar SDL	
	SDL_Quit();

	return 0;
}
