
#include "window.h"
#include "tetris.h"
#include "player.h"
#include "linear_ai.h"

#include <iostream>
#include <ctime> // used for random seed
#include <chrono> // used for timing
#include <algorithm>
#include <fstream>
#include <vector>


window gameWindow;
std::vector<tetris*> games;

//Time at which current frame was drawn
long long lastFrameUpdate;
long long lastDrop;

double FPS = 600; 	//Number AI moves and input-checks per second
int drawFPS = 60;	//Number of frames drawn per second
int dropDelay = 750000; // in microseconds

std::vector<ai*> players;


//Get the current time in microseconds
long long micros() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


void loop() {
	bool quit = false;
	SDL_Event e;
	while (!quit) {

		auto startTime = micros();
		if ((startTime - lastFrameUpdate) * drawFPS > 1000000) {
			gameWindow.drawScreen(games);
			lastFrameUpdate = startTime;
		}
		//Manage buttonpresses
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN ) {
				switch (e.key.keysym.sym) {
				case SDLK_SPACE:	
					for (tetris* g : games) g->init();
					break;
				case SDLK_l:
					FPS *= 1.3;
					break;
				case SDLK_k:
					FPS /= 1.3;
					break;
				}
			
			}
		}
		
		//Make moves
		for (int i = 0; i < games.size(); i++) {
			int m = players[i]->getNextMove(*games[i]);
			games[i]->makeMove(m);
			if (games[i]->isGameOver()) {
				games[i]->init();
			}
		}

		
		auto endTime = micros();
	
		if (endTime - startTime <= 1000000 / FPS) 
			SDL_Delay(1000 / FPS - (endTime - startTime)/1000);
	

	}
}


int main() {
	
	srand(time(0));
	gameWindow.init();
	games.push_back(new tetris());
	games.push_back(new tetris());
	games.push_back(new tetris());
	for (auto g : games) 
		g->init();


	lastFrameUpdate = micros();
	lastDrop = micros();

	//Instanciate AIs
	//The weights were found by an evolutionary algorithm.
	//The AIs are given in order of increasing generation.
	std::vector<long double> weights(4, -1);

	weights[0] = -0.515054;
	weights[1] = -0.155887;
	weights[2] = 0.149759;
	weights[3] = 0.179301;
	players.push_back(new linear_ai(weights));

	weights[0] = -0.717882;
	weights[1] = -0.0772711;
	weights[2] = 0.304969;
	weights[3] = -0.0770355;
	players.push_back(new linear_ai(weights));

	weights[0] = -0.525108;
	weights[1] = -0.0505178;
	weights[2] = 0.24386;
	weights[3] = -0.180518;
	players.push_back(new linear_ai(weights));



	loop();



	gameWindow.close();
	
	return 0;
}