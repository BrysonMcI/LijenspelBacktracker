// 
// LijenspelBacktracker.cpp : A backtracker for the puzzle Lijenspel (a.k.a Line Game)
// Rules and puzzles can be found at http://puzzlepicnic.com/genre?id=51.
// Author: Bryson McIver
// Created: 2/3/2017
// Last Update: 2/3/2017
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;

//The main backtracking function, handles creation of each step of a solution
// returns: a 3d array holding all possible solutions (hopefully one).
char*** backtracker(char** puzzle) {
	char*** solutions;
	solutions = new char**;
	solutions[0] = puzzle;

	return solutions;
}

//Entry point for the backtracker, handles IO, printing, and starting the backtracking
int main() {
	//TODO: IO.

	int numRows = 4;
	int numCols = 4;

	//Define Puzzle pointer
	char** puzzle;
	puzzle = new char *[numRows];

	//Create and intialize puzzle
	for (int n = 0; n < numRows; n++) {
		puzzle[n] = new char[numCols];
		for (int m = 0; m < numCols; m++) {
			puzzle[n][m] = ' ';
		}
	}

	//Hard code puzzle
	puzzle[1][0] = '1';
	puzzle[0][2] = '3';
	puzzle[2][3] = '4';
	puzzle[3][1] = '4';

	//Call backtracker and let it return a 3d array of puzzle solutions 
	char*** solutions;
	solutions = backtracker(puzzle);
	
	//TODO: Check if no solutions returned

	//Display Solutions Returned to console
	//Only does first solution

	//Print top
	for (int n = 0; n < numCols; n++) {
		cout << "__";
	}
	cout << endl;

	//Print grid
	for (int n = 0; n < numRows; n++) {
		cout << "| ";
		for (int m = 0; m < numCols; m++) {
			cout << solutions[0][n][m] << " | ";
		}
		cout << endl;
	}

	//Print bottom
	for (int n = 0; n < numCols; n++) {
		cout << "__";
	}
	cout << endl;

	return 0;
}
