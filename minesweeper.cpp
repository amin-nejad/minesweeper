#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>
#include "minesweeper.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* pre-supplied function to load mine positions from a file */
void load_board(const char *filename, char board[9][9]) {

  cout << "Loading board from file '" << filename << "'... ";

  ifstream in(filename);
  if (!in)
    cout << "Failed!" << endl;
  assert(in);

  char buffer[512];

  int row = 0;
  in.getline(buffer,512);
  while (in && row < 9) {
    for (int n=0; n<9; n++) {
      assert(buffer[n] == '.' || buffer[n] == '*' || buffer[n] == ' ' || buffer[n] == '?' || isdigit(buffer[n]));
      board[row][n] = buffer[n];
    }
    row++;
    in.getline(buffer,512);
  }

  cout << ((row == 9) ? "Success!" : "Failed!") << endl;
  assert(row == 9);
}

/* internal helper function */
void print_row(const char *data, int row) {
  cout << (char) ('A' + row) << "|";
  for (int i=0; i<9; i++) 
    cout << ( (data[i]=='.') ? ' ' : data[i]);
  cout << "|" << endl;
}

/* pre-supplied function to display a minesweeper board */
void display_board(const char board[9][9]) {
  cout << "  ";
  for (int r=0; r<9; r++) 
    cout << (char) ('1'+r);
  cout <<  endl;
  cout << " +---------+" << endl;
  for (int r=0; r<9; r++) 
    print_row(board[r],r);
  cout << " +---------+" << endl;
}

/* pre-supplied function to initialise playing board */ 
void initialise_board(char board[9][9]) {
  for (int r=0; r<9; r++)
    for (int c=0; c<9; c++)
      board[r][c] = '?';
}

/* add your functions here */

bool is_complete(char mines[9][9], char revealed[9][9]){

  int i = 0, j = 0;

  while (i < 9){

    while (j < 9){

      if (mines[i][j] != '*'){

	if (revealed[i][j] == '?'){
	  return false;
	}
      }

      j++;
    }

    j = 0;
    i++;
  }

  return true;
}

int count_mines(string position, char mines[9][9]){

  int mine_count = 0;
  
  int row = position[0] - 'A';
  int col = position[1] - '0' -1;

  for (int i = row -1; i < row + 2; i++){

    for (int j = col -1; j < col + 2; j++){

      if (i >= 0 && i < 9 && j >= 0 && j < 9){

	if (mines[i][j] == '*'){
	  
	  if (!(i == row && j == col)){
	    
	    mine_count++;
	  }
	}
      }
    }
  } 

  return mine_count;
}

MoveResult make_move(string position, char mines[9][9], char revealed [9][9]){

  // firstly check to see if position given makes sense
  
  bool flag = false;
  
  if (position.length() > 3 || position.length() < 2){
    return INVALID_MOVE;
  } else if (position.length() == 3){
    char flag_char = position[2];
    flag = true;
    if (flag_char != '*'){
      return INVALID_MOVE;
    }
  }

  int row = position[0] - 'A';
  int col = position[1] - '0' -1;

  if (row < 0 || row > 8 || col < 0 || col > 8){
    return INVALID_MOVE;
  }

  // check for redundant move

  if (revealed[row][col] != '?'){
    return REDUNDANT_MOVE;
  }
  
  // check for mine if flag is false
  if (mines[row][col] == '*' && flag == false){
    return BLOWN_UP;
  }

  // flag if true OR if false: recursively uncover squares on the board
  if (flag == true){
    revealed[row][col] = '*';
  } else {
    int mines_count = count_mines(position, mines);
    char mines_count_ch = mines_count + '0';

    if (mines_count > 0){
      revealed[row][col] = mines_count_ch;
    } else {
      revealed[row][col] = ' ';
    }
    MoveResult result = make_proper_move(position, mines, revealed);
    return result;
  }
  
  // default condition
  return VALID_MOVE;
}

MoveResult make_proper_move(string position, char mines[9][9], char revealed [9][9]){

  bool solved_board = is_complete(mines, revealed);
  static int count = 0;
  
  if (solved_board == true){
    count = 0;
    return SOLVED_BOARD;
  }

  if (count == 1000){
    return VALID_MOVE;
  }

  int row = position[0] - 'A';
  int col = position[1] - '0' -1;
  int square_value;
  
  square_value = count_mines(position, mines);
  char sq_val_ch = square_value + '0';
  
  if (revealed[row][col] == '?'){

    for (int i = row -1; i < row + 2; i++){
      for (int j = col -1; j < col + 2; j++){
	if (i >= 0 && i < 9 && j >= 0 && j < 9){
	  if (!(i == row && j == col)){
	    if (isspace(revealed[i][j])){
	      if (mines[row][col] != '*'){
		if (square_value != 0){
		  revealed[row][col] = sq_val_ch;
		} else {
		  revealed[row][col] = ' ';
		}
		goto here;
	      }
	    }
	  }
	}
      }
    }
  }

  count++;
  
 here:
  if (col == 8 && row != 8){
    col = 0;
    row++;
  } else if (col == 8 && row == 8){
    col = 0;
    row = 0;
  } else {
    col++;
  }
  char new_row = row + 'A';
  char new_col = col + '0' + 1;

  position = string(1, new_row) + string(1, new_col);
  make_proper_move (position, mines, revealed);

  if (solved_board == true){
    count = 0;
    return SOLVED_BOARD;
  }
  
  count = 0;
  return VALID_MOVE;
}

bool find_safe_move(char revealed[9][9], char* move){

  int empty_count = 0;
  int mine_count = 0;
  int row = 0, col = 0;
  string temp_flag_moves;
  string move_str;
  
  while (row < 9){
    while (col < 9){
      
      if (isdigit(revealed[row][col])){
	
	for (int i = row -1; i < row + 2; i++){
	  for (int j = col -1; j < col + 2; j++){
	    if (i >= 0 && i < 9 && j >= 0 && j < 9){
	      if (!(i == row && j == col)){
		char new_row = i + 'A';
		char new_col = j + '0' + 1;
		
		if (revealed[i][j] == '?'){
		  temp_flag_moves += string(1, new_row) + string(1, new_col) + "* ";
		  empty_count++;
		}
		
		if (revealed[i][j] == '*'){
		  mine_count++;
		}
	      }
	    }
	  }
	}

	if (empty_count != 0){
	  stringstream ss(temp_flag_moves);
	  string word;

	  if (mine_count + empty_count == (revealed[row][col] - '0')){
	    ss >> word;
	    move_str = word;
	    break;
	    
	  } else if (mine_count == (revealed[row][col] - '0')){
	    ss >> word;
	    word = word.substr(0,2);
	    move_str = word;
	    break;
	  }
	}

	mine_count = 0;
	empty_count = 0;
	temp_flag_moves.clear();
      }
      col++;
    }
    if (move_str.length() > 0){
      break;
    }
    col = 0;
    row++;
  }

  if (move_str.length() == 0){
    return false;
  }
  strcpy(move, move_str.c_str());
  return true;
}
