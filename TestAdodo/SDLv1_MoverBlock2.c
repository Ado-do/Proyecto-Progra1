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

// Constantes de las Dimensiones
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 690
#define BLOCK_LEN 53
#define BLOCK_MV 26
// const int SCREEN_WIDTH = 600;
// const int SCREEN_HEIGHT = 690;

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("Error inicializando SDL: %s\n", SDL_GetError());
	
	SDL_Window* win = SDL_CreateWindow("Intento de tetris", 
										SDL_WINDOWPOS_CENTERED, 
										SDL_WINDOWPOS_CENTERED, 
										SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	// Poner icono a la ventana
	SDL_Surface* icon = IMG_Load("udec_icon.webp");
	SDL_SetWindowIcon(win, icon);
	SDL_FreeSurface(icon);
	 
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	// SDL_Surface* bloque = IMG_Load("assets/block.webp");
	// SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, bloque);

    SDL_Texture* fondo = IMG_LoadTexture(rend, "assets/Fondo.png");
	SDL_Texture* bloque = IMG_LoadTexture(rend, "assets/block.webp");

    if (bloque == NULL || fondo == NULL) {
        printf("Error al crear texturas: %s\n", SDL_GetError());
    }

	// SDL_FreeSurface(bloque);

	SDL_Rect dest;
	SDL_QueryTexture(bloque, NULL, NULL, &dest.w, &dest.h);

	// Tamaño de cada 4 casillas
	dest.w = BLOCK_LEN + 2;
	dest.h = BLOCK_LEN;
	printf("dest.w inicial: %d\n", dest.w);
	printf("dest.h inicial: %d\n", dest.h);

	// Asignar posicion x inicial del objeto
	// dest.x = (SCREEN_WIDTH - dest.w) / 2;
	dest.x = 271;
	printf("dest.x inicial: %d\n", dest.x);

	// Asignar posicion y inicial del objeto
	// dest.y = (SCREEN_HEIGHT - dest.h) / 2;
	dest.y = 344;
	printf("dest.y inicial: %d\n", dest.y);

	// Controlar loop game
	bool close = 0;
	// bool softdrop = 0;

	// Variable que cuenta el tiempo de ejecucion
	// clock_t a;

	while (!close) {
		SDL_Event event;
		// Administracion de eventos
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					// Manejo del boton de cerrar
					close = 1;
					break;
				case SDL_KEYDOWN:
					// API de teclado para teclas presionadas
					switch (event.key.keysym.scancode) {
						case SDL_SCANCODE_W:
							// softdrop = 0;
							dest.y -= 1;
							printf("dest.y: %d\n", dest.y);
							break;
						case SDL_SCANCODE_UP:
							// softdrop = 0;
							dest.y -= BLOCK_MV;
							break;
						case SDL_SCANCODE_A:
							// softdrop = 0;
							dest.x -= 1;
							printf("dest.x: %d\n", dest.x);
							break;
						case SDL_SCANCODE_LEFT:
							// softdrop = 0;
							dest.x -= BLOCK_MV;
							break;
						case SDL_SCANCODE_S:
							// softdrop = 0;
							dest.y += 1;
							printf("dest.y: %d\n", dest.y);
							break;
						case SDL_SCANCODE_DOWN:
							// softdrop = 0;
							dest.y += BLOCK_MV;
							printf("dest.y: %d\n", dest.y);
							break;
						case SDL_SCANCODE_D:
							// softdrop = 0;
							dest.x += 1;
							printf("dest.x: %d\n", dest.x);
							break;
						case SDL_SCANCODE_RIGHT:
							// softdrop = 0;
							dest.x += BLOCK_MV;
							break;
						case SDL_SCANCODE_ESCAPE:
							// softdrop = 0;
							close = 1;
							break;
						default:
							break;
					}
			}
			// printf("Poll Event: %d\n", SDL_PollEvent(&event));
            if (close) break;
			// if(!softdrop) {
				// 	dest.y += BLOCK_MV;
				// softdrop = 1;
			// }
		}

		// SDL_Delay(1000);
		// dest.y += 68;
		// printf("dest.y por caida: %d\n", dest.y);

		// Perimetro derecho
		if (dest.x > 375) {
			dest.x = 375;
			printf("BLOCKEAO. dest.x: %d\n", dest.x);
		}
		// Perimetro izquierdo
		if (dest.x < 165) {
			dest.x = 165;
			printf("BLOCKEAO. dest.x: %d\n", dest.x);
		}
		// Perimetro inferior
		if (dest.y > 500) {
			dest.y = 500;
			printf("BLOCKEAO. dest.y: %d\n", dest.y);
		}
		// Perimetro superior
		if (dest.y < 137) {
			dest.y = 137;
			printf("BLOCKEAO. dest.y: %d\n", dest.y);
		}
		// Limpiar pantalla
		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, fondo, NULL, NULL);
		SDL_RenderCopy(rend, bloque, NULL, &dest);

		// Provoca de el "double buffers", para renderizado multiple
		SDL_RenderPresent(rend);
		
		// Calcular para los 60 fps
		SDL_Delay(1000 / 60);
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
