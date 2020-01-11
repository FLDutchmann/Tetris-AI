#pragma once
#include "tetris.h"

class ai {
public:
	//Dummy definition, to be overwritten by derived classes
	virtual int getNextMove(tetris&) { return -1; };
};