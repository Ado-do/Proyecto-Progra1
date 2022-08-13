#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#include "Tetris_static.h"

//! Definir funciones principales

// In game
void tetrisGameplay(Playfield *playfield, Tetromino *curr, Tetromino *next, Tetromino *holder) {
	//! INPUT ======================================================================================
	gameInput();

	//! LOGICA Y CAMBIOS ======================================================================================
	// Comprobar tiempo de caida si corresponde 
	if (fallDelay > 0) fallDelay--;
	if (checkFallTime(totalFrames, difficulty)) fall = true;

	// Aplicar logica y fisicas
	gameUpdate(playfield, curr, next, holder);

	// Actualizar playfield si una pieza cayo en stack
	if (droped) {
		updatePlayfield(playfield, curr, next);
		calculateScore(clearedLines);

		if (test) {
			printf("Level %d (caida cada %.2f (%ld) frames)\n", level, difficulty, lroundf(difficulty)); //? Test lvls
			printf("Score: %d, Combo: %d\n", score, combo); //? Test score
		}

		droped = false;
	}

	if (gameOver) {
		if (test) {
			printf("\nMatriz final de tablero:\n");
			printPlayfield(playfield);
		}
		textScore->rect.x = SCREEN_WIDTH/2 - textScore->rect.w/2;
		textScore->rect.y = SCREEN_HEIGHT/2 + 100;
	}
	
	if (nextLevel) {
		difficulty = calculateDifficulty(level);
		if (currBackground < 3) {
			currBackground++;
		} else {
			currBackground = 0;
		}
	}
	
//! RENDER ======================================================================================
	//* Cargar texturas en caso de actualizar
	// Si se limpiaron lineas, actualizar texto de score y texto
	if (clearedLines > 0) {
		// Actualizar texto lineas
		updateTextTexture(renderer, textLines, totalLines);
		// Actualizar texto score
		updateTextTexture(renderer, textScore, score);
		clearedLines = 0;
	}
	// Si se paso el nivel, actualizar texto de niveles
	if (nextLevel) {
		// Actualizar texto nivel
		updateTextTexture(renderer, textLevel, level);
		nextLevel = false;
	}

	//* Limpiar anterior frame
	SDL_RenderClear(renderer);

	//* Precargar texturas en render
	// Fondo ================================================================
	renderBackground(renderer, backgrounds[currBackground], gameboardInt);
	renderNextHold(renderer, next, holder);

	// Texto ================================================================
	// Renderizar texto fps
	renderFPS(renderer, textFPS);

	renderText(renderer, textScore);
	renderText(renderer, textLines);
	renderText(renderer, textLevel);

	renderText(renderer, textIntruc);

	// Playfield ============================================================
	renderPlayfield(renderer, playfield);
	renderGhostTetromino(renderer, playfield, curr);
	renderCurrTetromino(renderer, curr);

	// Interfaz superpuesta =================================================
	SDL_RenderCopy(renderer, gameboardExt, NULL, NULL);

	//* Mostrar textura en pantalla
	SDL_RenderPresent(renderer);
}
// Pantalla game Over
void tetrisGameOver(SDL_Renderer *renderer, SDL_Texture *gameOverTextures[], Text* textRecord, Text* textScore) {
	while (SDL_PollEvent(&events)) {
		if(events.type == SDL_QUIT) {
			running = false;
			break;
		}
		if (events.type == SDL_KEYDOWN) {
			if (events.key.keysym.sym == SDLK_ESCAPE) {
				running = false;
				break;
			}
		}
	}
	renderGameOver(renderer, gameOverTextures, textRecord, textScore);
}

//! Definir funciones secundarias

// Funcion que recibe el input del juego //TODO: Cambiar running -> gameover
void gameInput() {
	while(SDL_PollEvent(&events)) {
		if(events.type == SDL_QUIT) {
			running = false;
			break;
		}
		if (events.type == SDL_KEYDOWN) {
			switch (events.key.keysym.sym) {
				case SDLK_UP:
					if (!events.key.repeat) rotation = COUNTER_CLOCKWISE;
					break;
				case SDLK_DOWN:		softDrop = true; break;
				case SDLK_RIGHT:	right = true; break;
				case SDLK_LEFT:		left = true; break;
				case SDLK_z:
					if (!events.key.repeat) rotation = COUNTER_CLOCKWISE;
					break;
				case SDLK_x:
					if (!events.key.repeat) rotation = CLOCKWISE;
					break;
				case SDLK_a:
					if (!events.key.repeat) rotation = DOUBLE_CLOCKWISE;
					break;
				case SDLK_c: hold = true; break;
				case SDLK_SPACE:
					if (!events.key.repeat) hardDrop = true;
					break;
				case SDLK_r:
					if (!events.key.repeat) restart = true;
					break;
				case SDLK_ESCAPE: 	gameOver = true; break;
									// running = false; break;
				default: break;
			}
		}
	}
}

// Game updater (revisa coliones antes de mover)
void gameUpdate(Playfield *playfield, Tetromino *curr, Tetromino *next, Tetromino *holder) {
//* Process input
	if (rotation) {
		rotated = true;
		rotateTetromino(curr, rotation);
		if (collision(playfield, curr)) {
			switch (rotation) {
				case CLOCKWISE: 		rotateTetromino(curr, COUNTER_CLOCKWISE); break;
				case COUNTER_CLOCKWISE: rotateTetromino(curr, CLOCKWISE); break;
				case DOUBLE_CLOCKWISE: 	rotateTetromino(curr, DOUBLE_CLOCKWISE); break;
			}
			rotated = false;
		}
		rotation = false;
	}
	if (hardDrop) {
		if (holded) holded = false;
		hardDropTetromino(playfield, curr);
		hardDrop = false;
	}
	if (softDrop) {
		softDropTetromino(playfield, curr);
		softDrop = false;
	}
	if (hold) {
		if (lock_delay) { // En el caso de haber interrumpido un lock delay
			countLocks = 0;
			rotated = shifted = false;
			lock_delay = false;
		}

		if (!firstHold) {
			*holder = tetrominoes[curr->nShape];
			newTetromino(curr, next);
			firstHold = true;
			holded = true;
		} else {
			if (!holded) {
				int aux = curr->nShape;
				*curr = *holder;
				*holder = tetrominoes[aux];
				holded = true;
			}
		}
		hold = false;
	}
	if (right) {
		shifted = true;
		curr->x++;
		if (collision(playfield, curr)) {
			curr->x--;
			shifted = false;
		}
		right = false;
	}
	if (left) {
		shifted = true;
		curr->x--;
		if (collision(playfield, curr)) {
			curr->x++;
			shifted = false;
		}
		left = false;
	}
	if (fall) {
		curr->y++;
		if (collision(playfield, curr)) {
			curr->y--;
			// "Lock delay" https://tetris.fandom.com/wiki/Lock_delay
			if (!lock_delay) {
				countLocks = 0;
				lock_timer = totalFrames + 30; // 30 frames = 0.5 segs (60 fps game)
				lock_delay = true;
				if (test) printf("LOCK DELAY INICIADO\n"); //? TEST
			}
		}
		fall = false;
	}

//* After input process
	// Reiniciar lock delay si se hubo una rotacion o movimiento exitoso durante un lock delay anterior
	if (lock_delay) {
		if ((shifted || rotated) && countLocks < 5) { // Cap de 5 resets
			lock_timer = totalFrames + 30;
			rotated = shifted = false;
			if (test) printf("LOCK DELAY RESET\n"); //? TEST
			countLocks++;
		}

		if (totalFrames > lock_timer) {
			countLocks = 0;
			lock_delay = false;
			droped = true;
		}
	}

	if (restart) { //TODO: Reiniciar todo el gameplay (dificultad y todo)
		if (currBackground < 3) currBackground++;
		else currBackground = 0;
		initPlayfield(playfield);
		restart = false;
	}
}

// Funcion que actualiza tablero
void updatePlayfield(Playfield *playfield, Tetromino *curr, Tetromino *next) {
	if (lock_delay) { // En el caso de haber interrumpido un lock delay
		countLocks = 0;
		rotated = shifted = false;
		lock_delay = false;
	}
	// Guardar caracteristicas de ultima pieza dropeada
	lastDropedRow = curr->y;
	lastDropedSize = curr->size;

	// Ingresar pieza actual al stack
	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			if (curr->matrix[i][j]) {
				playfield->matrix[i + curr->y][j + curr->x] = curr->shape;
			}
		}
	}

	countStackHeight(playfield); // Comprobar altura del Stack

	if (nDrops >= 3) clearedLines = deleteLines(playfield);
	totalLines += clearedLines;
	// Revisar condiciones de Game Over
	if (lastStackRow <= 5) gameOver = checkGameOver(curr);
	// Generar nueva pieza si se siguen en juego
	if (!gameOver) newTetromino(curr, next);

	// Testeo
	if (test) {
		printPlayfield(playfield); //? Imprimir matrix actual
		printf("lastDropedRow: %d, lastDropedSize: %d!!!!!!!!!!\n", curr->y, curr->size); //? Test de drops
	}
}

// Generar nueva pieza de manera pseudo aleatoria //TODO: FALTA AGREGAR 7-BAG
void newTetromino(Tetromino *curr, Tetromino *next) {
	//! 7-bag randomizer (https://tetris.fandom.com/wiki/Random_Generator)
	// Generar dos bolsas de piezas aleatorias al iniciar el juego
	if (nDrops == 0) {
		currBag = 0;
		// Algoritmo "Fisher–Yates shuffle" (Permutacion aleatoria)
		for (int i = 7-1; i >= 0; i--) {
			int j = rand() % (i+1);
			int aux = bags[currBag][i];
			bags[currBag][i] = bags[currBag][j];
			bags[currBag][j] = aux;
		}
		// Asignar piezas "curr" y "next" a primeros elementos de la bolsa generada aleatoriamente
		*curr = tetrominoes[bags[0][0]];
		*next = tetrominoes[bags[0][1]];
		currElem = 1;

	// Elegir siguiente pieza en la bolsa (generar otra si ya se acabaron los elementos)
	} else {
		currElem++;
		if (currElem == 7) {
			currElem = 0;
			currBag = (currBag == 0) ? 1 : 0;
			// Generar bolsa
			for (int i = 7-1; i >= 0; i--) {
				int j = rand() % (i+1);
				int aux = bags[currBag][i];
				bags[currBag][i] = bags[currBag][j];
				bags[currBag][j] = aux;
			}
			*curr = tetrominoes[next->nShape];
			*next = tetrominoes[bags[currBag][currElem]];
		} else {
			*curr = tetrominoes[next->nShape];
			*next = tetrominoes[bags[currBag][currElem]];
		}
	}

	//! Pseudoaleatorio randomizer
	// if (nDrops == 0) {
	// 	*curr = tetrominoes[rand() % 7];
	// 	*next = tetrominoes[rand() % 7];
	// } else {
	// 	*curr = *next;
	// 	*next = tetrominoes[rand() % 7];
	// }

	// Pequeño delay antes de que la nueva pieza baje de nuevo
	fallDelay = lroundf(difficulty);
}

// Funcion que revisa si el jugador perdio //TODO: MEJORAR DETECCION DE GAMEOVER
bool checkGameOver(Tetromino *curr) {
	bool gameOver = false;
	// Contar bloques de la pieza encima del tablero, si los 4 estan arriba, pierdes
	int countBlocks = 0;
	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			// Coordenada Y respecto al tablero de bloque actual
			int coordY = i + curr->y;
			//TODO: Hacer mas flexible respecto a situacion del tablero y pieza actual
			if (curr->matrix[i][j] && coordY <= 2) countBlocks++;
		}
	}
	switch (countBlocks) {
		case 3: //* "Mini clutch" (Salvar juego si la ultima pieza logro limpiar al menos una linea antes de caer)
			if (clearedLines == 0) {
				if (test) printf("GAME OVER!!!!\n"); //? Avisar gameover
				gameOver = true;
			}
			break;
		case 4: //* "Top out" (Pieza cayo totalmente fuera de playfield)
			if (test) printf("GAME OVER!!!!\n"); //? Avisar gameover
			gameOver = true;
			break;
		default: break;
	}
	return gameOver;
}

// Funcion que comprueba estado de linea (-1 = borde, 0 = vacia, 1 = incompleta, 2 = completa)
int checkLineState(Playfield *playfield, Uint8 row) {
	int countBlocks = 0;
	for (int column = 1; column < BOARD_WIDTH - 1; column++) {
		if (playfield->matrix[row][column] == '#') return -1;
		if (playfield->matrix[row][column] != ' ') countBlocks++;
	}
	if (countBlocks == 10) return 2;
	else if (countBlocks == 0) return 0;
	else return 1;
}

// Funcion que elimina lineas completas por la anterior pieza dropeada
int deleteLines(Playfield *playfield) {
	Uint8 deletedLines = 0;
	
	Uint8 rowInterval = lastDropedRow + lastDropedSize;
	if (rowInterval > BOARD_HEIGHT - 2) rowInterval = BOARD_HEIGHT - 2;

	//* Llenar lineas completas de espacios vacios (Recorrer matriz hacia abajo)
	for (int row = lastDropedRow; row <= rowInterval; row++) {
		if (checkLineState(playfield, row) == 2) {
			for (int column = 1; column < BOARD_WIDTH - 1; column++) {
				playfield->matrix[row][column] = ' ';	
			}
			deletedLines++;
		}
	}

	//* Bajar piezas que quedaron flotando (Recorrer matriz hacia arriba)
	for (int mainRow = rowInterval; mainRow > lastStackRow; mainRow--) { 
		if (checkLineState(playfield, mainRow) == 0) {
			int secondRow = mainRow - 1; // Recorrer filas hasta encontrar filas incompletas
			while (checkLineState(playfield, secondRow) == 0 && secondRow > lastStackRow) secondRow--; // Saltarse lineas vacias
			// Swap linea vacia (mainRow) con siguiente linea incompleta (sencondRow)
			for (int column = 1; column < BOARD_WIDTH - 1; column++) {
				playfield->matrix[mainRow][column] = playfield->matrix[secondRow][column];
				playfield->matrix[secondRow][column] = ' ';
			}
		}
	}

	return deletedLines;
}

// Funcion que revisa colisiones de tetromino actual
bool collision(Playfield *playfield, Tetromino *curr) {
	for (int i = 0; i < curr->size; i++) {
		for (int j = 0; j < curr->size; j++) {
			if (curr->matrix[i][j] && playfield->matrix[i + curr->y][j + curr->x] != ' ') {
				return true;
			}
		}
	}
	return false;
}

// Funcion que cuenta altura actual del stack
void countStackHeight(Playfield *playfield) {
	for (int row = 0; row < BOARD_HEIGHT - 1; row++) {
		if (checkLineState(playfield, row) == 1) {
			lastStackRow = row;
			if (test) printf("lastStackRow: %d (altura: %d)\n", lastStackRow, (BOARD_HEIGHT - 1) - lastStackRow);
			break;
		}
	}
}

//TODO: WALL KICK PARA CASOS BASICOS COMO POR EJEMPLO COLISION CON BORDE CUANDO "lastRowStack" ES MENOR A "posY" DE FIGURA
void wallKickFRUNA(Playfield *playfield, Tetromino *curr, Sint8 rotation);

// Funcion que rota piezas
void rotateTetromino(Tetromino *tetro, const Sint8 sense) {
	Tetromino copy = *tetro;
	for(int i = 0; i < tetro->size; i++) {
			for(int j = 0; j < tetro->size; j++) {
				tetro->matrix[i][j] = copy.matrix[j][i];
			}
		}
	copy = *tetro;
	switch (sense) {
		case COUNTER_CLOCKWISE:
			for(int i = 0; i < tetro->size/2; i++) {
				for(int j = 0; j < tetro->size; j++) {
					bool t = copy.matrix[i][j];
					tetro->matrix[i][j] = copy.matrix[tetro->size - i - 1][j];
					tetro->matrix[tetro->size - i - 1][j] = t;
				}
			}
			break;
		case CLOCKWISE:
			for(int i = 0; i < tetro->size; i++) {
				for(int j = 0; j < tetro->size/2; j++) {
					bool t = copy.matrix[i][j];
					tetro->matrix[i][j] = copy.matrix[i][tetro->size - j - 1];
					tetro->matrix[i][tetro->size - j - 1] = t;
				}
			}
			break;
		case DOUBLE_CLOCKWISE:
			for(int i = 0; i < tetro->size; i++) {
				for(int j = 0; j < tetro->size/2; j++) {
					bool t = copy.matrix[i][j];
					tetro->matrix[i][j] = copy.matrix[i][tetro->size - j - 1];
					tetro->matrix[i][tetro->size - j - 1] = t;
				}
			}
			rotateTetromino(tetro, CLOCKWISE);
			break;
		default:
			break;
	}
}

// Funcion que hacer caer pieza al stack instantaneamente
void hardDropTetromino(Playfield *playfield, Tetromino *curr) {
	if (nDrops < 3) nDrops++;
	// Bajar directamente hasta una altura en que sea una colision
	if (curr->y < lastStackRow - 4) {
		curr->y = lastStackRow - 4;
	}
	// Bajar pieza hasta que choque
	while (!collision(playfield, curr)) curr->y++;
	curr->y--;

	droped = true;
}

// Funcion que mueve un bloque abajo la pieza //TODO: MEJORAR SOFTDROP ****************************************************
void softDropTetromino(Playfield *playfield, Tetromino *curr) {
	curr->y++;
	if (collision(playfield, curr)) {
		curr->y--;
		droped = true;
		if (nDrops < 3) nDrops++;
	}
}

// Funcion que maneja delay de fall en base a dificultad actual
bool checkFallTime(Uint64 totalFrames, float difficulty) {
	if ((totalFrames % lroundf(difficulty) == 0)) {
		return true;
	} else {
		return false;
	}
}

// Funcion que calcula score
void calculateScore(int linesCleared) {
	// Calcular score segun cantida de lineas limpiadas al mismo tiempo //TODO: AGREGAR CASOS ESPECIALES EJ: Tspins
	switch(linesCleared) {
		case 1: {score += 100  * (level + combo); break;} // Single
		case 2: {score += 300  * (level + combo); break;} // Double
		case 3: {score += 500  * (level + combo); break;} // Triple
		case 4: {score += 1000 * (level + combo); break;} // Tetris
		//TODO TSPINS
		//TODO FULL CLEAR
		default: break;
	}

	if (linesCleared > 0) combo++;
	else combo = 0;

	// Cada 20 lineas limpiadas
	if (totalLines >= 10 * level) { //TODO Encontrar mejor condicion para pasar niveles
		if (level < 15) { //TODO Despues del nivel 13 las lineas caen mas de dos filas por frame (quizas lo arregle alfinal)
			level++;
			nextLevel = true;
		}
	}
}

// Calcular dificultd segun nivel //TODO ARREGLAR FORMULA
float calculateDifficulty(Uint8 level) {
	return (pow((0.8 - ((level - 1) *0.007)), level - 1)) * 60;
}

// Funcion que inicializa todo SDL y demas librerias usadas
bool initTetris(SDL_Window **ptrWindow, SDL_Renderer **ptrRenderer) { 
	// Inicializar toda la biblioteca de SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("*** Error initializing SDL2: %s\n", SDL_GetError());
		return 0;
	}
	// Calidad de escalado
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") == SDL_FALSE) {
		printf("*** Error assigning scaling hint: %s\n", SDL_GetError());
	}
	// Inicializar SDL_image
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		printf("*** Error initializing SDL_Image: %s\n", IMG_GetError());
		return 0;
	}
	// Inicializar SDL_TTF
	if (TTF_Init() == -1) {
		printf("*** Error initializing SDL_TTF: %s\n", TTF_GetError());
		return 0;
	}
	// Crear ventana
	*ptrWindow = SDL_CreateWindow("Intento de tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (*ptrWindow == NULL) {
		printf("*** Error creating window: %s\n", SDL_GetError());
		return 0;
	}
	// Crear renderer
	*ptrRenderer = SDL_CreateRenderer(*ptrWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*ptrRenderer == NULL) {
		printf("*** Error creating renderer: %s\n", SDL_GetError());
		return 0;
	}
	SDL_SetRenderDrawColor(*ptrRenderer, 0, 0, 0, 255); // Color fondo (Negro)

	return 1;
}

// Inicializar/Reiniciar tablero
void initPlayfield(Playfield *playfield) {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (j == 0 || j == BOARD_WIDTH - 1) {
				playfield->matrix[i][j] = '#';
			} else if (i == BOARD_HEIGHT - 1) {
				playfield->matrix[i][j] = '#';
			} else {
				playfield->matrix[i][j] = ' ';
			}
		}
	}
	if (test) printPlayfield(playfield);
}

// Funcion que inicializa objeto de la estructura Text
Text* initText(const char *str, FontInfo *fontInfo, const SDL_Color color, const int x, const int y, const float size) {
	Text* text = (Text*) malloc(sizeof(Text));
	*text = (Text) {	// Se castea a un dato tipo "Text", ya que como estamos inicializandolo desde un puntero tenemos que usar un literal compuesto (googlea "Compound literal")
		.string		= "", 									// String del texto (vacio por ahora)
		.font 		= fontInfo->font,						// Fuente (Cargada con ayuda de TTF_OpenFont("path del font", tamaño letra))
		.color 		= color,								// Color del texto
		.texture	= NULL,									// Textura (NULL ya que se crea y asigna posteriormente)
		.rect 		= {x, y, 0, 0},							// Rect del texto (Posicion/Tamaño), el tamaño se asigna posteriormente al crear la textura
		.size		= size
	};
	strcpy(text->string, str);	// Copiar parametro string en la string de estructura
	if (text->font == NULL) printf("*** Error initText \"%s\": %s\n", text->string,TTF_GetError());
	return text;
}

// Abre font para poder utilizarla al crear textos
void openFont(FontInfo *info) {
	info->font = TTF_OpenFont(info->path, info->size);
	if (info->font == NULL) printf("*** Error openFont: %s\n", TTF_GetError());
}

// Funcion que asigna texturas a tetrominos
void loadTetrominoesTextures(SDL_Renderer *renderer) {
	bool error = false;
	// Asignar texturas
	for (int nShape = 0; nShape < 9; nShape++) {
		switch (nShape) {
			case 7:
				ghostBlock = IMG_LoadTexture(renderer, blockPaths[nShape]);
				if (ghostBlock == NULL) error = true;
				break;
			case 8:
				lockBlock = IMG_LoadTexture(renderer, blockPaths[nShape]);
				if (lockBlock == NULL) error = true;
				break;
			default:
				blockColors[nShape] = IMG_LoadTexture(renderer, blockPaths[nShape]);
				if (blockColors[nShape] == NULL) error = true;
				break;
		}
	}
	if (error) printf("Error loadTetrominoesTextures: %s\n", SDL_GetError());
}

// Cargar fondos
void loadBackgroundsTextures(SDL_Renderer *renderer, SDL_Texture *backgrounds[]) {
	bool error = false;
	for (int i = 0; i < 4; i++) {
		backgrounds[i] = IMG_LoadTexture(renderer, backgroundsPath[i]);
		if (backgrounds[i] == NULL) error = true;
	}
	if (error) printf("Error loadBackgroundsTextures: %s\n", SDL_GetError());
}

void loadGameOverTextures(SDL_Renderer *renderer, SDL_Texture *gameOverTextures[]) {
	bool error = false;
	for (int i = 0; i < 5; i++) {
		gameOverTextures[i] = IMG_LoadTexture(renderer, gameOverPath[i]);
		if (gameOverTextures[i] == NULL) error = true;
	}
	if (error) {
		printf("Error loadGameOverTextures(LoadTextures): %s\n", SDL_GetError());
	} else {
		if (SDL_QueryTexture(gameOverTextures[1], NULL, NULL, &gameOverRect.w, &gameOverRect.h) < 0) {
			printf("Error loadGameOverTextures(QueryTexture): %s\n", SDL_GetError());
		} else {
			gameOverRect.x = SCREEN_WIDTH/2 - gameOverRect.w/2;
			gameOverRect.y = 300;
		}
	}
}

// Funcion que carga textura de texto
void loadTextTexture(SDL_Renderer *renderer, Text *text) {
	if (text->texture != NULL) SDL_DestroyTexture(text->texture);

	SDL_Surface* textSurface = TTF_RenderText_Solid(text->font, text->string, text->color);
	if (textSurface == NULL) {
		printf("*** Error loadTextTexture (Surface): %s\n", TTF_GetError());
	} else {
		text->rect.w = textSurface->w * text->size;
		text->rect.h = textSurface->h * text->size;
	}
	text->texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if (text->texture == NULL) printf("*** Error loadTextTexture (Texture): %s\n", SDL_GetError());
	SDL_FreeSurface(textSurface);
}

// Funcion que actualiza textura de texto
void updateTextTexture(SDL_Renderer *renderer, Text *text, int number) {
	size_t i; // Indice en text->string
	// Busca el primer numero en la string
	for (i = 0; i < strlen(text->string); i++) {
		if (isdigit(text->string[i]) != 0) break;
	}
	// Cambiar string
	snprintf(text->string + i, 10,"%d", number);
	// Actualizar texture
	loadTextTexture(renderer, text);
}

//? Imprimir grilla actual
void printPlayfield(Playfield *playfield) {
	int row = 0, column = 0;
	printf("La matriz de la grilla actual es:\n");
	for (int i = 0; i < BOARD_HEIGHT + 1; i++) {
		if (i == 3) printf("   ---------------------------------------\n");
		if (i == BOARD_HEIGHT) printf("   ---------------------------------------\n");
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (j == 0 && i < BOARD_HEIGHT) printf("%2d | ", row++);
			if (i < BOARD_HEIGHT) printf("%2c ", playfield->matrix[i][j]);
			if (j == BOARD_WIDTH - 1 && i < BOARD_HEIGHT) printf("|\n");
			if (i == BOARD_HEIGHT) {
				if (j != 0) printf("%2d ", column++);
				else printf("     %2d ", column++);
			}
		}
	}
	printf("\n");
}

// Funcion que renderiza textura de texto
void renderText(SDL_Renderer *renderer, Text *text) {
	if (SDL_RenderCopy(renderer, text->texture, NULL, &text->rect) < 0) printf("*** Error renderText \"%s\": %s\n", text->string, SDL_GetError());
}

// Funcion para renderizar especificamente los fps
void renderFPS(SDL_Renderer *renderer, Text *textFPS) {
	// Cargar texto de FPS actuales
	if (totalFrames % 30 == 0) {
		snprintf(textFPS->string + 5, 6,"%.2f", FPS);
		loadTextTexture(renderer, textFPS); // Cargar textura de string con cantidad de FPS
	}
	if (totalFrames > 30) {
		renderText(renderer, textFPS);
	}
}

// Renderizar texturas de fondo
void renderBackground(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *gameboardInt) {
	if (SDL_RenderCopy(renderer, background, NULL, NULL) < 0) printf("Error renderBackground(background): %s\n", SDL_GetError());
	if (SDL_RenderCopy(renderer, gameboardInt, NULL, NULL) < 0) printf("Error renderBackground(gameboardInt): %s\n", SDL_GetError());
}

// Renderizar todo respecto a pantalla de game over //TODO AGREGAR PANTALLA DE GAME OVER
void renderGameOver(SDL_Renderer *renderer, SDL_Texture *gameOverTextures[], Text *textRecord, Text *textScore) {
	if (totalFrames % 20 == 0) currGameOverTitle++;
	if (currGameOverTitle > 4) currGameOverTitle = 1;

	SDL_RenderClear(renderer);

	if (SDL_RenderCopy(renderer, gameOverTextures[0], NULL, NULL) < 0) {
		printf("*** Error renderGameOver(gameOverBackground): %s\n", SDL_GetError());
	}
	if (SDL_RenderCopy(renderer, gameOverTextures[currGameOverTitle], NULL, &gameOverRect) < 0) {
		printf("*** Error renderGameOver(gameOverTitles): %s\n", SDL_GetError());
	}
	renderText(renderer, textRecord);
	renderText(renderer, textScore);
	
	renderFPS(renderer, textFPS);

	SDL_RenderPresent(renderer);
}

// Renderizar piezas next y hold
void renderNextHold(SDL_Renderer *renderer, Tetromino *next, Tetromino *holder) {
	bool errorNext = false, errorHold1 = false, errorHold2 = false;
	int currRectNext = 0, currRectHolder = 0, nextX = 665, nextY = 200, holderX = 90, holderY = 200; // Posicion de pieza Hold y Next
	// Ajustar segun pieza (para que este centrado)
	switch (next->shape) {
		case 'O': nextX += 20; break;
		case 'I': nextX -= 20; nextY -= 5; break;
		default: break;
	}
	// Renderizar Next
	for(int i = 0; i < next->size; i++) {
		for(int j = 0; j < next->size; j++) {
			if(next->matrix[i][j]) {
				next->rects[currRectNext].w = next->rects[currRectNext].h = TILE_SIZE + 1;
				next->rects[currRectNext].x = (j * (TILE_SIZE + 1)) + nextX;
				next->rects[currRectNext].y = (i * (TILE_SIZE + 1)) + nextY;
				if (SDL_RenderCopy(renderer, blockColors[next->nShape], NULL, &next->rects[currRectNext])) {
					errorNext = true;
				}
				// SDL_RenderDrawRect(renderer, &next->rects[currRectNext]);
				currRectNext++;
			}
		}
	}
	// (Si ya se holdeo una pieza previamente) Renderizar Hold
	if (firstHold) {
		// Ajustar segun pieza
		switch (holder->shape) {
			case 'O': holderX += 20; break;
			case 'I': holderX -= 20; holderY -= 5; break;
			default: break;
		}
		// Renderizar Hold
		for(int i = 0; i < holder->size; i++) {
			for(int j = 0; j < holder->size; j++) {
				if(holder->matrix[i][j]) {
					holder->rects[currRectHolder].w = holder->rects[currRectHolder].h = TILE_SIZE + 1;
					holder->rects[currRectHolder].x = (j * (TILE_SIZE + 1)) + holderX;
					holder->rects[currRectHolder].y = (i * (TILE_SIZE + 1)) + holderY;
					if (holded) { // Textura de "Lock" si no se puede volver a holdear
						if (SDL_RenderCopy(renderer, lockBlock, NULL, &holder->rects[currRectHolder]) < 0) errorHold1 = true;
					} else { // Textura de la pieza
						if (SDL_RenderCopy(renderer, blockColors[holder->nShape], NULL, &holder->rects[currRectHolder]) < 0) errorHold2 = true;
					}
					currRectHolder++;
				}
			}
		}
	}
	if (errorNext) printf("Error renderNextHold(Next): %s\n", SDL_GetError());
	if (errorHold1) printf("Error renderNextHold(HoldLock): %s\n", SDL_GetError());
	if (errorHold2) printf("Error renderNextHold(Hold): %s\n", SDL_GetError());
}

// Renderizar pieza actual
void renderCurrTetromino(SDL_Renderer *renderer, Tetromino *tetro) {
	// Indice de rects de la pieza
	int currRect = 0;
	for (int i = 0; i < tetro->size; i++) {
		for (int j = 0; j < tetro->size; j++) {
			if (tetro->matrix[i][j]) {
				tetro->rects[currRect].w = tetro->rects[currRect].h = TILE_SIZE + TILE_MARGIN;
				tetro->rects[currRect].x = ((tetro->x - 1 + j) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
				tetro->rects[currRect].y = ((tetro->y + i) * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
				SDL_RenderCopy(renderer, blockColors[tetro->nShape], NULL, &(tetro->rects[currRect]));
				// SDL_RenderDrawRect(renderer, &(tetro->rects[currRect]));
				currRect++;
			}
		}
	}
}

// Renderiza previsualizacion de caida de pieza actual
void renderGhostTetromino(SDL_Renderer *renderer, Playfield *playfield, Tetromino *curr) {
	bool noFitting = false;
	int currRect = 0;
	Tetromino currGhost = *curr;
	while (!noFitting) {
		if (collision(playfield, &currGhost)) {
			currGhost.y--;
			for(int i = 0; i < currGhost.size; i++) {
				for(int j = 0; j < currGhost.size; j++) {
					if(currGhost.matrix[i][j]) {
						currGhost.rects[currRect].w = currGhost.rects[currRect].h = TILE_SIZE;
						currGhost.rects[currRect].x = ((currGhost.x - 1 + j) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
						currGhost.rects[currRect].y = ((currGhost.y + i) * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
						SDL_RenderCopy(renderer, ghostBlock, NULL, &currGhost.rects[currRect]);
						currRect++;
					}
				}
			}
			noFitting = true;
		} else currGhost.y++;
	}
}

// Renderizar tablero
void renderPlayfield(SDL_Renderer *renderer, Playfield *playfield) {
	for (int row = BOARD_HEIGHT - 2; row >= lastStackRow; row--) {
		for (int column = 1; column < BOARD_WIDTH - 1; column++) {
			if (playfield->matrix[row][column] != ' ') {
				SDL_Rect tmpRect;
				int nShape;
				tmpRect.w = tmpRect.h = TILE_SIZE + TILE_MARGIN;
				tmpRect.x = ((column - 1) * (TILE_SIZE + 1)) + BOARD_X_ORIGIN;
				tmpRect.y = (row * (TILE_SIZE + 1)) + BOARD_Y_ORIGIN;
				switch (playfield->matrix[row][column]) {
					case 'L': nShape = 0; break;
					case 'Z': nShape = 1; break;
					case 'I': nShape = 2; break;
					case 'J': nShape = 3; break;
					case 'O': nShape = 4; break;
					case 'S': nShape = 5; break;
					case 'T': nShape = 6; break;
					default: break;
				}
				if (SDL_RenderCopy(renderer, blockColors[nShape], NULL, &tmpRect) < 0) {
					printf("*** Error renderPlayfield: %s\n", SDL_GetError());
				}
				// SDL_RenderDrawRect(renderer, &tmpRect);
			}
		}
	}
}

void framesControl() {
	totalFrames++; // Contar frames
	frame_time = SDL_GetTicks64() - capTimer; // Tiempo de creacion de frame anterior
	if (frame_time < 1000 / 60) SDL_Delay(1000 / 60 - frame_time);  // Esperar si el tiempo de creacion de frame fue menor a 1000/60 ticks, de manera de que el juego vaya a 60FPS
	current_time = SDL_GetTicks64() - start_time; // Tiempo actual en juego
	FPS = totalFrames / (current_time / 1000.f); // Total de frames dividos por el tiempo total (seg) en juego = (FPS)
}

// Funcion que libera texto junto a su textura y fuente cargada
void destroyText(Text *text) {
	SDL_DestroyTexture(text->texture);
	free(text);
}

// Funcion que apaga y limpia todos los subsystemas de SDL usados.
void quitTetris(SDL_Window *window, SDL_Renderer *renderer) {
	bool error = false;
	if (*SDL_GetError() != '\0') {
		printf("*** LAST SDL ERROR: %s\n", SDL_GetError());
		error = true;
	}
	if (*TTF_GetError() != '\0') {
		printf("*** LAST SDL_TTF ERROR: %s\n", TTF_GetError());
		error = true;
	}
	if (*IMG_GetError() != '\0') {
		printf("*** LAST SDL_IMG ERROR: %s\n", IMG_GetError());
		error = true;
	}
	if (error) SDL_Delay(3000);

	TTF_Quit();
	IMG_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
