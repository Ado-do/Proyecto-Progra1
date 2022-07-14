// Para funciones de inicializacion y apagado
#include <SDL2/SDL.h>
// Para renderizar imagenes y graficos en pantalla
#include <SDL2/SDL_image.h>
// Para usar funciones basadas en tiempo como SDL_Delay()
#include <SDL2/SDL_timer.h>

int main(int argc, char *argv[])
{
	// Retorna 0 o 1 dependiendo si se inicializo todo con exito
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	// Crear ventana
	SDL_Window* win = SDL_CreateWindow("Intento de tetris", 
										SDL_WINDOWPOS_CENTERED, 
										SDL_WINDOWPOS_CENTERED, 
										600, 1000, 0);
	 
	// Son "SDL_RendererFlags", estas se utilizan para crear renders con una determinada configuracion.
	// SDL_RENDERER_ACCELERATED es para crear un render que use aceleracion de hardware (usa GPU)
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;

	// Crea un render para dibujar nuestras imagenes
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	// Crea una superficie para cargar una imagen en la memoria principal
	SDL_Surface* surface;

	// IMG_Load es una funcion que pide un path de tu imagen para convertirla en superficie
	surface = IMG_Load("O_block0.webp");

	// Carga imagen en la memomoria del hardware grafico
	// Convierte la superficie a textura (CPU -> GPU)
	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

	// Libera la memoria ocupada por la superficie ya convertida
	SDL_FreeSurface(surface);

	// Nos deja controlar la posicion de nuestra imagen para poder moverla con nuestro teclado
	SDL_Rect dest;

	// Conectar nuestra textura con "dest" para controlar la posicion
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

	// Ajustar alto y ancho de nuestra imagen
	dest.w /= 3;
	dest.h /= 3;

	// Asignar posicion x inicial del objeto
	dest.x = (1000 - dest.w) / 2;

	// Asignar posicion x inicial del objeto
	dest.y = (1000 - dest.h) / 2;

	// Controlar ciclo de la animacion
	int close = 0;

	// Velicidad de la caja
	int speed = 600;

	// Bucle de animacion
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
				case SDL_SCANCODE_UP:
					dest.y -= speed / 30;
					break;
				case SDL_SCANCODE_A:
				case SDL_SCANCODE_LEFT:
					dest.x -= speed / 30;
					break;
				case SDL_SCANCODE_S:
				case SDL_SCANCODE_DOWN:
					dest.y += speed / 30;
					break;
				case SDL_SCANCODE_D:
				case SDL_SCANCODE_RIGHT:
					dest.x += speed / 30;
					break;
                case SDL_SCANCODE_ESCAPE:
                    close = 1;
				    break;
				default:
					break;
				}
			}
            if (close) break;
		}

		// Perimetro derecho
		if (dest.x + dest.w > 1000)
			dest.x = 1000 - dest.w;

		// Perimetro izquierdo
		if (dest.x < 0)
			dest.x = 0;

		// Perimetro inferior
		if (dest.y + dest.h > 1000)
			dest.y = 1000 - dest.h;

		// Perimetro superior
		if (dest.y < 0)
			dest.y = 0;

		// Limpiar pantalla
		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, tex, NULL, &dest);

		// Provoca de el "double buffers", para renderizado multiple
		SDL_RenderPresent(rend);

		// Calcular para los 60 fps
		SDL_Delay(1000 / 60);
	}

	// Destrozar textura
	SDL_DestroyTexture(tex);

	// Destrozar renderizador
	SDL_DestroyRenderer(rend);

	// Destrozar ventana
	SDL_DestroyWindow(win);

	// Cerrar SDL	
	SDL_Quit();

	return 0;
}
