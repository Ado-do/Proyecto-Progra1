#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
// Variables globales

//Letras del teclado constantes 
enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

const int SCREEN_WIDTH = 700; // Con esto vemos el tamaño de la ventana
const int SCREEN_HEIGHT = 900; // Si queremos modificar toda la ventana solo cambiamos aqui los numeros

SDL_Window* gWindow = NULL; // La ventana en la que renderizaremos todo

SDL_Surface* gScreenSurface = NULL; // La superficie de la ventana.

//SDL_Surface* gImage = NULL; // La imagen que colocaremos en la ventana

SDL_Surface* gCurrentSurface = NULL; // La imagen actual

SDL_Surface* gKeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ]; // La imagen que corresponde a la tecla q apretamos


/*
Estas variables las usaremos en este test nomas, ya que al finalizar el proyecto, las declararemos siempre
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
SDL_Surface* CargarSuperficie( char * texto )
{
    //Load image at specified path
    SDL_Surface* superficieCargada = SDL_LoadBMP(texto);
    if( superficieCargada== NULL )
    {
        printf( "No se pudo cargar la imagen o.o! SDL Error: %s\n", SDL_GetError() );
    }

    return superficieCargada;
}
// Carga nuestra media
bool cargarMedia(){
	//La flag para ver si funciona todo
    bool funciona = true;

    //Cargamos la imagen
    //gImage = SDL_LoadBMP( "t.bmp" );
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = CargarSuperficie( "t.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == NULL )
    {
        printf( "Failed to load default image!\n" );
        funciona = false;
    }
    //if( gImage == NULL )
    //{
      //  printf( "No se pudo cargar la imagen (T.T) %s! SDL Error: %s\n", "t.bmp", SDL_GetError());
        //funciona = false;
    //} 
    // Cambia la imagen cuando apretas arriba
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = CargarSuperficie( "Dibujo1.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == NULL )
    {
        printf( "No se pudo cargar la imagen del boton arriba (T.T) \n" );
        funciona = false;
    }
    // Cambiar la imagen cuando apretas abajo
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] = CargarSuperficie( "Dibujo2.bmp");
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] == NULL )
    {
        printf( "No se pudo cargar la imagen del boton abajo (>.<)!\n" );
        funciona = false;
    }


    return funciona;

}
// Con esta ultima funcion cerraremos la ventana y Sdl
void CerrarSDl(){
  	//Libera la superficie 
    SDL_FreeSurface( gCurrentSurface );
    //gImage = NULL;
    gCurrentSurface= NULL;
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
            //Un flag en el Main
            bool salir = false;

            //Con esto utilizamos eventos 
            SDL_Event e;
            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
            while(!salir){ // Esto se llama Main Loop o Game loop (Tengo entendido que es donde ocurre todo)
                // Eventos que tendremos en la cola
                while(SDL_PollEvent( &e ) != 0 ){
                        //Cerrará el programa si el usuario apreta X
                        if( e.type == SDL_QUIT )
                    {
                        salir = true;
                    }
                    //El usuario apreta una tecla
                    else if( e.type == SDL_KEYDOWN )
                    {
                        //Que tecla apreta
                        switch( e.key.keysym.sym )
                        {
                            case SDLK_UP:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                            break;

                            case SDLK_DOWN:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                            break;

                            //case SDLK_LEFT:
                            //gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
                            //break;

                            //case SDLK_RIGHT:
                            //gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
                            //break;

                            default:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                            break;
                        }
                    }
                }//Apply the image
                SDL_BlitSurface( gCurrentSurface, NULL, gScreenSurface, NULL );
            
                //Update the surface
                SDL_UpdateWindowSurface( gWindow );
            }
        }
    }

    //LIberamos recursos y cerramos SDL.
    CerrarSDl();

    return 0;
}