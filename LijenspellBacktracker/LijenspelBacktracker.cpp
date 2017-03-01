// 
// LijenspelBacktracker.cpp : A backtracker for the puzzle Lijenspel (a.k.a Line Game)
// Rules and puzzles can be found at http://puzzlepicnic.com/genre?id=51.
// Author: Bryson McIver
// Created: 2/3/2017
// Last Update: 2/28/2017
//
//	Outputs solution to solution.txt and a fresh clean board at original.txt
//
//  10% Sure Puzzle Creation takes up more memory than it really needs, working on it.
//  Puzzle creation now works though! Supports partially made puzzles! Finishes creation for you! Create from scratch!
//  Run with puzzle.txt in the normal format, but put all x in for the grid (or a partial creation) and select option 4 in the menu.
//  Example currently in puzzle.txt
//
//  Note: Creation can take a long time, there's a lot of branching, some of which is pretty random. 6x6 seems to take anywhere between 1 and 10 minutes.
//
// Aditional Details:
//  Only supports single digit numbers in the grid.
//  A 9 9 puzzle considered hard took about 5 seconds on my i7 4770. Woo!
//  Supports solving from partial solutions/checking if you can still solve based on current state
//  Little home made rating system, step by step, and until next guess modes made for creating puzzles (by hand)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream> //Lots of IO
#include <string>
#include <algorithm>
#include <fstream> //File streams
#include <vector> //Basically a dynamic array
#include <iomanip> //setprecision
#include <math.h>
using namespace std;

//Useful globals
int numRows;
int numCols;

//Used to keep track of number locations in puzzle to speed up backtracking
//curDir is a way to not redo directions and configurations that had already been completed.
//	It is used as an index to the char array of arrows
struct number {
	int value, row, col, remaining, curDir;
};

//Global so we don't have to pass it
vector<number> numbers;
int numNumbers;

//Keeps track of what operating mode we want
int mode;

//Difficulty is rated based on size, number of numbers, and number of guesses.
double difficultyRate;

//Keeps track of which number we are working on in the numbers array
int currentNumber = 0;

//Nice list of the arrow characters
const char arrows[4] = {'^', '>', 'v', '<'};

//Checks if two puzzle boards are equivilant
bool checkEquals(char** puzzleOne, char** puzzleTwo) {
	//Loop through puzzles and compare characters
	for (int k = 0; k < numRows; k++) {
		for (int l = 0; l < numCols; l++) {
			if (puzzleOne[k][l] != puzzleTwo[k][l]) {
				return false;
			}
		}
	}
	return true;
}

//Will clean up our solutions vector
void deleteDuplicates(vector<char**> &solutions) {
	
	//Loop through vectors, checking each vector past it against it. Delete duplicates.
	for (int i = 0; i < solutions.size(); i++) {
		for (int j = i + 1; j < solutions.size(); j++) {	
			if (checkEquals(solutions[i], solutions[j])) {
				//Erase duplicate
				solutions.erase(solutions.begin() + j);
			}
		}
	}
}

//Will sort our numbers for us :)
//params: the two number structs to sort
int numbersSorter(number num1, number num2) {
	return (num1.value < num2.value);
}

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
		for (int n = 0; n < numCols; n++) {
			cout << "|___";
		}
		cout << "|" << endl;
	}
	cout << endl;
}

//Prints the squares array used in backtracking
void printSquares(int** puzzle) {
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
		for (int n = 0; n < numCols; n++) {
			cout << "|___";
		}
		cout << "|" << endl;
	}
	cout << endl;
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

	return true;
}

void updateRemaining(char** puzzle) {
	//Loops through numbers and changes their remaining based on the arrows they are connnect to
	int row, col, val, found;
	
	for (int i = 0; i < numNumbers; i++) {
		//Loop all 4 directions counting remaining
		row = numbers[i].row;
		col = numbers[i].col;
		val = numbers[i].value;
		found = 0;
		//Look in each diretion and see if we can get curRemain to 0
		//Up
		for (int j = row - 1; j >= 0; j--) {
			if (puzzle[j][col] != '^') {
					break;
			}
			else {
				found++;
			}
		}
		//Down
		for (int j = row + 1; j < numRows; j++) {
			if (puzzle[j][col] != 'v') {
				break;
			}
			else {
				found++;
			}
		}
		//Right
		for (int j = col + 1; j < numCols; j++) {
			if (puzzle[row][j] != '>') {
				break;
			}
			else {
				found++;
			}
		}
		//Left
		for (int j = col - 1; j >= 0; j--) {
			if (puzzle[row][j] != '<') {
				break;
			}
			else {
				found++;
			}
		}
		
		//Update remaining
		numbers[i].remaining = val - found;

	}

	//Re-sort based on remaining
	sort(numbers.begin(), numbers.end(), numbersSorter);

}

//The main backtracking function, handles creation of each step of a solution
// returns: a 3d vector holding all possible solutions (hopefully one).
void backtracker(vector<char**> &solutions, char** puzzleState) {
	
	if (mode == 3) {
		cout << "Current State" << endl;
		printPuzzle(puzzleState);
		system("pause");
	}

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

	//Update remaining values of puzzle.
	updateRemaining(puzzle);

	//Not valid, don't continue
	if (!isValid(puzzle)) {
		for (int n = 0; n < numCols; n++) {
			delete[]puzzle[n];
		}
		delete[]puzzle;
		return;
	}
	//Fill squares that only have one option and check if invalid due to squares not having any options
	//Do we have a solution? If so save it
	if (isSolved(puzzle)) {
		mode = 1;
		solutions.push_back(puzzle);
		return;
	}
	//Keep going, generate next states
	else {
		//Need a duplicate sized grid of ints
		int** squares;
		squares = new int *[numCols];
		int* spaceUsed = new int[numNumbers];

		for (int i = 0; i < numNumbers; i++) {
			spaceUsed[i] = 0;
		}

		//Create and intialize our new grid
		for (int n = 0; n < numCols; n++) {
			squares[n] = new int[numRows];
			for (int m = 0; m < numRows; m++) {
				if (puzzle[n][m] == '^') {
					squares[n][m] = 0;
				}
				else if (puzzle[n][m] == '>') {
					squares[n][m] = 1;
				}
				else if (puzzle[n][m] == 'v') {
					squares[n][m] = 2;
				}
				else if (puzzle[n][m] == '<') {
					squares[n][m] = 3;
				}
				else {
					squares[n][m] = -1;
				}
			}
		}

		//Now loop through all the numbers, if a square can be touched it gets the corresponding index of the arrow
		// it would have in the arrows array.
		// If it stays at -1, we know we have an invalid 
		// 4 Means a number is there
		// If it is a 5, then it has multiple options and will be ignored
		// In the end numbers will be filled in with their arrows on the puzzle board
		for (int k = 0; k < numNumbers; k++) {
			int curRow = numbers[k].row;
			int curCol = numbers[k].col;
			squares[curRow][curCol] = 4;
			//Loop in all directions setting numbers;
			//Up
			int remaining = numbers[k].remaining;
			for (int i = curRow - 1; i >= 0; i--) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (puzzle[i][curCol] != ' ' && puzzle[i][curCol] != '^') {
					//Not empty, not right direction, dead end
					break;
				}
				else if (puzzle[i][curCol] == ' ') {
					if (squares[i][curCol] == -1) {
						//We can set this square
						squares[i][curCol] = 0;
					}
					else {
						//Someone else has already been here
						squares[i][curCol] = 5;
					}
				}
				else {
					//This square was already taken by a good arrow and doesn't cost us.
					remaining++;
				}
			}
			//Right
			remaining = numbers[k].remaining;
			for (int i = curCol + 1; i < numCols; i++) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (puzzle[curRow][i] != ' ' && puzzle[curRow][i] != '>') {
					//Not empty, not right direction, dead end
					break;
				}
				else if (puzzle[curRow][i] == ' ') {
					if (squares[curRow][i] == -1) {
						//We can set this square
						squares[curRow][i] = 1;
					}
					else {
						//Someone else has already been here
						squares[curRow][i] = 5;
						//We don't stop because a farther square could matter more.
					}
				}
				else {
					//This square was already taken by a good arrow and doesn't cost us.
					remaining++;
				}
			}
			//Down
			remaining = numbers[k].remaining;
			for (int i = curRow + 1; i < numRows; i++) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (puzzle[i][curCol] != ' ' && puzzle[i][curCol] != 'v') {
					//Not empty, not right direction, dead end
					break;
				}
				else if (puzzle[i][curCol] == ' ') {
					if (squares[i][curCol] == -1) {
						//We can set this square
						squares[i][curCol] = 2;
					}
					else {
						//Someone else has already been here
						squares[i][curCol] = 5;
					}
				}
				else {
					//This square was already taken by a good arrow and doesn't cost us.
					remaining++;
				}
			}
			//Left
			remaining = numbers[k].remaining;
			for (int i = curCol - 1; i >= 0; i--) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (puzzle[curRow][i] != ' ' && puzzle[curRow][i] != '<') {
					//Not empty, not right direction, dead end
					break;
				}
				else if (puzzle[curRow][i] == ' ') {
					if (squares[curRow][i] == -1) {
						//We can set this square
						squares[curRow][i] = 3;
					}
					else {
						//Someone else has already been here
						squares[curRow][i] = 5;
						//We don't stop because a farther square could matter more.
					}
				}
				else {
					//This square was already taken by a good arrow and doesn't cost us.
					remaining++;
				}
			}
		}

		//Check to see if a space can't be reached (Essentially another is valid check with a new criteria)
		for (int m = 0; m < numRows; m++) {
			for (int n = 0; n < numCols; n++) {
				if (squares[m][n] == -1) {
					for (int n = 0; n < numCols; n++) {
						delete[]squares[n];
					}
					delete[]squares;
					delete[]spaceUsed;
					for (int n = 0; n < numCols; n++) {
						delete[]puzzle[n];
					}
					delete[]puzzle;
					return;
				}
			}
		}

		//now we go number by number and see if it has a square it sohuld extend to and make sure it has the remaining to do so
		for (int k = 0; k < numNumbers; k++) {
			int curRow = numbers[k].row;
			int curCol = numbers[k].col;
			//Loop in all directions setting numbers;
			//Up
			int remaining = numbers[k].remaining;
			for (int i = curRow - 1; i >= 0; i--) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (squares[i][curCol] == 0) {
					//Loop back and drop arrows
					for (int j = i; j <= curRow - 1; j++) {				
						if (puzzle[j][curCol] == '^') {
							break;
						}
						else if (puzzle[j][curCol] != ' ') {
							for (int l = 0; l < numNumbers; l++) {
								numbers[l].remaining += spaceUsed[l];
							}
							for (int n = 0; n < numCols; n++) {
								delete[]puzzle[n];
							}
							delete[]puzzle;
							for (int n = 0; n < numCols; n++) {
								delete[]squares[n];
							}
							delete[]squares;
							delete[]spaceUsed;
							return;
						}
						spaceUsed[k]++;
						puzzle[j][curCol] = '^';
					}
				}
			}
			//Right
			remaining = numbers[k].remaining;
			for (int i = curCol + 1; i < numCols; i++) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (squares[curRow][i] == 1) {
					//Loop back and drop arrows
					for (int j = i; j >= curCol + 1; j--) {
						if (puzzle[curRow][j] == '>') {
							break;
						}
						else if (puzzle[curRow][j] != ' ') {
							for (int l = 0; l < numNumbers; l++) {
								numbers[l].remaining += spaceUsed[l];
							}
							for (int n = 0; n < numCols; n++) {
								delete[]puzzle[n];
							}
							delete[]puzzle;
							for (int n = 0; n < numCols; n++) {
								delete[]squares[n];
							}
							delete[]squares;
							delete[]spaceUsed;
							return;
						}
						puzzle[curRow][j] = '>';
						spaceUsed[k]++;
					}
				}
			}
			//Down
			remaining = numbers[k].remaining;
			for (int i = curRow + 1; i < numRows; i++) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (squares[i][curCol] == 2) {
					//Loop back and drop arrows
					for (int j = i; j >= curRow + 1; j--) {
						if (puzzle[j][curCol] == 'v') {
							break;
						}
						if (puzzle[j][curCol] != ' ') {
							for (int l = 0; l < numNumbers; l++) {
								numbers[l].remaining += spaceUsed[l];
							}
							for (int n = 0; n < numCols; n++) {
								delete[]puzzle[n];
							}
							delete[]puzzle;
							for (int n = 0; n < numCols; n++) {
								delete[]squares[n];
							}
							delete[]squares;
							delete[]spaceUsed;
							return;
						}
						puzzle[j][curCol] = 'v';
						spaceUsed[k]++;
					}
				}
			}
			//Left
			remaining = numbers[k].remaining;
			for (int i = curCol - 1; i >= 0; i--) {
				remaining--;
				//This number can't reach in this direction any longer
				if (remaining < 0) {
					break;
				}
				if (squares[curRow][i] == 3) {
					//Loop back and drop arrows
					for (int j = i; j <= curCol - 1; j++) {
						if (puzzle[curRow][j] == '<') {
							break;
						}
						else if (puzzle[curRow][j] != ' ') {
							for (int l = 0; l < numNumbers; l++) {
								numbers[l].remaining += spaceUsed[l];
							}
							for (int n = 0; n < numCols; n++) {
								delete[]puzzle[n];
							}
							delete[]puzzle;
							for (int n = 0; n < numCols; n++) {
								delete[]squares[n];
							}
							delete[]squares;
							delete[]spaceUsed;
							return;
						}
						puzzle[curRow][j] = '<';
						spaceUsed[k]++;
					}
				}
			}

			numbers[k].remaining -= spaceUsed[k];

			//Someone used too many spaces :(
			if (numbers[k].remaining < 0) {
				//Multiple spaces were only accessable by this number, and couldn't all be serviced by it.
				for (int i = 0; i < numNumbers; i++) {
					numbers[i].remaining += spaceUsed[i];
				}
				for (int n = 0; n < numCols; n++) {
					delete[]puzzle[n];
				}
				delete[]puzzle;
				for (int n = 0; n < numCols; n++) {
					delete[]squares[n];
				}
				delete[]squares;
				delete[]spaceUsed;
				return;
			}
		}

		//Clean up
		for (int n = 0; n < numCols; n++) {
			delete[]squares[n];
		}
		delete[]squares;
		
		//Essentially skip the entire bottom half of this code and continue on with least values remaining.
		for (int k = 0; k < numNumbers; k++) {
			if (spaceUsed[k] != 0) {
				backtracker(solutions, puzzle);
				for (int n = 0; n < numCols; n++) {
					delete[]puzzle[n];
				}
				delete[]puzzle;
				delete[]spaceUsed;
				return;
			}
		}

		delete[]spaceUsed;

		//Essentially max we could solve, return it as a solution to cheat and make this easy on myself
		if (mode == 2) {
			solutions.push_back(puzzle);
			return;
		}

		//We have to guess, increase difficulty
		difficultyRate++;

		//THIS IS THE END OF LEAST VALUES REMAINING NOW WE JUST TRY FROM LOWEST NUMBER GOING FOR LOWEST NUMBER OF BRANCHES
		//Why do we not try based off which squares have the least amount of options you say?
		//Because in practice (and solving them by hand) I found that the combination of these two methods was very effective
		//And allows for agressive solving by the computer through our pruning option.
		//Maybe I'm wrong, who knows.

		//Are we done with the current number?
		int incremented = 0;
		while (numbers[currentNumber].remaining == 0) {
			currentNumber++;
			incremented++;
			if (currentNumber == numNumbers) {
				//I don't think we will ever get here
				currentNumber = 0;
			}
		}

		//Recursively call the new arrows we can
		int row = numbers[currentNumber].row;
		int col = numbers[currentNumber].col;
		int curDir = numbers[currentNumber].curDir;
		//Up
		if (curDir <= 0) {
			for (int i = row - 1; i >= 0; i--) {
				if (puzzle[i][col] != '^') {
					//Is it empty? Then we can fill.
					if (puzzle[i][col] == ' ') {
						puzzle[i][col] = '^';
						backtracker(solutions, puzzle);
						//Reset the space when we don't want it set
						puzzle[i][col] = ' ';
					}
					else {
						//Done with this direction
						curDir = 1;
						numbers[currentNumber].curDir = curDir;
					}
					//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
					break;
				}
			}
		}
		//Right
		if (curDir <= 1) {
			for (int i = col + 1; i < numCols; i++) {
				if (puzzle[row][i] != '>') {
					//Is it empty? Then we can fill.
					if (puzzle[row][i] == ' ') {
						puzzle[row][i] = '>';
						backtracker(solutions, puzzle);
						//Reset this space
						puzzle[row][i] = ' ';
					}
					else {
						//Done with this direction
						curDir = 2;
						numbers[currentNumber].curDir = curDir;
					}
					//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
					break;
				}
			}
		}
		//Down
		if (curDir <= 2) {
			for (int i = row + 1; i < numRows; i++) {
				if (puzzle[i][col] != 'v') {
					//Is it empty? Then we can fill.
					if (puzzle[i][col] == ' ') {
						puzzle[i][col] = 'v';
						backtracker(solutions, puzzle);
						//Reset this space
						puzzle[i][col] = ' ';
					}
					else {
						//Done with this direction
						curDir = 3;
						numbers[currentNumber].curDir = curDir;
					}
					//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
					break;
				}
			}
		}
		//Left
		if (curDir <= 3) {
			for (int i = col - 1; i >= 0; i--) {
				if (puzzle[row][i] != '<') {
					//Is it empty? Then we can fill.
					if (puzzle[row][i] == ' ') {
						puzzle[row][i] = '<';
						backtracker(solutions, puzzle);
						//Reset space
						puzzle[row][i] = ' ';
					}
					else {
						//Done with this direction ( shouldn't matter).
						curDir = 0;
						numbers[currentNumber].curDir = curDir;
					}
					//Otherwise its a number or a bad arrow and we can't (we also want to break after recursion.
					break;
				}
			}
		}

		while (incremented > 0) {
			numbers[currentNumber].curDir = 0;
			currentNumber--;
			incremented--;
		}
		//And clean up memory
		for (int n = 0; n < numCols; n++) {
			delete[]puzzle[n];
		}
		delete[]puzzle;

		return;
	}
}

// Will create a puzzle using random methods along with pruning. The difficulty will tune the guesses needed to solve the puzzle.
// Your puzzle.txt should follow the format of normal file inputs and can be anywhere from all X to completed (but why). In the end it will be overwritten with the completed puzzle.
void puzzleCreation(char** puzzleState, vector<char**> &solutions) {

	srand((int)time(0));

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

	//Update remaining values of puzzle.
	updateRemaining(puzzle);
	//Not valid, don't continue
	if (!isValid(puzzle)) {
		for (int n = 0; n < numCols; n++) {
			delete[]puzzle[n];
		}
		delete[]puzzle;
		return;
	}
	//Check if we have solveable puzzle
	backtracker(solutions, puzzle);
	if (solutions.size() == 1) {
		mode = 1;
		return;
	}
	solutions.clear();

	//Need a duplicate sized grid of ints
	int** squares;
	squares = new int *[numCols];
	int* spaceUsed = new int[numNumbers];

	for (int i = 0; i < numNumbers; i++) {
		spaceUsed[i] = 0;
	}

	//Create and intialize our new grid
	for (int n = 0; n < numCols; n++) {
		squares[n] = new int[numRows];
		for (int m = 0; m < numRows; m++) {
			if (puzzle[n][m] == '^') {
				squares[n][m] = 0;
			}
			else if (puzzle[n][m] == '>') {
				squares[n][m] = 1;
			}
			else if (puzzle[n][m] == 'v') {
				squares[n][m] = 2;
			}
			else if (puzzle[n][m] == '<') {
				squares[n][m] = 3;
			}
			else {
				squares[n][m] = -1;
			}
		}
	}

	//Now loop through all the numbers, if a square can be touched it gets the corresponding index of the arrow
	// it would have in the arrows array.
	// If it stays at -1, we know we have an invalid 
	// 4 Means a number is there
	// If it is a 5, then it has multiple options and will be ignored
	// In the end numbers will be filled in with their arrows on the puzzle board
	for (int k = 0; k < numNumbers; k++) {
		int curRow = numbers[k].row;
		int curCol = numbers[k].col;
		squares[curRow][curCol] = 4;
		//Loop in all directions setting numbers;
		//Up
		int remaining = numbers[k].remaining;
		for (int i = curRow - 1; i >= 0; i--) {
			remaining--;
			//This number can't reach in this direction any longer
			if (remaining < 0) {
				break;
			}
			if (puzzle[i][curCol] != ' ' && puzzle[i][curCol] != '^') {
				//Not empty, not right direction, dead end
				break;
			}
			else if (puzzle[i][curCol] == ' ') {
				if (squares[i][curCol] == -1) {
					//We can set this square
					squares[i][curCol] = 0;
				}
				else {
					//Someone else has already been here
					squares[i][curCol] = 5;
				}
			}
			else {
				//This square was already taken by a good arrow and doesn't cost us.
				remaining++;
			}
		}
		//Right
		remaining = numbers[k].remaining;
		for (int i = curCol + 1; i < numCols; i++) {
			remaining--;
			//This number can't reach in this direction any longer
			if (remaining < 0) {
				break;
			}
			if (puzzle[curRow][i] != ' ' && puzzle[curRow][i] != '>') {
				//Not empty, not right direction, dead end
				break;
			}
			else if (puzzle[curRow][i] == ' ') {
				if (squares[curRow][i] == -1) {
					//We can set this square
					squares[curRow][i] = 1;
				}
				else {
					//Someone else has already been here
					squares[curRow][i] = 5;
					//We don't stop because a farther square could matter more.
				}
			}
			else {
				//This square was already taken by a good arrow and doesn't cost us.
				remaining++;
			}
		}
		//Down
		remaining = numbers[k].remaining;
		for (int i = curRow + 1; i < numRows; i++) {
			remaining--;
			//This number can't reach in this direction any longer
			if (remaining < 0) {
				break;
			}
			if (puzzle[i][curCol] != ' ' && puzzle[i][curCol] != 'v') {
				//Not empty, not right direction, dead end
				break;
			}
			else if (puzzle[i][curCol] == ' ') {
				if (squares[i][curCol] == -1) {
					//We can set this square
					squares[i][curCol] = 2;
				}
				else {
					//Someone else has already been here
					squares[i][curCol] = 5;
				}
			}
			else {
				//This square was already taken by a good arrow and doesn't cost us.
				remaining++;
			}
		}
		//Left
		remaining = numbers[k].remaining;
		for (int i = curCol - 1; i >= 0; i--) {
			remaining--;
			//This number can't reach in this direction any longer
			if (remaining < 0) {
				break;
			}
			if (puzzle[curRow][i] != ' ' && puzzle[curRow][i] != '<') {
				//Not empty, not right direction, dead end
				break;
			}
			else if (puzzle[curRow][i] == ' ') {
				if (squares[curRow][i] == -1) {
					//We can set this square
					squares[curRow][i] = 3;
				}
				else {
					//Someone else has already been here
					squares[curRow][i] = 5;
					//We don't stop because a farther square could matter more.
				}
			}
			else {
				//This square was already taken by a good arrow and doesn't cost us.
				remaining++;
			}
		}
	}

	vector<vector<int>> avaiableCords;
	//Check to see if a space can't be reached (Essentially another is valid check with a new criteria)
	for (int m = 0; m < numRows; m++) {
		for (int n = 0; n < numCols; n++) {
			if (squares[m][n] == -1) {
				vector<int> cord;
				cord.push_back(m);
				cord.push_back(n);
				avaiableCords.push_back(cord);
			}
		}
	}

	//Clean up
	for (int n = 0; n < numCols; n++) {
		delete[]squares[n];
	}
	delete[]squares;
	delete[]spaceUsed;
	printPuzzle(puzzle);
	//Drop numbers on -1s 
	while (!avaiableCords.empty()) {
		int idx = rand() % avaiableCords.size();
		vector<int> cord = avaiableCords[idx];
		int curRow = cord[0];
		int curCol = cord[1];
		//Find sizes that can fit there
		int max = 0;
		//Up
		for (int i = curRow - 1; i >= 0; i--) {
			//This number can reach in this direction any longer
			if (puzzle[i][curCol] == ' ') {
				max++;
			}
			else {
				break;
			}
		}
		//Right			
		for (int i = curCol + 1; i < numCols; i++) {
			//This number can't reach in this direction any longer	
			if (puzzle[curRow][i] == ' ') {
				max++;
			}
			else {
				break;
			}
		}
		//Down
		for (int i = curRow + 1; i < numRows; i++) {
			//This number can't reach in this direction any longer
			if (puzzle[i][curCol] == ' ') {
				max++;
			}
			else {
				break;
			}
		}
		//Left
		for (int i = curCol - 1; i >= 0; i--) {
			if (puzzle[curRow][i] == ' ') {
				max++;
			}
			else {
				break;
			}
		}
		if (max != 0) {
			int size = (rand() % max) + 1;
			puzzle[curRow][curCol] = size + '0';
			number num;
			num.value = size;
			num.remaining = size;
			num.col = curCol;
			num.row = curRow;
			num.curDir = 0;
			numbers.push_back(num);
			numNumbers++;
			//Recurse
			puzzleCreation(puzzle, solutions);
			if (solutions.size() == 1) {
				mode = 1;
				return;
			}
			numNumbers--;
			
			for (int i = 0; i < numbers.size(); i++) {
				if (numbers[i].row == num.row && numbers[i].col == num.col) {
					numbers.erase(numbers.begin() + i);
				}
			}
		}
		vector<int>().swap(cord);
		vector<int>().swap(avaiableCords[idx]);
		avaiableCords.erase(avaiableCords.begin() + idx);
	}
	vector<vector<int>>().swap(avaiableCords);
	//Clean up
	for (int n = 0; n < numCols; n++) {
		delete[]puzzle[n];
	}
	delete[]puzzle;
}

//Entry point for the backtracker, handles IO, printing, and starting the backtracking
int main() {

	//Open File For Input
	//First line must be ROW COL
	//Following lines will be x's representing spaces and appropriate numbers.
	ifstream file ("puzzle.txt");
	string buffer;
	getline(file, buffer);
	//Read the top line for the size of puzzle
	int i = 0;
	string num = "";
	while (isdigit(buffer[i])) {
		num += buffer[i];
		i++;
	}
	numRows = stoi(num);

	i++;
	num = "";
	while (isdigit(buffer[i])) {
		num += buffer[i];
		i++;
	}
	numCols = stoi(num);

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
			else if (buffer[m] == 'v' || buffer[m] == '^' || buffer[m] == '<' || buffer[m] == '>') {
				puzzle[n][m] = buffer[m];
			}
			else {
				puzzle[n][m] = buffer[m];
				numbers.push_back(number());
				numbers[numNumbers].row = n;
				numbers[numNumbers].col = m;
				numbers[numNumbers].value = buffer[m] - '0';
				numbers[numNumbers].remaining = numbers[numNumbers].value;
				numbers[numNumbers].curDir = 0;
				numNumbers++;
			}
		}
	}

	//Close the file
	file.close();

	//Sort array of numbers so that we can just do them in order
	sort(numbers.begin(), numbers.end(), numbersSorter);

	//Print OG Puzzle
	cout << "Lijenspel Backtracker" << endl << endl;
	cout << "Starting State:" << endl;

	printPuzzle(puzzle);

	cout << "Select Option:" << endl;
	cout << "1 - Solve" << endl;
	cout << "2 - Solve until guess" << endl;
	cout << "3 - One Step At a Time" << endl;
	cout << "4 - Creation Mode" << endl;
	cout << "Number: ";

	cin >> mode;

	cout << endl;

	vector<char**> solutions;

	difficultyRate = 0;
	//Call backtracker and let it return a 3d array of puzzle solutions
	if (mode == 4) {
		puzzleCreation(puzzle, solutions);
	}
	else {
		backtracker(solutions, puzzle);
	}

	delete[]puzzle;

	//No solution :(
	if (solutions.empty()) {
		cout << "No States for Provided Mode" << endl << endl;
	}
	
	//Print Solutions
	else {

		//Check for duplicate solutions
		deleteDuplicates(solutions);

		//Calculate DiffcultyRating if we solved the whole thing
		if (mode == 1) {
			difficultyRate = (((5)*(pow(difficultyRate, (double)1/4)/4)*(numRows/6)*(numCols/6)) / solutions.size())+1;
			if (difficultyRate > 10) {
				//Just a catch for the crazy hard ones that disobey scaling
				difficultyRate = 10;
			}
			cout << "This puzzle is rated a " << setprecision(4) << difficultyRate << " out of 10 (10 being extremely hard)" << endl;
		}

		//Display Solutions Returned to console
		cout << "There is/are " << solutions.size() << " state(s):" << endl;
		if (solutions.size() == 1) {
			//File output if only one solution
			ofstream solution;
			solution.open("solution.txt");
			solution << numRows << " " << numCols << "\n";
			for (int n = 0; n < numRows; n++) {
				for (int m = 0; m < numCols; m++) {
					solution << solutions[0][n][m];
				}
				solution << endl;
			}
			solution.close();
			ofstream original;
			original.open("original.txt");
			original << numRows << " " << numCols << endl;
			for (int n = 0; n < numRows; n++) {
				for (int m = 0; m < numCols; m++) {
					if (solutions[0][n][m] == '<' || solutions[0][n][m] == '>' || solutions[0][n][m] == 'v' || solutions[0][n][m] == '^') {
						original << 'x';
					}
					else {
						original << solutions[0][n][m];
					}				
				}
				original << endl;
			}
			original.close();

		}
		while (!solutions.empty()) {
			printPuzzle(solutions[solutions.size() - 1 ]);
			solutions.pop_back();
		}
	}
	
	//Wait for input to close
	system("pause");
	return 0;
}
