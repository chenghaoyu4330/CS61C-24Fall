#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // 创建一条存储在堆内存中的蛇
  snake_t *snake = malloc(sizeof(snake_t));
  if (snake == NULL) {
    free(snake);
    return NULL;
  }
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->live = true;

  // 创建游戏状态，存储在堆内存中
  game_state_t *state = malloc(sizeof(game_state_t));
  if (state == NULL) {
    return NULL;
  }
  state->num_rows = 18;
  state->board = malloc(state->num_rows * sizeof(char *));
  if (state->board == NULL) {
    free(state);
    return NULL;
  }
  state->board[0] = strcpy(malloc(21 * sizeof(char)), "####################");
  state->board[1] = strcpy(malloc(21 * sizeof(char)), "#                  #");
  state->board[2] = strcpy(malloc(21 * sizeof(char)), "# d>D    *         #");
  for (unsigned int i = 3; i < state->num_rows - 1; ++i) {
    state->board[i] = strcpy(malloc(21 * sizeof(char)), "#                  #");
  }
  state->board[state->num_rows - 1] = strcpy(malloc(21 * sizeof(char)), "####################");
  state->num_snakes = 1;
  state->snakes = snake;

  return state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // 释放与游戏状态相关的所有内存，包括棋盘和蛇。
  // 注意，我们需要释放棋盘的每一行，因为释放行指针数组（state->board）并不会释放行本身（state->board[i]）。
  if (state == NULL) {
    return;
  }
  for (unsigned int i = 0; i < state->num_rows; ++i) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // 按行打印棋盘到指定的文件指针fp
  for (unsigned int i = 0; i < state->num_rows; ++i) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  char valid_chars[5] = {'w', 'a', 's', 'd'};
  for (int i=0; i<4; ++i) {
    if (c == valid_chars[i]) {
      return true;
    }
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  char valid_chars[6] = {'W', 'A', 'S', 'D', 'x'};
  for (int i=0; i<5; ++i) {
    if (c == valid_chars[i]) {
      return true;
    }
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  char valid_chars[14] = {'w', 'a', 's', 'd', '^', '<', 'v', '>', 'W', 'A', 'S', 'D', 'x'};
  for (int i=0; i<13; ++i) {
    if (c == valid_chars[i]) {
      return true;
    }
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  char* body_chars = "^<v>";
  char* tail_chars = "wasd";
  for (int i=0; i<4; ++i) {
    if (c == body_chars[i]) return tail_chars[i];
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  char* body_chars = "^<v>";
  char* head_chars = "WASD";
  for (int i=0; i<4; ++i) {
    if (c == head_chars[i]) return body_chars[i];
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  switch (c)
  {
  case 'v':
  case 's':
  case 'S':
    return cur_row + 1;
    break;

  case '^':
  case 'w':
  case 'W':
    return cur_row - 1;
    break;
  
  default:
    return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  switch (c)
  {
  case '>':
  case 'd':
  case 'D':
    return cur_col + 1;
    break;

  case '<':
  case 'a':
  case 'A':
    return cur_col - 1;
    break;
  
  default:
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  unsigned int cur_snake_head_col = state->snakes[snum].head_col;
  unsigned int cur_snake_head_row = state->snakes[snum].head_row;
  char cur_snake_head = get_board_at(state, cur_snake_head_row, cur_snake_head_col);
  switch (cur_snake_head)
  {
  case 'W':
    return get_board_at(state, cur_snake_head_row-1, cur_snake_head_col);
  
  case 'A':
    return get_board_at(state, cur_snake_head_row, cur_snake_head_col-1);
  
  case 'S':
    return get_board_at(state, cur_snake_head_row+1, cur_snake_head_col);
  
  case 'D':
    return get_board_at(state, cur_snake_head_row, cur_snake_head_col+1);
  
  default:
    break;
  }
  
  return '?';
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  unsigned int cur_snake_head_col = state->snakes[snum].head_col;
  unsigned int cur_snake_head_row = state->snakes[snum].head_row;
  char cur_snake_head = get_board_at(state, cur_snake_head_row, cur_snake_head_col);

  switch (cur_snake_head)  // 除了更新新的头的信息之外，记得更新老的头所在board位置的字符
  {
  case 'W':
    state->snakes[snum].head_row -= 1;
    set_board_at(state, cur_snake_head_row-1, cur_snake_head_col, 'W');
    set_board_at(state, cur_snake_head_row, cur_snake_head_col, '^');
    break;
  
  case 'A':
    state->snakes[snum].head_col -= 1;
    set_board_at(state, cur_snake_head_row, cur_snake_head_col-1, 'A');
    set_board_at(state, cur_snake_head_row, cur_snake_head_col, '<');
    break;
  
  case 'S':
    state->snakes[snum].head_row += 1;
    set_board_at(state, cur_snake_head_row+1, cur_snake_head_col, 'S');
    set_board_at(state, cur_snake_head_row, cur_snake_head_col, 'v');
    break;
  
  case 'D':
    state->snakes[snum].head_col += 1;
    set_board_at(state, cur_snake_head_row, cur_snake_head_col+1, 'D');
    set_board_at(state, cur_snake_head_row, cur_snake_head_col, '>');
    break;
  
  default:
    printf("%s", "?");
    break;
  }

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  return NULL;
}
