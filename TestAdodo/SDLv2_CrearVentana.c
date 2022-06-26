#include <stdio.h>
#include <SDL2/SDL.h>

// Constantes de las Dimensiones
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *argv[]) {

    // La ventana a la que renderizaremos
    SDL_Window* window = NULL;

    // La superficie contenida por la ventana
    SDL_Surface* screenSurface = NULL;

    // Iniciualizar SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("No se pudo iniciar SDL! SDL_Error: %s\n", SDL_GetError());
		return 1;
    } else {
        // Crear ventana
        window = SDL_CreateWindow("Intento SDL", 
								SDL_WINDOWPOS_UNDEFINED, 
								SDL_WINDOWPOS_UNDEFINED, 
								SCREEN_WIDTH, SCREEN_HEIGHT, 
								SDL_WINDOW_SHOWN);
        if (!window) {
            printf("No se pudo crear la ventana! SDL_Error: %s\n", SDL_GetError());
        } else {
            // Generar la superficie de la ventana
            screenSurface = SDL_GetWindowSurface(window);
            // Rellenar la superficie de negro
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface -> format, 0xFF, 0xFF, 0xFF));
            // Actualizar la superficie
            SDL_UpdateWindowSurface(window);
            // Esperar 2 segundos
            SDL_Delay(2000);
        }
    }

    // Masacrar a la pobre ventanita
    SDL_DestroyWindow(window);
    // Cerrar subsistemas de SDL
    SDL_Quit();

    return 0;
}