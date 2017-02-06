// 
// LijenspelBacktracker.cpp : A backtracker for the puzzle Lijenspel (a.k.a Line Game)
// Rules and puzzles can be found at http://puzzlepicnic.com/genre?id=51.
// Author: Bryson McIver
// Created: 2/3/2017
// Last Update: 2/3/2017
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream> //Lots of IO
#include <string>
#include <algorithm>
#include <search.h> //Quick sort
#include <fstream> //File streams
#include <vector> //Basically a dynamic array
using namespace std;

//Useful globals
int numRows;
int numCols;

//Used to keep track of number locations in puzzle to speed up backtracking
struct number {
	int value, row, col, remaining;
};

//Global so we don't have to pass it
vector<number> numbers;
int numNumbers;

//Keeps track of which number we are working on in the numbers array
int currentNumber = 0;

//Nice list of the arrow characters
const char arrows[4] = {'<', '>', '^', 'v'};

//Keeps track of found solution count
int numSolutions = 0;

//Prints a single representation of the puzzle
void printPuzzle(char** puzzle) {
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
}

//Checks if you have a solution to the puzzle
// param puzzle: The puzzle state to check for a solution
bool isSolved(char** puzzle) {
	//Check all our numbers have their arrows
	for (int i = 0; i < numNumbers; i++) {
		if (numbers[i].remaining != 0) {
			return false;
		}
	}
	//Check all squares are filled
	for (int n = 0; n < numRows; n++) {
		for (int m = 0; m < numCols; m++) {
			//Check if it is an arrow or a number, otherwise blank
			if (!(isdigit(puzzle[n][m])) && puzzle[n][m] != '<' && puzzle[n][m] != '^' && puzzle[n][m] != 'v' && puzzle[n][m] != '>') {
				return false;
			}
		}
	}
	//All squares filled, all numbers have their arrows.
	return true;
}

//Checks if we are still working on a good puzzle
//Pruning techniques:
//	Make sure every number has space to expand to their needed size
//  Make sure every empty square can be reached
bool isValid(char** puzzle) {
	//First Check Each Number Can Expand
	int curRemain, row, col;
	for (int i = 0; i < numNumbers; i++) {
		//Gather current info
		curRemain = numbers[i].remaining;
		row = numbers[i].row;
		col = numbers[i].col;
		//Look in each diretion and see if we can get curRemain to 0
		//Up
		for (int j = row - 1; j >= 0; j--) {
			if (puzzle[j][col] != '^') {
				//Is it empty? Then we could fill it.
				if (puzzle[j][col] == ' ') {
					curRemain--;
				}
				//Otherwise this direction is done
				else {
					break;
				}
			}
		}
		//Down
		for (int j = row + 1; j < numRows; j++) {
			if (puzzle[j][col] != 'v') {
				//Is it empty? Then we could fill it.
				if (puzzle[j][col] == ' ') {
					curRemain--;
				}
				//Otherwise this direction is done
				else {
					break;
				}
			}
		}
		//Right
		for (int j = col + 1; j < numCols; j++) {
			if (puzzle[row][j] != '>') {
				//Is it empty? Then we could fill it.
				if (puzzle[row][j] == ' ') {
					curRemain--;
				}
				//Otherwise this direction is done
				else {
					break;
				}
			}
		}
		//Left
		for (int j = col - 1; j >= 0; j--) {
			if (puzzle[row][j] != '<') {
				//Is it empty? Then we could fill it.
				if (puzzle[row][j] == ' ') {
					curRemain--;
				}
				//Otherwise this direction is done
				else {
					break;
				}
			}
		}
		//Did it have space?
		if (curRemain > 0) {
			return false;
		}
	}

	//Also check if all the spaces can be reached

	return true;
}

//The main backtracking function, handles creation of each step of a solution
// returns: a 3d array holding all possible solutions (hopefully one).
void backtracker(char*** solutions, char** puzzleState) {
	
	//Deep copy our current state
	char** puzzle;
	puzzle = new char *[numCols];

	//Create and intialize puzzle
	for (int n = 0; n < numCols; n++) {
		puzzle[n] = new char[numRows];
		for (int m = 0; m < numRows; m++) {
			puzzle[n][m] = puzzleState[n][m];
		}
	}

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
		//Are we done with the current number?
		bool incremented = false;
		if (numbers[currentNumber].remaining == 0) {
			currentNumber++;
			incremented = true;
			if (currentNumber == numNumbers) {
				//I don't think we will ever get here
				currentNumber = 0;
			}
		}

		//Recursively call the new arrows we can
		numbers[currentNumber].remaining--;
		int row = numbers[currentNumber].row;
		int col = numbers[currentNumber].col;
		//Up
		for (int i = row-1; i >= 0; i--) {
			if (puzzle[i][col] != '^') {
				//Is it empty? Then we can fill.
				if (puzzle[i][col] == ' ') {
					puzzle[i][col] = '^';
					backtracker(solutions, puzzle);
					//Reset the space when we don't want it set
					puzzle[i][col] = ' ';
					if (numSolutions > 0) {
						return;
					}
				}
				//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
				break;
			}
		}
		//Right
		for (int i = col + 1; i < numCols; i++) {
			if (puzzle[row][i] != '>') {
				//Is it empty? Then we can fill.
				if (puzzle[row][i] == ' ') {
					puzzle[row][i] = '>';
					backtracker(solutions, puzzle);
					//Reset this space
					puzzle[row][i] = ' ';
					if (numSolutions > 0) {
						return;
					}
				}
				//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
				break;
			}
		}
		//Down
		for (int i = row+1; i < numRows; i++) {
			if (puzzle[i][col] != 'v') {
				//Is it empty? Then we can fill.
				if (puzzle[i][col] == ' ') {
					puzzle[i][col] = 'v';
					backtracker(solutions, puzzle);
					//Reset this space
					puzzle[i][col] = ' ';
					if (numSolutions > 0) {
						return;
					}
				}
				//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
				break;
			}
		}
		//Left
		for (int i = col - 1; i >= 0; i--) {
			if (puzzle[row][i] != '<') {
				//Is it empty? Then we can fill.
				if (puzzle[row][i] == ' ') {
					puzzle[row][i] = '<';
					backtracker(solutions, puzzle);
					//Reset space
					puzzle[row][i] = ' ';
					if (numSolutions > 0) {
						return;
					}
				}
				//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
				break;
			}
		}

		//If we came back we need to give it its remaining back and go back to our old current number
		numbers[currentNumber].remaining++;
		if (incremented) {
			currentNumber--;
		}
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

	//Open File For Input
	//First line must be ROW COL
	//Following lines will be x's representing spaces and appropriate numbers.
	ifstream file ("puzzle.txt");
	string buffer;
	getline(file, buffer);
	numRows = buffer[0];
	numCols = buffer[2];

	//Define Puzzle pointer
	char** puzzle;
	puzzle = new char *[numRows];

	//Create and intialize puzzle
	for (int n = 0; n < numRows; n++) {
		puzzle[n] = new char[numCols];
		getline(file, buffer);
		for (int m = 0; m < numCols; m++) {
			if (buffer[m] == 'x') {
				puzzle[n][m] = ' ';
			}
			else {
				puzzle[n][m] = buffer[m];
				numbers.push_back(number());
				numbers[numNumbers].row = n;
				numbers[numNumbers].col = m;
				numbers[numNumbers].value = buffer[m] - '0';
				numbers[numNumbers].remaining = numbers[numNumbers].value;
				numNumbers++;
			}
		}
	}

	//Sort array of numbers so that we can just do them in order
	sort(numbers.begin(), numbers.end(), numbersSorter);

	//Print OG Puzzle
	cout << "Lijenspel Backtracker" << endl << endl;
	cout << "Starting State:" << endl;

	printPuzzle(puzzle);

	//Call backtracker and let it return a 3d array of puzzle solutions 
	char*** solutions = new char**;
	backtracker(solutions, puzzle);

	//No solution :(
	if (numSolutions == 0) {
		cout << "No Solutions for Provided State" << endl << endl;
	}
	
	//Print Solutions
	else {

		//Display Solutions Returned to console
		//Only does first solution

		cout << "Solutions:" << endl;
		printPuzzle(solutions[numSolutions-1]);

	}
	
	//Wait for input to close
	system("pause");
	return 0;
}
