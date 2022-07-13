#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
// Variables globales


const int SCREEN_WIDTH = 496; // Con esto vemos el tamaño de la ventana
const int SCREEN_HEIGHT = 574; // Si queremos modificar toda la ventana solo cambiamos aqui los numeros

SDL_Window* gWindow = NULL; // La ventana en la que renderizaremos todo

SDL_Surface* gScreenSurface = NULL; // La superficie de la ventana.

SDL_Surface* gHelloWorld = NULL; // La imagen que colocaremos en la ventana
/*
Estas variables las usaremos en este test nomas, ya que al finalizar el proyecto, las declararemos siesmpre
en sus respectivos areas, ya que al tenerlas activadas relentizan el programa.
*/

//Inicia SDL y abre una imagen.
bool iniciar(){
	bool funciona = true;
	//Inicializamos sdl
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "No se pudo abrir SDL :(((( SDL_Error: %s\n", SDL_GetError() );
        funciona = false;
    }
    else
    {
        //Se crea la ventana
        gWindow = SDL_CreateWindow( "Ttttetris.exe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "La ventana no se pudo crear (>.<) SDL_Error: %s\n", SDL_GetError() );
            funciona = false;
        }
        else
        {
            //se crea la superficie de la ventana
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return funciona;

}
// Carga nuestra media
bool cargarMedia(){
	//La flag para ver si funciona todo
    bool funciona = true;

    //Cargamos la imagen
    gHelloWorld = SDL_LoadBMP( "t.bmp" );
    if( gHelloWorld == NULL )
    {
        printf( "No se pudo cargar la imagen (T.T) %s! SDL Error: %s\n", "t.bmp", SDL_GetError());
        funciona = false;
    }

    return funciona;

}
// Con esta ultima funcion cerraremos la ventana y Sdl
void CerrarSDl(){
  	//Libera la superficie 
    SDL_FreeSurface( gHelloWorld );
    gHelloWorld = NULL;

    //Destruye la ventana 
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Cierra el SDL
    SDL_Quit();


}
int main(int argc, char const *argv[])
{
	//Iniciamos SDL y creamos una ventana
    if( !iniciar() )
    {
        printf( "¿Que paso?, fallamos en iniciar!\n" );
    }
    else
    {
        //Load media
        if( !cargarMedia() )
        {
            printf( "Fallamos en cargar la imagen UWU!\n" );
        }
        else
        {
            //Aplicamos la imagen
            SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );
            //Actualizamos la imagen
            SDL_UpdateWindowSurface( gWindow );
            //esperamos 2 segundos
            SDL_Delay( 2000 );
        }
    }

    //LIberamos recursos y cerramos SDL.
    CerrarSDl();

    return 0;
}