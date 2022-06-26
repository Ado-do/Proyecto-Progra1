#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

// Constantes de las Dimensiones
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *argv[]) {

    // Iniciualizar SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("No se pudo iniciar SDL! SDL_Error: %s\n", SDL_GetError());
		return 1;
    }
    // Crear ventana
    // SDL_Windows es una estrucura que guarda toda la inforamcion de la propia ventana (tamaño, posicion, fullscreen, borders, etc)
    SDL_Window* window = SDL_CreateWindow("Intento SDL", 
                                            SDL_WINDOWPOS_CENTERED, 
                                            SDL_WINDOWPOS_CENTERED, 
                                            SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        printf("No se pudo crear la ventana! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // Crear el render (cargar programa en la GPU), lo cual se hara cargo de "dibujar" el contenido de la ventana
    //  SDL_Renderer es una estructura que maneja todo el renderizado, es la que "dibuja" en nuestra ventana
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, render_flags);
    if (!rend) {
        printf("Error creando rendere! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    } 
    // Cargar imagen en la memoria utilizando una funcionn de la libreria "SDL_image"
    // SDL_Surface es una estructura que representa los datos de la imagen en la memoria (RAM)
    // Para "superficies" se utiliza la CPU para renderizarlas (software rendering)
    // Se pueden renderizar con un render tipo "SDL_CreateSoftwareRenderer"
    SDL_Surface* surface = IMG_Load("black.png");
    if (!surface) {
        printf("Error creando surface! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Antes de que podamos "dibujar" la imagen en la ventana necesitamos crear una "Textura"
    // Una "Textura" corresponde a los datos de la imagen cargados en la memoria de la GPU (VRAM)
    // Para "texturas" se utiliza la GPU para renderizarlas (hardware rendering),
    // Utilizaremos el "render" antes creado para renderizar texturas 
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface); // Tras crear la texutra liberarenos "surface"
    if (!texture) {
        printf("Error creando textura! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Limpiar la ventana (La limpia de lo que se veia en el codigo SDLv2)
    // (Si no la limpiamos veriamos una copia de lo que teniamos en patalla sobre la ventana)
    SDL_RenderClear(rend);
    // Dibujar la imagen en la ventana
    SDL_RenderCopy(rend, texture, NULL, NULL); // Copia una textura en el render
    SDL_RenderPresent(rend);    // En resumen hace mas smooth nuestra animacion (tiene que ver con el "double buffer")
    // Esperar 5 segundos
    SDL_Delay(5000);

    // Limpiar "resources" antes de salir
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    // Masacrar a la pobre ventanita
    SDL_DestroyWindow(window);
    // Cerrar subsistemas de SDL
    SDL_Quit();

    return 0;
}