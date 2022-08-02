#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include "Tetris_static.h"
#include "Tetris_functions.h"

/* //! LISTA DE COSAS POR HACER EN ORDEN
 * - ARREGLAR FPS
 * - PULIR GAMEOVER
 * - HACER FUNCIONAL PARA LINUX (ARREGLAR "segmentation fault (core dump")
 * - MEJORAR ALEATORIEDAD (7-BAG)
 * - INCLUIR PANTALLA DE INICIO Y MENU
 * - PAUSA
 * - SONIDOS/MUSICA
 * - MEJORAR DIFICULTAD
 * - AGREGAR WALLKICKS -> MEJORAR SCORE (TSPIN, FULLCLEAR)
 */

//! MAIN ====================================================================================================================================
int main(int argc, char *argv[]) {
	srand(time(NULL));

	if (!initSDL(&window, &renderer)) {
		printf("Error inicializando SDL: %s\n", SDL_GetError());
		SDL_Delay(3000);
		return -1;
	}
//! DECLARAR Y INICIALIZAR *******************************************************************************************************

	textFPS = initText("FPS: 60", &upheavalFont, (SDL_Color){255,255,255,200}, 10, 1, 1);
	textIntruc = initText("Press R to restart game", &upheavalFont, (SDL_Color){255,255,255,200}, 610, 1, 1);
	textLevel = initText("Level: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 50, 700, 1.3);
	textScore = initText("Score: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 50, 730, 1.3);
	textLines = initText("Lines: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 50, 760, 1.3);
	textRecord = initText("New Record! Score: 0", &upheavalFont, (SDL_Color){255,255,255,255}, 280, 600, 1.5);

	loadBackgroundsTexture(renderer, backgrounds);
	loadGameOverTexture(renderer, gameOverTextures);
	loadTetrominoesTexture(renderer);
	gameboardExt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardExt.png");
	gameboardInt = IMG_LoadTexture(renderer, "assets/gameboards/gameboardInt.png");

	loadTextTexture(renderer, textIntruc);

	loadTextTexture(renderer, textScore);
	loadTextTexture(renderer, textLines);
	loadTextTexture(renderer, textRecord);

	initPlayfield(&playfield);

	level = 1;
	difficulty = calculateDifficulty(level);

	updateTextTexture(renderer, textLevel, level);
	
	newTetromino(&curr, &next);

	running = 1; // Flag de control de gameloop
	start_time = SDL_GetTicks64(); // Tiempo en que se inicio gameloop

	//! GAME LOOP *********************************************************************************************************************
	while (running) {
		capTimer = SDL_GetTicks64(); // Tiempo de inicio de frame

		//! PANTALLAS (MENU, TETRIS, GAMEOVER)
		if (!gameOver) tetrisGameScreen(&playfield, &curr, &next, &holder);
		else gameOverScreen();

		//! CONTROL DE FRAMES Y TIEMPOS ======================================================================================
		framesControl();
	}

	//! CERRAR Y LIBERAR RECURSOS ****************************************************************************************************************************/
	
	destroyText(textLines);
	destroyText(textScore);
	destroyText(textLevel);
	destroyText(textIntruc);
	destroyText(textFPS);
	quitSDL(window, renderer);

	return 0;
}

// int main(int argc, char const *argv[]) {
    // Pseudo codigo

    /*
    InitSDL();  // Inicializar SDL
    game Tetris;    // Inicializar estructura Tetris
    
    loadTetrisAssets(&game);  // Cargar assets de Tetris

    while (game.running) {  // Gameloop
        game.HandleEvents();    // Manejo de eventos/input

        game.Update();  // Logica del juego

        game.Render();  // Mostrar cambios en pantalla
    }

    game.Release(); // Liberar recursos y cerrar programa
    */
//    return 0;
// }