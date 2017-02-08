// 
// LijenspelBacktracker.cpp : A backtracker for the puzzle Lijenspel (a.k.a Line Game)
// Rules and puzzles can be found at http://puzzlepicnic.com/genre?id=51.
// Author: Bryson McIver
// Created: 2/3/2017
// Last Update: 2/5/2017
//
// Aditional Details:
//	Only supports basic grid puzzles 9x9 and below.
//  Only support single digit numbers.
//  Missing a way to keep track of old previously reached configurations (This is a limitation
//    currently due to the fact that I go one arrow at a time).
//  A 9 9 puzzle considered hard took unknown minutes on my i7 4770.
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
//curDir is a way to not redo directions and configurations that had already been completed.
//	It is used as an index to the char array of arrows
struct number {
	int value, row, col, remaining, curDir;
};

//Global so we don't have to pass it
vector<number> numbers;
int numNumbers;
int* totalSpaceUsed;

//Keeps track of which number we are working on in the numbers array
int currentNumber = 0;

//Nice list of the arrow characters
const char arrows[4] = {'^', '>', 'v', '<'};

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
		for (int n = 0; n < numCols; n++) {
			cout << "|___";
		}
		cout << "|" << endl;
	}
	cout << endl;
}

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

	//Also check if all the spaces can be reached

	return true;
}

//Checks if any square cannot be filled and is therefore invalid, or if there is only one option for a square and it can be filled
bool remaingSquares(char** puzzle) {
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
		}
	}
	printSquares(squares);
	printPuzzle(puzzle);
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
						for (int i = 0; i < numNumbers; i++) {
							totalSpaceUsed[i] += spaceUsed[i];
						}
						return false;
					}
					puzzle[j][curCol] = '^';
					spaceUsed[k]++;
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
					if (puzzle[curRow][j] != '>') {
						break;
					}
					else if (puzzle[curRow][j] != ' ') {
						for (int i = 0; i < numNumbers; i++) {
							totalSpaceUsed[i] += spaceUsed[i];
						}
						return false;
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
						for (int i = 0; i < numNumbers; i++) {
							totalSpaceUsed[i] += spaceUsed[i];
						}
						return false;
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
						for (int i = 0; i < numNumbers; i++) {
							totalSpaceUsed[i] += spaceUsed[i];
						}
						return false;
					}
					puzzle[curRow][j] = '<';
					spaceUsed[k]++;
				}
			}
		}

		numbers[k].remaining -= spaceUsed[k];
		if (numbers[k].remaining < 0) {
			//Multiple spaces were only accessable by this number, and couldn't all be serviced by it.
			for (int i = 0; i < numNumbers; i++) {
				totalSpaceUsed[i] += spaceUsed[i];
			}
			return false;
		}
	}
	printPuzzle(puzzle);

	for (int n = 0; n < numCols; n++) {
		delete[]squares[n];
	}
	delete[]squares;
	
	//Continue this process until we can't eliminate any more.
	for (int k = 0; k < numNumbers; k++) {
		if (spaceUsed[k] != 0) {
			bool valid = remaingSquares(puzzle);
			if (valid) {
				return true;
			}
			else {
				for (int i = 0; i < numNumbers; i++) {
					totalSpaceUsed[i] += spaceUsed[i];
				}
				return false;
			}
		}
	}
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
	
	//Not valid, don't continue
	if (!isValid(puzzle)) {
		for (int n = 0; n < numCols; n++) {
			delete[]puzzle[n];
		}
		delete[]puzzle;
		
		for (int i = 0; i < numNumbers; i++) {
			numbers[i].remaining += totalSpaceUsed[i];
		}

		return;
	}
	//Fill squares that only have one option and check if invalid due to squares not having any options
	for (int i = 0; i < numNumbers; i++) {
		totalSpaceUsed[i] = 0;
	}
	if (!remaingSquares(puzzle)) {
		//Essentially not valid
		for (int n = 0; n < numCols; n++) {
			delete[]puzzle[n];
		}
		delete[]puzzle;

		for (int i = 0; i < numNumbers; i++) {
			numbers[i].remaining += totalSpaceUsed[i];
		}

		return;
	}
	//Check again after remaining squares goes off
	if (!isValid(puzzle)) {
		for (int n = 0; n < numCols; n++) {
			delete[]puzzle[n];
		}
		delete[]puzzle;

		for (int i = 0; i < numNumbers; i++) {
			numbers[i].remaining += totalSpaceUsed[i];
		}

		return;
	}
	//Do we have a solution? If so save it
	if (isSolved(puzzle)) {
		solutions[numSolutions] = puzzle;
		numSolutions++;
		
		for (int i = 0; i < numNumbers; i++) {
			numbers[i].remaining += totalSpaceUsed[i];
		}

		return;
	}
	//Keep going, generate next states
	else {
		//Are we done with the current number?
		int incremented = 0;
		printPuzzle(puzzle);
		while (numbers[currentNumber].remaining == 0) {
			currentNumber++;
			incremented++;
			if (currentNumber == numNumbers) {
				//I don't think we will ever get here
				currentNumber = 0;
			}
		}

		//Recursively call the new arrows we can
		numbers[currentNumber].remaining--;
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

		//If we came back we need to give it its remaining back and go back to our old current number
		numbers[currentNumber].remaining++;
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
		
		for (int i = 0; i < numNumbers; i++) {
			numbers[i].remaining += totalSpaceUsed[i];
		}

		return;
	}
}

//Will sort our numbers for us :)
//params: the two number structs to sort
int numbersSorter(number num1, number num2) {
	return (num1.value < num2.value);
}

//Entry point for the backtracker, handles IO, printing, and starting the backtracking
int main() {

	//Open File For Input
	//First line must be ROW COL
	//Following lines will be x's representing spaces and appropriate numbers.
	ifstream file ("puzzle.txt");
	string buffer;
	getline(file, buffer);
	numRows = buffer[0] - '0';
	numCols = buffer[2] - '0';

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

	//Call backtracker and let it return a 3d array of puzzle solutions 
	char*** solutions = new char**;
	totalSpaceUsed = new int[numNumbers];
	backtracker(solutions, puzzle);

	//No solution :(
	if (numSolutions == 0) {
		cout << "No Solutions for Provided State" << endl << endl;
	}
	
	//Print Solutions
	else {

		//Display Solutions Returned to console
		//Only does first solution

		cout << "There is/are " << numSolutions << " solution(s):" << endl;
		while (numSolutions > 0) {
			numSolutions--;
			printPuzzle(solutions[numSolutions]);
		}
	}
	
	//Wait for input to close
	system("pause");
	return 0;
}
