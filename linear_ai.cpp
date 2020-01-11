#include "linear_ai.h"
#include <iostream>
#include <map>
#include <bitset>

int linear_ai::calculateHeight(tetris& t) {
	int sum = 0;
	for (int j = 0; j < 10; j++) {
		for (int i = 2; i < 22; i++) {
			if (t.getBoard()[i][j]) {
				sum += 22 - i;
				break;
			}
		}
	}
	return sum;
}


int sqr(int x) { return x*x; }


int linear_ai::calculateBumpiness(tetris& t) {
	int sum = 0;
	int prevHeight = 0;
	for (int j = 0; j < 10; j++) {
		for (int i = 2; i < 22; i++) {
			if (t.getBoard()[i][j]) {
				if (j != 0)
					sum += sqr(prevHeight - (22 - i));
				prevHeight = 22 - i;
				break;
			}
			if (i == 21) {
				sum += sqr(prevHeight);
				prevHeight = 0;
			}
		}
	}
	return sum;
}


int linear_ai::calculateFullLines(tetris& t) {
	int numLines = 0;
	for (int i = 0; i < 22; i++) {
		int count = 0;
		for (int j = 0; j < 10; j++) {
			count += t.getBoard()[i][j] != 0;
		}
		numLines += count == 10;
	}
	return numLines;
}


int linear_ai::calculateHoles(tetris& t) {
	int numHoles = 0;
	for (int j = 0; j < 10; j++) {
		bool encounteredPeice = false;
		for (int i = 0; i < 22; i++) {
			encounteredPeice |= t.getBoard()[i][j];
			if (encounteredPeice && !t.getBoard()[i][j]) {
				numHoles++;
			}
		}
	}
	return numHoles;
}


long double linear_ai::calculateScore(tetris& t, int piece, int row, int col, int r) {
	//Set the board of t as if the piece actually occupies those spots
	//This breaks encapsulation, but it makes the calculations cleaner
	auto board = t.getBoard();
	auto cellPositions = t.getCellPositions(piece, r);
	
	for (int i = 0; i < 4; i++) {
		if (board[cellPositions[i][1] + row][cellPositions[i][0] + col]) std::cout << "Already occupied" << std::endl;
 		board[cellPositions[i][1]+row][cellPositions[i][0]+col] = piece;
	}
	long double score = (long double) calculateHeight(t) * weights[0]
					  + (long double) calculateBumpiness(t) * weights[1]
					  + (long double) calculateFullLines(t) * weights[2]
		              + (long double) calculateHoles(t) * weights[3];

	for (int i = 0; i < 4; i++) {
		board[cellPositions[i][1]+row][cellPositions[i][0]+col] = 0;
	}
	return score;
}


long double linear_ai::calculateScore(tetris& t, int row, int col, int r) {
	return calculateScore(t, t.getPiece(), row, col, r);
}


linear_ai::position linear_ai::nextPosition(position pos, int k) {
	if (k >= 3) {
		//Handle rotation
		//3 -> -1 (== 3 mod 4)
		//4 -> 1
		pos.rot += k == 3 ? 3 : 1;
		pos.rot %= 4;
	}
	else {
		//Handle movement
		pos.col += (k - 1) % 2;
		pos.row += (2 - k) % 2;
	}
	return pos;
}

int linear_ai::hash(position &p) {
	return (p.row+5) + (p.col+5)*30 + p.rot*30*30;
}


//Fill the path stack 
//Possibly don't recreate all the arrays every time the function is called. Only visited needs to be emptied.
void linear_ai::fillMoveStack(tetris& t) {
	//Find a shortest path to every possible ending position using a breadth first search.

	std::bitset<4000> visited; visited.reset();
	std::vector<position> frontier;

	position start; 
	start.row = t.getPieceRow(); 
	start.col = t.getPieceCol();
	start.rot = t.getRotation();
	
	int startHash = hash(start);
	frontier.push_back(start);
	parent[startHash] = start;
	visited[startHash] = true;

	long double bestScore = -1e9;
	position bestPos = start;
	
	while (!frontier.size() == 0) {
		std::vector<position> next;

		for (auto pos : frontier) {
			//Add unvisited neighbours to the next frontier
			for (int k = 0; k < 4; k++) {
				auto n = nextPosition(pos, k);
				int h = hash(n);
				if (!visited[h] && t.isValidPosition(n.col, n.row, n.rot)) {
					next.push_back(n);
					visited[h] = true;
					parent[h] = pos;
					button[h] = k;
				}
			}
			//Check whether this can be a terminal position
			auto down = nextPosition(pos, 1);
			if (!t.isValidPosition(down.col, down.row, down.rot)) {
				long double score = calculateScore(t, pos.row, pos.col, pos.rot);
				int downHash = hash(down);
				if (score > bestScore) {
					bestScore = score;
					bestPos = down;
					parent[downHash] = pos;
					button[downHash] = 1;
				}
			}
		}
		
		frontier.swap(next);
	}

	//Fill the move stack with the moves that lead to the best position 
	int bestPosHash = hash(bestPos);
	while (parent[bestPosHash] != bestPos) {
		path.push(button[bestPosHash]);
		bestPos = parent[bestPosHash];
		bestPosHash = hash(bestPos);
	}
}


int linear_ai::getNextMove(tetris& t) {
	if (path.empty()) fillMoveStack(t);
	int m = path.top(); path.pop();
	return m;

}