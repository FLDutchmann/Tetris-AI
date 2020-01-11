#pragma once
#include <queue>

const int BOARD_HEIGHT = 22;
const int BOARD_WIDTH = 10;

class tetris {
private:
	int board[BOARD_HEIGHT][BOARD_WIDTH];

	//Information about the falling piece
	int piece;
	int rotation;
	int pieceRow;
	int pieceCol;
	
	int score;
	bool gameOver;
	std::queue<int> pieceQueue;
	void refillQueue();
	void initializeNextPiece();
	static int cellPositions[8][4][4][2];
	//Returns the next piece and updates the queue
	int fetchNextPiece();

	//Returns the number of full rows removed
	int removeFullRows();
	void removeRow(int row);

	//Returns false if there was no place for the new piece
	bool materializePiece();

public:
	tetris();

	//Returns a pointer to the current board.
	auto getBoard() { return board; }
	int getPiece() { return piece; }
	int getRotation() { return rotation; }
	int getPieceRow() { return pieceRow; }
	int getPieceCol() { return pieceCol; }
	int getScore() { return score; }
	int getNextPiece() { return pieceQueue.front(); }
	bool isGameOver() { return gameOver; }
	bool isValidPosition(int x, int y, int r);
	//Returns an array of the positions of the cells of the current
	//tetromino, relative to its position.
	auto getCellPositions() { return cellPositions[piece][rotation]; }
	auto getCellPositions(int piece, int rotation) { return cellPositions[piece][rotation]; }

	//Applies the outcome of a buttonpress to the board. If the move did not result in a change, the function returns 1, else it returns 0.
	//Note that pressing down always changes the state of the board.
	//0 = move left; 1 = move down; 2 = move right; 3 = rotate right; 4 = rotate left.
	//That way left and right % 2 == 0.
	int makeMove(int move);

	void init();
};