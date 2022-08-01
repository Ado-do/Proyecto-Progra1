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

	initPlayfield(&playfield);

	textFPS = initText("FPS: ", &upheavalFont, (SDL_Color){255,255,255,200}, 10, 1, 1);
	textIntruc = initText("Press R to restart game", &upheavalFont, (SDL_Color){255,255,255,200}, 0, 1, 1);

	loadBackgroundsTexture(renderer, backgrounds);
	loadTetrominoesTexture(renderer);
	gameboardExt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardExt.png");
	gameboardInt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardInt.png");
	loadTextTexture(renderer, textIntruc);
	textIntruc->rect.x = SCREEN_WIDTH - textIntruc->rect.w - 5;

	Tetromino curr = tetrominoes[rand()%7];
	Tetromino next = tetrominoes[rand()%7];
	Tetromino holder;

//! GAME LOOP *********************************************************************************************************************
	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

	//! INPUT ======================================================================================
		gameInput(&curr, &next, &playfield);

	//! LOGICA Y CAMBIOS ======================================================================================

		if(checkFallTime(countFrames)) fall = true;
		gameUpdate(&playfield, &curr, &next, &holder);
		if (droped) {
			updatePlayfield(&playfield, &curr, &next);
			droped = false;
		}

		//TODO: CONTAR PUNTAJE
	//! RENDER ======================================================================================
		// Cargar texto de FPS actuales
		if (countFrames > 0) {
			snprintf(textFPS->string + 5, 6,"%.1f", FPS);
			loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
		}

		SDL_RenderClear(renderer);
		// Fondo ================================================================
		renderBackground(renderer, backgrounds[currBackground], gameboardInt);
		renderNextHold(renderer, &next, &holder);

		// Texto ================================================================
		if (countFrames > 0) renderText(renderer, textFPS);
		renderText(renderer, textIntruc);

		// Playfield ============================================================
		renderPlayfield(renderer, &playfield);
		renderGhostTetromino(renderer, &playfield, &curr);
		renderTetromino(renderer, &curr);

		// Interfaz superpuesta =================================================
		SDL_RenderCopy(renderer, gameboardExt, NULL, NULL);
        
		SDL_RenderPresent(renderer);

	//! CONTROL DE FRAMES Y TIEMPOS ======================================================================================
		countFrames++; // Contar frames
		frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame anterior
		if (frame_time < 1000 / 60) SDL_Delay(1000 / 60 - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
		current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
		FPS = countFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS)
	}

//! CERRAR Y LIBERAR RECURSOS ****************************************************************************************************************************/

	freeText(textIntruc);
	freeText(textFPS);
	QuitSDL(window, renderer, &playfield);

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