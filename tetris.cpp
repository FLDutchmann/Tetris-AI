#include "tetris.h"
#include <algorithm>
#include <iostream>

int shuffleList[] = { 1, 2, 3, 4, 5, 6, 7 };

//Gives the locations of the filled cells for each piece and rotation, 
//relative to the position of the piece 
// [piece][rotation][cell][dx or dy]
int tetris::cellPositions[8][4][4][2] = {
	{		// Empty piece. Normally unused, but this makes the IDs line up better
		{ { 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 } },
		{ { 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 } },
		{ { 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 } },
		{ { 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 } }
	},
	{		// I piece Possile change: The piece doesn't do a wallkick properly if it is rotated incorrectly
		{ { -1, 0 },{ 0, 0 },{ 1, 0 },{ 2, 0 } },
		{ {  1,-1 },{ 1, 0 },{ 1, 1 },{ 1, 2 } },
		{ { -1, 1 },{ 0, 1 },{ 1, 1 },{ 2, 1 } },
		{ {  0,-1 },{ 0, 0 },{ 0, 1 },{ 0, 2 } }
	},
	{		// O piece
		{ { 0, 0 },{ 1, 0 },{ 0, -1 },{ 1, -1 } },
		{ { 0, 0 },{ 1, 0 },{ 0, -1 },{ 1, -1 } },
		{ { 0, 0 },{ 1, 0 },{ 0, -1 },{ 1, -1 } },
		{ { 0, 0 },{ 1, 0 },{ 0, -1 },{ 1, -1 } }
	},
	{		// T piece
		{ { -1, 0 },{ 0, 0 },{ 0, -1 },{ 1, 0 } },
		{ { 0, -1 },{ 0, 0 },{ 1, 0 },{ 0, 1 } },
		{ { 1, 0 },{ 0, 0 },{ 0, 1 },{ -1, 0 } },
		{ { 0, 1 },{ 0, 0 },{ -1, 0 },{ 0, -1 } }
	},
	{		// J piece
		{ { -1, -1 },{ -1, 0 },{ 0, 0 },{ 1, 0 } },
		{ { 1, -1 },{ 0, -1 },{ 0, 0 },{ 0, 1 } },
		{ { 1, 1 },{ 1, 0 },{ 0, 0 },{ -1, 0 } },
		{ { -1, 1 },{ 0, 1 },{ 0, 0 },{ 0, -1 } }
	},
	{		// L piece
		{ { -1, 0 },{ 0, 0 },{ 1, 0 },{ 1, -1 } },
		{ { 0, -1 },{ 0, 0 },{ 0, 1 },{ 1, 1 } },
		{ { 1, 0 },{ 0, 0 },{ -1, 0 },{ -1, 1 } },
		{ { 0, 1 },{ 0, 0 },{ 0, -1 },{ -1, -1 } }
	},
	{		// S piece
		{ { -1, 0 },{ 0, 0 },{ 0, -1 },{ 1, -1 } },
		{ { 0, -1 },{ 0, 0 },{ 1, 0 },{ 1, 1 } },
		{ { 1, 0 },{ 0, 0 },{ 0, 1 },{ -1, 1 } },
		{ { 0, 1 },{ 0, 0 },{ -1, 0 },{ -1, -1 } }
	},
	{		// Z piece
		{ { -1, -1 },{ 0, -1 },{ 0, 0 },{ 1, 0 } },
		{ { 1, -1 },{ 1, 0 },{ 0, 0 },{ 0, 1 } },
		{ { 1, 1 },{ 0, 1 },{ 0, 0 },{ -1, 0 } },
		{ { -1, 1 },{ -1, 0 },{ 0, 0 },{ 0, -1 } }
	}
};


void tetris::refillQueue() {
	std::random_shuffle(&shuffleList[0], &shuffleList[7]);
	for (int i = 0; i < 7; i++) {
		pieceQueue.push(shuffleList[i]);
	}
}


//srand needs to be seeded for the pieces to show up in completely random order.
//This is not done in the tetris class because this is speed-critical code.
tetris::tetris() {
	
}


//Pieces are drawn from a bag without replacement. The bag is refilled with one
//of each piece whenever it is empty. This prevents long chains of a single piece.
int tetris::fetchNextPiece() {
	int r = pieceQueue.front();
	pieceQueue.pop();
	if (pieceQueue.empty()) {
		refillQueue();
	}
	return r;
}


void tetris::init() {
	gameOver = false;

	//Reset the bag
	std::sort(&shuffleList[0], &shuffleList[7]);
	while (!pieceQueue.empty()) {
		pieceQueue.pop();
	}
	refillQueue();


	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			board[i][j] = 0;
		}
	}
	piece = pieceQueue.front();
	pieceQueue.pop();
	score = 0;
	rotation = 0;
	piece = 0;
	pieceCol = 0;
	//If row < 2 initializeNextPiece will register a game over and stop
	//the initialization early.
	pieceRow = 2;
	initializeNextPiece();
}

//Returns false iff the falling piece overlaps a full cell.
bool tetris::isValidPosition(int x, int y, int r) {
	bool valid = true;
	auto cells = getCellPositions(piece, r);

	for (int i = 0; i < 4; i++) {
		int tx = x + cells[i][0];
		int ty = y + cells[i][1];
		if (tx < 0 || tx >= BOARD_WIDTH || ty < 0 || ty >= BOARD_HEIGHT
	     || board[ty][tx] != 0) {
			return false;
		}
	}

	return true;
}


//Returns whether the current piece causes a game over
bool tetris::materializePiece() {
	auto cells = getCellPositions();
	bool gameOver = false;

	for (int i = 0; i < 4; i++) {
		int x = pieceCol + cells[i][0];
		int y = pieceRow + cells[i][1];
		if (y >= 2)
			board[y][x] = piece;
		else gameOver = true;

	}
	return gameOver;

}


void tetris::initializeNextPiece() {
	pieceRow = 1;
	pieceCol = 4;
	piece = fetchNextPiece();
	rotation = 0;
}


void tetris::removeRow(int row) {
	for (int i = row; i > 0; i--) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			board[i][j] = board[i - 1][j];
		}
	}
	//This code is probably unnecessary, since the top row should always
	//empty, but it is left here just in case
	for (int j = 0; j < BOARD_WIDTH; j++) {
		board[0][j] = 0;
	}
}

//Returns the number of removed rows
int tetris::removeFullRows() { 
	int numRows = 0;
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		bool full = true;
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (board[i][j] == 0) {
				full = false;
			}
		}
		if (full) {
			numRows++;
			removeRow(i);
		}
	}
	return numRows;
}


//Returns 1 if a piece was materialised, 0 otherwise
int tetris::makeMove(int k) {
	if (gameOver) return 0;
	int col = pieceCol;
	int row = pieceRow;
	int r = rotation;

	if (k >= 3) { //Rotate
		//3 -> -1 (== 3 mod 4)
		//4 -> 1
		r += k == 3 ? 3 : 1;
		r %= 4;
	}
	else { //Move left, right or down
		col += (k - 1) % 2;
		row += (2 - k) % 2;
	}
	if (isValidPosition(col, row, r)) {
		pieceCol = col;
		pieceRow = row;
		rotation = r;
		return 0;
	}
	else if (k == 1) { //Moving down into an invalid position.

		if (materializePiece()) {
			gameOver = true;
			return 0;
		}
		initializeNextPiece();
		
		//removeFullRows returns the amount of rows removed without altering the score
		int s = removeFullRows(); 
		score += 100 * s;
		return 1;
	}

	return 0;
}