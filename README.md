# LijenspelBacktracker
A backtracker for the puzzle Lijenspel (a.k.a Line Game)
Rules and puzzles can be found at http://puzzlepicnic.com/genre?id=51.
Author: Bryson McIver
Created: 2/3/2017
Last Update: 3/2/2017

  Adding human strategy to look if a a number needs to use a space to complete itself
  Currently for speeding up creation pruning, but should be easy to move into the backtracker once it works

  Outputs solution to solution.txt and a fresh clean board at original.txt

  Puzzle creation now works though! Supports partially made puzzles! Finishes creation for you! Create from scratch!
  Run with puzzle.txt in the normal format, but put all x in for the grid (or a partial creation) and select option 4 in the menu.
  Example currently in puzzle.txt

  Note: Creation can take a long time, there's a lot of branching, some of which is pretty random. 6x6 seems to take anywhere between 1 and 5 minutes.
        Ideas for improvement on creation would be appreciated.

 Aditional Details:
  Only supports single digit numbers in the grid.
  A 9 9 puzzle considered hard took about 5 seconds on my i7 4770. Woo!
  Supports solving from partial solutions/checking if you can still solve based on current state
  Little home made rating system, step by step, and until next guess modes made for creating puzzles (by hand)