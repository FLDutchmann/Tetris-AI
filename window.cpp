#include "window.h"
#include "tetris.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = SCREEN_WIDTH * 9 / 16;
const SDL_Color BACKGROUND_COLOR = { 0xCC, 0xCC, 0xCC };


window::window() {

}


bool window::init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	} 
	else {
		//Create window
		gWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			std::printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}


void window::close() {
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}


//WARNING: Remember to delete this object
auto rect(int x, int y, int w, int h) {
	SDL_Rect *r = new SDL_Rect();
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
	return r;
}


const float BOARD_X = 0.1;
const float BOARD_Y = 0.1;

const int CELL_WIDTH = SCREEN_WIDTH * 0.020;
const int CELL_HEIGHT = CELL_WIDTH;
// I
// O
// T
// J
// L
// S
// Z
const int CELL_COLOR[8][3] = {
	{0x00, 0x00, 0x00},
	{0x00, 0xFF, 0xFF},
	{0xFF, 0xFF, 0x00},
	{0xFF, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0xFF, 0x77, 0x00},
	{0x00, 0xFF, 0x00},
	{0xFF, 0x00, 0x00}
};


auto window::getColor(int c) {
	return SDL_MapRGB(gScreenSurface->format, CELL_COLOR[c][0], CELL_COLOR[c][1], CELL_COLOR[c][2]);
}


//c refers to the ID of a piece
void window::fillRect(int x, int y, int w, int h, int c) {
	auto r = rect(x, y, w, h);
	SDL_FillRect(gScreenSurface,
		r,
		getColor(c));
	delete r;
}


void window::fillRect(int x, int y, int w, int h, SDL_Color c) {
	auto r = rect(x, y, w, h);
	SDL_FillRect(gScreenSurface,
		r,
		SDL_MapRGB(gScreenSurface->format, c.r, c.g, c.b));
	delete r;
}


int NETWORK_X = 1100;
int NETWORK_Y = 192;
int NEURON_WIDTH = 10;
int NEURON_HEIGHT = 10;


void window::drawScreen(std::vector<tetris*> &game) {
	//Background:
	SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b));

	for (int b = 0; b < game.size(); b++) {
		auto tetromino = game[b]->getCellPositions();


		int x = SCREEN_WIDTH * BOARD_X;
		int y = SCREEN_HEIGHT * BOARD_Y;
	
		//Draw the board

		auto board = game[b]->getBoard();
		for (int i = 2; i < BOARD_HEIGHT; i++) {
			for (int j = 0; j < BOARD_WIDTH; j++) {
				int cx = x + CELL_WIDTH * j;
				int cy = y + CELL_HEIGHT * i;
				int c = board[i][j];
				fillRect(cx+b*300, cy, CELL_WIDTH, CELL_HEIGHT, c);
			}
		}

		//Draw the current piece
		int pieceRow = game[b]->getPieceRow();
		int pieceCol = game[b]->getPieceCol();
		int piece = game[b]->getPiece();
		for (int i = 0; i < 4; i++) {
			int px = tetromino[i][0] + pieceCol;
			int py = tetromino[i][1] + pieceRow;
			int tx = x + px * CELL_WIDTH;
			int ty = y + py * CELL_HEIGHT;
			if (py >= 2)
				fillRect(tx+ b * 300, ty, CELL_WIDTH, CELL_HEIGHT, piece);
		}

	}
	

	SDL_UpdateWindowSurface(gWindow);



}