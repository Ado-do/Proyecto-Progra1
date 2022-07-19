#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

// Constantes
#define WIDTH 600
#define HEIGHT 690
#define TILE_SIZE 26
#define INICIAL_X 9.5
#define INICIAL_Y 8.3

bool running;

int frameCount, timerFPS, lastFrame, fps;
// bool left, right, up, down, rotate, drop;

// Estructura de piezas
typedef struct Forma {
 SDL_Color color;
 bool matrix[4][4];
 double x, y;
 int size;
 char letra;
} shape;

// Arreglo de tetrominos
shape blocks[7] = { 
	// L BLOCK
	{{255,127,0}, // Color Naranjo
	{{0,0,1,0} 
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}}
	,INICIAL_X,INICIAL_Y,3,'L'}
	// Z BLOCK
	,{{255,0,0}, // Color Rojo
	{{1,1,0,0}
	,{0,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X,INICIAL_Y,3,'Z'}
	// I BLOCK
	,{{0,255,255}, // Color Celeste
	{{1,1,1,1}
	,{0,0,0,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X,INICIAL_Y,4,'I'}
	// J BLOCK
	,{{0,0,255}, // Color Azul
	{{1,0,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X,INICIAL_Y,3,'J'}
	// O BLOCK
	,{{255,255,0}, // Color Amarillo
	{{1,1,0,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X,INICIAL_Y,2,'O'}
	// S BLOCK
	,{{0,255,0}, // Color Verde
	{{0,1,1,0}
	,{1,1,0,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X,INICIAL_Y,3,'S'}
	// T BLOCK
	,{{128,0,128}, // Color Morado
	{{0,1,0,0}
	,{1,1,1,0}
	,{0,0,0,0}
	,{0,0,0,0}
	},INICIAL_X,INICIAL_Y,3,'T'}};

// Invierte columnas de laterales
shape reverseCols(shape s) {
    shape tmp = s;
    for(int i=0; i < s.size; i++) {
        for(int j=0; j < s.size/2; j++) {
            bool t = s.matrix[i][j];
            tmp.matrix[i][j] = s.matrix[i][s.size - j - 1];
            tmp.matrix[i][s.size - j - 1] = t;
        }
    }
    return tmp;
}
shape transpose(shape* s) {
	shape tmp = *s;
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			tmp.matrix[i][j] = s->matrix[j][i];
		}
	}
	return tmp;
}

void rotation(shape* s, int clockwise) {
	// *s = transpose(s);
	switch (clockwise) {
		case 1:
			*s = reverseCols(transpose(s));
			return;
		case 2:
			*s = transpose(s);
			return;
		case 3:
			return;
	}
}

void counterClockwise(shape* s, bool sense) {
	// Transponer figura
	shape copy = *s;
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			s->matrix[i][j] = copy.matrix[j][i];
			// copy.matrix[i][j] = s->matrix[j][i];
		}
	}
	copy = *s;
	// Hacer wea rara para que rote
    for(int i = 0; i < s->size; i++) {
        for(int j = 0; j < s->size/2; j++) {
            bool t = copy.matrix[i][j];
			s->matrix[i][j] = copy.matrix[i][s->size - j - 1];
			s->matrix[i][s->size - j - 1] = t;
        }
    }
}

void drop(shape* s) {
	*s = blocks[rand() % 7];
	// // Numero de pieza aleatoria
	// int randomBlock = rand() % 7; 
	// // Asignar color
	// s->color = blocks[randomBlock].color;
	// // Asignar forma
	// for (int i = 0; i < 4; i++) {
	// 	for (int j = 0; j < 4; j++) {
	// 		s->matrix[i][j] = blocks[randomBlock].matrix[i][j];
	// 	}
	// }
	// // Asignar tamaño
	// s->size = blocks[randomBlock].size;
	// // Copiar letra
	// s->letra = blocks[randomBlock].letra;
}

void draw(shape* s, SDL_Rect* rect, SDL_Renderer* renderer) {
	for(int i = 0; i < s->size; i++) {
		for(int j = 0; j < s->size; j++) {
			if(s->matrix[i][j]) {
				rect->x = (s->x + j) * TILE_SIZE;
				rect->y = (s->y + i) * TILE_SIZE;
				SDL_SetRenderDrawColor(renderer, s->color.r, s->color.g, s->color.b, 255); // Escoger color para cuadrados
				SDL_RenderFillRect(renderer, rect); // Pintar Cuadrados
				SDL_SetRenderDrawColor(renderer, 219, 219, 219, 255); // Escoger color para contorno (Gris)
				// SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Escoger color para contorno (Blanco)
				SDL_RenderDrawRect(renderer, rect); // Pintar contorno
			}
		}
	}
	printf("La matriz de \"%c\" en DRAW es:\n", s->letra);
	for (int i = 0; i < 4; ++i) {
		printf("| ");
		for (int j = 0; j < 4; ++j) {
			printf("%d ", s->matrix[i][j]);
		}
		printf("|\n");
	}
}

void input(shape* cur) {
	// up = down = left = right = rotate = drop = 0;
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		if(e.type == SDL_QUIT) running = false;
		switch(e.type) {
			// case SDL_KEYUP: //DOWN
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym) {
					case SDLK_LEFT:
						// left = 1;
						cur->x--;
						break;
					case SDLK_RIGHT:
						// right = 1;
						cur->x++;
						break;
					case SDLK_UP:
						// up = 1;
						cur->y--;
						break;
					case SDLK_DOWN:
						// down = 1;
						cur->y++;
						break;
					case SDLK_z:
						// rotate = 1;
						counterClockwise(cur, 1);
						break;
					case SDLK_x:
						rotation(cur, 1);
						break;
					case SDLK_a:
						counterClockwise(cur, 1);
						counterClockwise(cur, 1);
						break;
					case SDLK_SPACE:
						drop(cur);
						// drop = 1;
						break;
					case SDLK_ESCAPE:
						running = false;
						break;
					default:
						break;
				}
		}
	}
}

void render(SDL_Renderer* renderer, SDL_Texture* fondo) {
	// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, fondo, NULL, NULL);

	frameCount++;
	int timerFPS = SDL_GetTicks() - lastFrame;
	if (timerFPS < (1000/60)) {
		SDL_Delay((1000/60) - timerFPS);
	}
}

int main(int argc, char *argv[]) {
	srand(time(NULL));

	shape cur = blocks[rand() % 7];

	SDL_Rect rect;
	rect.w = rect.h = TILE_SIZE;

	running = 1;
	static int lastTime = 0;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("Error inicializando SDL: %s\n", SDL_GetError());
	}

	SDL_Renderer* renderer;
	SDL_Window* window;
	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) != 0) {
		printf("Error al intentar SDL_CreateWindowAndRenderer(): %s\n", SDL_GetError());
	}	
	SDL_SetWindowTitle(window, "Intento de Tetris");
	SDL_Surface* icon = IMG_Load("udec_icon.webp");
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

    SDL_Texture* fondo = IMG_LoadTexture(renderer, "assets/Fondo.png");

    printf("Si es que hay algun error, es: %s\n", SDL_GetError());

	while(running) {
		lastFrame = SDL_GetTicks();
		if(lastFrame >= (lastTime + 1000)) {
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}

		// update(&cur);
		input(&cur);
		render(renderer, fondo);

		draw(&cur, &rect, renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}