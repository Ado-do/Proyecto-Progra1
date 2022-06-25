#include <stdio.h>
#include <SDL2/SDL.h>
int main(int argc, char const *argv[])
{
	SDL_Window* window = NULL; // INICIAMOS EL VIDEO Y LA IMAGEN
    SDL_Surface* screenSurface = NULL;
    SDL_Surface* imageSurface = NULL;
   	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) // Si la imagen no se inicia, nos dira el error
    {
        printf("SDL no pudo iniciar!! SDL_Error: %s\n", SDL_GetError());
    }else{// Creamos la ventana
    	window = SDL_CreateWindow("Proyecto Tetris",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,496,574,SDL_WINDOW_SHOWN);
    	if(window == NULL){
    		printf("La ventana no pudo haber sido creada SDL_Error: %s\n",SDL_GetError());
    	}else{
    		screenSurface = SDL_GetWindowSurface(window);
    		imageSurface = SDL_LoadBMP("Dibujo1.bmp");
    		if(imageSurface == NULL){
    			printf("Imagen Error; %s\n",SDL_GetError());
    		}else{
    			SDL_BlitSurface(imageSurface, NULL,screenSurface, NULL);
    			SDL_UpdateWindowSurface(window);
    			SDL_Delay(5000);// Delay de 5 segundos
    		}
    	}
    }
    imageSurface = NULL;
    window = NULL;
    screenSurface = NULL;
    SDL_DestroyWindow(window); // Liberamos espacio
    SDL_Quit(); // Cerramos sdl
	return 0;
}