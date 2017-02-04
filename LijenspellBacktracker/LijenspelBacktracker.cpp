// 
// LijenspelBacktracker.cpp : A backtracker for the puzzle Lijenspel (a.k.a Line Game)
// Rules and puzzles can be found at http://puzzlepicnic.com/genre?id=51.
// Author: Bryson McIver
// Created: 2/3/2017
// Last Update: 2/3/2017
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream> //Lots of IO
#include <search.h> //Quick sort
using namespace std;

//Useful globals
int numRows;
int numCols;

//Used to keep track of number locations in puzzle to speed up backtracking
struct number {
	int value, x, y;
};

//Global so we don't have to pass it
number* numbers;
int numNumbers;

//Keeps track of which number we are working on in the numbers array
int currentNumber = 0;

//Nice list of the arrow characters
const char arrows[4] = {'<', '>', '^', 'v'};

//Keeps track of found solution count
int numSolutions = 0;

//Checks if you have a solution to the puzzle
// param puzzle: The puzzle state to check for a solution
bool isSolved(char** puzzle) {

	return false;
}

//Checks if we are still working on a good puzzle
//Pruning techniques:
//	Make sure every number has space to expand to their needed size
//  Make sure every empty square can be reached
bool isValid(char** puzzle) {

	return false;
}

//The main backtracking function, handles creation of each step of a solution
// returns: a 3d array holding all possible solutions (hopefully one).
void backtracker(char*** solutions, char** puzzle) {
	//Do we have a solution? If so save it
	if (isSolved(puzzle)) {
		solutions[numSolutions] = puzzle;
		numSolutions++;
		return;
	}
	//Are we on the right track? If not, bail.
	else if (!isValid(puzzle)) {
		return;
	}
	//Keep going, generate next states
	else {

		return;
	}
}

//Will sort our numbers for us :)
//params: the two number structs to sort
int numbersSorter(const void* num1, const void* num2) {
	return ((number*)num1)->value > ((number*)num2)->value ? 1 : -1;
}

//Entry point for the backtracker, handles IO, printing, and starting the backtracking
int main() {

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

	//TODO: IO.
	
	//Define number of numbers, create list space for the struct
	numNumbers = 4;
	numbers = new number[4];

	numRows = 4;
	numCols = 4;

	//Hard code puzzle
	puzzle[1][0] = '1';
	numbers[0].value = 1;
	numbers[0].x = 1;
	numbers[0].y = 0;

	puzzle[0][2] = '3';
	numbers[1].value = 3;
	numbers[1].x = 0;
	numbers[1].y = 2;

	puzzle[2][3] = '4';
	numbers[2].value = 4;
	numbers[2].x = 2;
	numbers[2].y = 3;

	puzzle[3][1] = '4';
	numbers[3].value = 4;
	numbers[3].x = 3;
	numbers[3].y = 1;

	//Sort array of numbers so that we can just do them in order
	qsort(numbers, numNumbers, sizeof(number), numbersSorter);

	//Print OG Puzzle
	cout << "Lijenspel Backtracker" << endl << endl;
	cout << "Starting State:" << endl;

	//Print top
	for (int n = 0; n < numCols; n++) {
		cout << "____";
	}
	cout << endl;

	//Print grid
	for (int n = 0; n < numRows; n++) {
		cout << "| ";
		for (int m = 0; m < numCols; m++) {
			cout << puzzle[n][m] << " | ";
		}
		cout << endl;
	}

	//Print bottom
	for (int n = 0; n < numCols; n++) {
		cout << "____";
	}
	cout << endl << endl;

	//Call backtracker and let it return a 3d array of puzzle solutions 
	int numSolutions;
	char*** solutions = new char**;
	numSolutions = backtracker(solutions, puzzle);

	//No solution :(
	if (numSolutions == 0) {
		cout << "No Solutions for Provided State" << endl << endl;
	}
	
	//Print Solutions
	else {

		//Display Solutions Returned to console
		//Only does first solution

		cout << "Solutions:" << endl;

		//Print top
		for (int n = 0; n < numCols; n++) {
			cout << "____";
		}
		cout << endl;

		//Print grid
		for (int i = 0; i < numSolutions; i++) {
			for (int n = 0; n < numRows; n++) {
				cout << "| ";
				for (int m = 0; m < numCols; m++) {
					cout << solutions[i][n][m] << " | ";
				}
				cout << endl;
			}
		}

		//Print bottom
		for (int n = 0; n < numCols; n++) {
			cout << "____";
		}
		cout << endl;
		cout << endl;
	}
	
	//Wait for input to close
	system("pause");
	return 0;
}
