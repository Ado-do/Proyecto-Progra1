#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <math.h>
//La ventana que estaremos renderizando
SDL_Window* gWindow = NULL;

//La ventana renderizzada
SDL_Renderer* gRenderer = NULL;

//La textura recurriente que utilizaremos
SDL_Texture* gTexture = NULL;

const int SCREEN_WIDTH = 600; 
const int SCREEN_HEIGHT = 690;



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
        	//Creamos un renderizado para la ventana
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "El renderizadono pudo crearse (u.u)! SDL Error: %s\n", SDL_GetError() );
                funciona = false;
            }
            else
            {
                //Iniciamos el renderizado de color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //cargamos una imagen png(?)
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image no pudo iniciarse!!! SDL_image Error: %s\n", IMG_GetError() );
                     funciona = false;
                }
            }
        }
    }

    return funciona;

}
void CerrarSDl(){
  //Free loaded image
    SDL_DestroyTexture( gTexture );
    gTexture = NULL;

    //Destroy window    
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

}
 

bool loadMedia()
{
    //flag
    bool success = true;

    //nada para cargar
    return success;
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
        if( !loadMedia() )
        {
            printf( "Fallamos en cargar la imagen UWU!\n" );
        }
        else
        {
            //Un flag en el Main
            bool salir = false;

            //Con esto utilizamos eventos 
            SDL_Event e;
            while(!salir){ // Esto se llama Main Loop o Game loop (Tengo entendido que es donde ocurre todo)
                // Eventos que tendremos en la cola
                while(SDL_PollEvent( &e ) != 0 ){
                        //Cerrará el programa si el usuario apreta X
                        if( e.type == SDL_QUIT )
                    {
                        salir = true;
                    }
      		  	//Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );
                //Render red filled quad
                //SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                //SDL_SetRenderDrawColor( gRenderer, 0xFF, 0, 0, 0xFF );        
                //SDL_RenderFillRect( gRenderer, &fillRect );
                //Render green outlined quad
                SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
                SDL_SetRenderDrawColor( gRenderer, 0, 0xFF, 0, 0xFF );
                // SDL_Rect outlineRect1 = { SCREEN_WIDTH / 5.8, SCREEN_HEIGHT / 5.8, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
                SDL_SetRenderDrawColor( gRenderer, 0, 0xFF, 0, 0xFF );            
                SDL_RenderDrawRect( gRenderer, &outlineRect );

                /*Draw blue horizontal line
                /SDL_SetRenderDrawColor( gRenderer, 0, 0, 0xFF, 0xFF );        
                /SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 4, SCREEN_WIDTH, SCREEN_HEIGHT / 4);
                /SDL_SetRenderDrawColor( gRenderer, 0, 0, 0xFF, 0xFF );        
                SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );
                SDL_SetRenderDrawColor( gRenderer, 0, 0, 0xFF, 0xFF );        
                SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 3, SCREEN_WIDTH, SCREEN_HEIGHT / 3 );
                SDL_SetRenderDrawColor( gRenderer, 0, 0, 0xFF, 0xFF );        
                SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
                */
            	}
            	/*Draw vertical line of yellow dots
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0, 0xFF );
                for( int i = 0; i < SCREEN_HEIGHT; i += 4 )
                {
                    SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
                }*/

                //Update screen
                SDL_RenderPresent( gRenderer );
            }

        	}
    	}
    	//LIberamos recursos y cerramos SDL.
    CerrarSDl();

    return 0;
	}