#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include "Tetris_functions.h"

//! MAIN ====================================================================================================================================
int main(int argc, char *argv[]) {
	srand(time(NULL));

	if (!initSDL(&window, &renderer)) {
		printf("Error inicializando SDL: %s\n", SDL_GetError());
		SDL_Delay(3000);
		return -1;
	}
//! DECLARAR Y INICIALIZAR *******************************************************************************************************

	textFPS = initText("FPS: ", &upheavalFont, (SDL_Color){255,255,255,200}, 10, 1, 1);
	textIntruc = initText("Press R to restart game", &upheavalFont, (SDL_Color){255,255,255,200}, 0, 1, 1);
	textLevel = initText("Level: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 50, 700, 1.3);
	textScore = initText("Score: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 50, 730, 1.3);
	textLines = initText("Lines: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 50, 760, 1.3);
	textRecord = initText("New Record! Score: ", &upheavalFont, (SDL_Color){255,255,255,255}, 280, 600, 1.5);

	loadBackgroundsTexture(renderer, backgrounds);
	loadGameOverTexture(renderer, gameOverTextures);

	loadTetrominoesTexture(renderer);
	gameboardExt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardExt.png");
	gameboardInt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardInt.png");
	loadTextTexture(renderer, textIntruc);
	textIntruc->rect.x = SCREEN_WIDTH - textIntruc->rect.w - 5;

	loadTextTexture(renderer, textLevel);
	loadTextTexture(renderer, textScore);
	loadTextTexture(renderer, textLines);
	loadTextTexture(renderer, textRecord);

	initPlayfield(&playfield);

	level = 1;
	difficulty = 60; // Caida cada 60 frames (lvl 1)

	Tetromino curr = tetrominoes[rand()%7];
	Tetromino next = tetrominoes[rand()%7];
	Tetromino holder;

//! GAME LOOP *********************************************************************************************************************
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

		tetrisLoop(&playfield, &curr, &next, &holder);

	//! CONTROL DE FRAMES Y TIEMPOS ======================================================================================
		countFrames++; // Contar frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame anterior
		if (frame_time < 1000 / 60) SDL_Delay(1000 / 60 - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS)
	}

//! CERRAR Y LIBERAR RECURSOS ****************************************************************************************************************************/
	
	freeText(textLines);
	freeText(textScore);
	freeText(textLevel);
	freeText(textIntruc);
	freeText(textFPS);
	QuitSDL(window, renderer);

	return 0;
}
// int main(int argc, char const *argv[]) {
    // Pseudo codigo

    /*
    InitSDL();  // Inicializar SDL
    game Tetris;    // Inicializar Tetris
    
    InitBackground(&game);  // Cargar texturas del background
    while (game.running) {  // Gameloop
        game.HandleEvents();    // Manejo de eventos e input

        game.Update();  // Logica del juego

        game.Render();  // Mostrar cambios en pantalla
    }

    game.Release();
    */
//    return 0;
// }