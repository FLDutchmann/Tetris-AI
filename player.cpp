#include "player.h"
#include <iostream>


void player::addMove(SDL_Event e) {
	
	int m = -1;
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_LEFT:
			m = 0;
			break;
		case SDLK_DOWN:
			m = 1;
			break;
		case SDLK_RIGHT:
			m = 2;
			break;
		case SDLK_UP:
			m = 3;
			break;
		}
		if (m != -1) {
			moves.push(m);
		}
	}
}


int player::getNextMove() {
	if (moves.empty()) return -1;

	int m = moves.front(); moves.pop();
	return m;
}




