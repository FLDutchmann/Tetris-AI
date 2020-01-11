#pragma once
#include <SDL2/SDL.h>
#include "tetris.h"
#undef main

class window {
private:
	//The window that will be rendered to
	SDL_Window* gWindow = NULL;

	//The surface contained by the window
	SDL_Surface* gScreenSurface = NULL;

	auto getColor(int c);
	void fillRect(int x, int y, int w, int h, int c);
	void fillRect(int x, int y, int w, int h, SDL_Color c);

public:
	window();
	//Initialise the window. 
	//Returns false if the initialisation failed.
	bool init();
	//Close the window. This is called when the X button is presssed.
	void close();

	void drawScreen(std::vector<tetris*> &game);
	void printText(int, int, const char*);
};