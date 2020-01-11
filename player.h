#pragma once
#include <queue>
#include <SDL2/SDL.h>

class player{
private:
	std::queue<int> moves;

public:
	void addMove(SDL_Event e);
	int getNextMove();
};