#pragma once
#include "ai.h"
#include <stack>

class linear_ai : public ai {
private:

	//Height, bumpiness, full lines, holes
	std::vector<long double> weights;

	// The features outlined in the report
	int calculateHeight(tetris&);
	int calculateBumpiness(tetris&);
	int calculateFullLines(tetris&);
	int calculateHoles(tetris&);

	//Scores the state of t using the specified tetromino in the specified position.
	long double calculateScore(tetris& t, int piece, int row, int col, int r) ;
	//Scores the state of t using the current tetromino in the specified position.
	long double calculateScore(tetris& t, int row, int col, int r) ;

	//Calculates path to the optimal location for falling piece and stores it in the path stack.
	void fillMoveStack(tetris&);

	//Stores the buttons that need to be pressed to land the piece in the optimal location.
	std::stack<int> path;
	
	//Wrapper to store the location and rotation of a tetris piece.
	struct position {
		int row, col, rot;
		bool operator!= (position &a) {
			return !(a.row == row && a.col == col && a.rot == rot);
		}
		friend bool operator< (const position &a, const position &b) {
			if (a.row < b.row) return true;
			if (b.row < a.row) return false;
			if (a.col < b.col) return true;
			if (b.col < a.col) return false;
			if (a.rot < b.rot) return true;
			if (b.rot < a.rot) return false;
			return false;
		}
	};

	//The integer represents a button press, with the same encoding as the tetris class:
	//0 = move left; 1 = move down; 2 = move right; 3 = rotate right; 4 = rotate left.

	position nextPosition(position, int);
	int hash(position&);
	std::vector<position> parent;
	std::vector<int> button;


public:
	linear_ai(std::vector<long double> weights) { 
		this->weights = weights; 
		button.assign(4000, 0);
		position p;
		parent.assign(4000, p);
	};


	int	getNextMove(tetris&);
	void resetQueue();
	
};