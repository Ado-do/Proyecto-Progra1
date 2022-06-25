#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
int main(int argc, char const *argv[])
{
	SDL_Window* window = NULL; // INICIAMOS EL VIDEO Y LA IMAGEN
	SDL_Surface* windowSurface = NULL;
	SDL_Surface* im1 = NULL;
	SDL_Surface* im2 = NULL;
	SDL_Surface* im3 = NULL;
	SDL_Surface* imCurrent = NULL;
	SDL_Surface* menu1= NULL;
	SDL_Surface* menu2= NULL;



	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Proyecto Tetris",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,496,574,SDL_WINDOW_SHOWN);
	windowSurface = SDL_GetWindowSurface(window);

	im1 = SDL_LoadBMP("Dibujo1.bmp");
	im2 = SDL_LoadBMP("Dibujo2.bmp");
	im3 = SDL_LoadBMP("Dibujo3.bmp");
	menu1 = SDL_LoadBMP("t1.bmp");
	menu2 = SDL_LoadBMP("t2.bmp");
	imCurrent = menu1;


	bool Corriendo = true;
	SDL_Event ev;

	while(Corriendo)
	{ int b=1,d=0;
		while(SDL_PollEvent(&ev) != 0)
		{
			if(ev.type == SDL_QUIT)Corriendo = false;

			else
			{	
				while(d<=5){
						imCurrent=menu2;
						SDL_BlitSurface(imCurrent, NULL,windowSurface, NULL);
    					SDL_UpdateWindowSurface(window);
    					SDL_Delay(200);
    					imCurrent=menu1;
						SDL_BlitSurface(imCurrent, NULL,windowSurface, NULL);
    					SDL_UpdateWindowSurface(window);
    					SDL_Delay(200);
    					d++;
    				}
					
				if(ev.key.keysym.sym ==SDLK_SPACE)
				{   for(int i = 0;i<=2;i++){
						imCurrent=im1;
						SDL_BlitSurface(imCurrent, NULL,windowSurface, NULL);
    					SDL_UpdateWindowSurface(window);
    					SDL_Delay(1000);
    					imCurrent=im2;
    					SDL_BlitSurface(imCurrent, NULL,windowSurface, NULL);
    					SDL_UpdateWindowSurface(window);
    					SDL_Delay(1000);
    					imCurrent=im3;
    					SDL_BlitSurface(imCurrent, NULL,windowSurface, NULL);
    					SDL_UpdateWindowSurface(window);
    					SDL_Delay(1000);
    				}

				}
			}
				
		}
				SDL_BlitSurface(imCurrent, NULL,windowSurface, NULL);
    			SDL_UpdateWindowSurface(window);
	}
	SDL_FreeSurface(im1);
	SDL_FreeSurface(im2);
	SDL_FreeSurface(im3);
	SDL_FreeSurface(menu1);
	SDL_FreeSurface(menu2);
	imCurrent = im1 = im2 = im3 = menu1 = menu2 = NULL;
	window = NULL;
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}