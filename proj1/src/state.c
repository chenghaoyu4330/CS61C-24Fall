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
  // 创建一条蛇，存储在堆中
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

  // 创建游戏状态，存储在堆中
  game_state_t *state = malloc(sizeof(game_state_t));
  if (state == NULL) {
    return NULL;
  }

  state->num_snakes = 1;
  state->snakes = snake;

  state->num_rows = 18;
  state->board = malloc(state->num_rows * sizeof(char *));
  if (state->board == NULL) {
    free(state);
    return NULL;
  }

  for (int i = 0; i < 18; i++) {
    state->board[i] = (char *) malloc(22 * sizeof(char));
    if (state->board[i] == NULL) {
      perror("Failed to allocate memory for default_state");
      return NULL;
    }
  }

  // 来自task 5的反馈：这里一定要加\n！！！不然后面调试有好果汁吃！！
  // \n是因为task 5实现的从文件中读取行的函数，会把\n一并读取，因此这里构建board时也要把\n带上
  strcpy(state->board[0], "####################\n");
  for (int i = 1; i <= 16; ++i) {
    if (i == 2) {
      continue;
    }
    strcpy(state->board[i], "#                  #\n");
  }
  strcpy(state->board[2], "# d>D    *         #\n");
  strcpy(state->board[17], "####################\n");

  return state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // 释放与游戏状态相关的所有内存，包括棋盘和蛇。
  // 注意需要释放board的每一行，因为释放行指针数组（state->board）并不会释放行本身（state->board[i]）。
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
    fprintf(fp, "%s", state->board[i]);  // 来自task 5的反馈：这里要删掉\n，因为board的每一行已经有\n了
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
  // 获取当前头部的位置和字符
  unsigned int cur_snake_head_col = state->snakes[snum].head_col;
  unsigned int cur_snake_head_row = state->snakes[snum].head_row;
  char cur_snake_head = get_board_at(state, cur_snake_head_row, cur_snake_head_col);

  // 计算移动后的头部所在位置，返回该位置的字符
  unsigned int next_snake_head_col = get_next_col(cur_snake_head_col, cur_snake_head);
  unsigned int next_snake_head_row = get_next_row(cur_snake_head_row, cur_snake_head);
  return get_board_at(state, next_snake_head_row, next_snake_head_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // 获取当前头部所在位置以及字符
  unsigned int cur_snake_head_col = state->snakes[snum].head_col;
  unsigned int cur_snake_head_row = state->snakes[snum].head_row;
  char cur_snake_head = get_board_at(state, cur_snake_head_row, cur_snake_head_col);

  // 计算新头部的位置
  unsigned int new_snake_head_row = get_next_row(cur_snake_head_row, cur_snake_head);
  unsigned int new_snake_head_col = get_next_col(cur_snake_head_col, cur_snake_head);

  // 更新新头部的位置
  state->snakes[snum].head_row = new_snake_head_row;
  state->snakes[snum].head_col = new_snake_head_col;

  // 将新头部的位置设置为当前头部；调用head_to_body，将老头部所在的head转换为body
  set_board_at(state, new_snake_head_row, new_snake_head_col, cur_snake_head);
  set_board_at(state, cur_snake_head_row, cur_snake_head_col, head_to_body(cur_snake_head));

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
  // 获取当前尾巴所在位置以及字符
  unsigned int cur_snake_tail_col = state->snakes[snum].tail_col;
  unsigned int cur_snake_tail_row = state->snakes[snum].tail_row;
  char cur_snake_tail = get_board_at(state, cur_snake_tail_row, cur_snake_tail_col);

  // 计算新尾巴的位置以及字符
  unsigned int new_snake_tail_row = get_next_row(cur_snake_tail_row, cur_snake_tail);
  unsigned int new_snake_tail_col = get_next_col(cur_snake_tail_col, cur_snake_tail);
  char new_snake_tail_cur_body = get_board_at(state, new_snake_tail_row, new_snake_tail_col);

  // 更新新尾巴的位置
  state->snakes[snum].tail_row = new_snake_tail_row;
  state->snakes[snum].tail_col = new_snake_tail_col;

  // 将当前尾巴的位置设为空格；调用body_to_tail，将新尾巴所在的body转换为tail
  set_board_at(state, cur_snake_tail_row, cur_snake_tail_col, ' ');
  set_board_at(state, new_snake_tail_row, new_snake_tail_col, body_to_tail(new_snake_tail_cur_body));

  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  unsigned int num_snakes = state->num_snakes;
  for (unsigned int i=0; i<num_snakes; ++i) {
    snake_t* snake = &state->snakes[i];

    // 判断当前蛇的存活情况，死亡则跳过
    if (!snake->live) continue;

    // 获取蛇即将移动到的位置的字符
    char dest_board_char = next_square(state, i);

    // 根据即将移动到的位置的信息，更新蛇的状态
    if (is_snake(dest_board_char) || dest_board_char == '#')  // 撞到蛇的身体或者墙壁，死亡、停止移动、头部替换为'x'
    {
      snake->live = false;
      set_board_at(state, snake->head_row, snake->head_col, 'x');
    }
    else if (dest_board_char == '*') // 遇到水果，吃掉并增加1个长度，则可以只更新头部，不更新尾部；棋盘新增水果
    {
      update_head(state, i);
      add_food(state);
    }
    else  // 遇到空格，正常更新头和尾
    {
      update_head(state, i);
      update_tail(state, i);
    }
    
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  if (fp == NULL) {
    return NULL;
  }

  size_t capacity = 16;  // 初始缓冲区大小
  size_t length = 0;     // 当前读取的字符数

  char *buffer = (char *) malloc(capacity * sizeof(char));
  if (buffer == NULL) {
    return NULL;
  }

  while (fgets(buffer + length, capacity - length, fp)) {
    if (strchr(buffer + length, '\n')) {
      // 如果读到了换行符，说明这一行已经读完
      return buffer;
    }

    // 更新已读取的长度
    length += strlen(buffer + length);
    // 从上次读取的位置（指针指向buffer + length）开始计算长度，strlen返回的是从该位置到null字符的长度
    // 无需担心strlen读不到null字符的问题，因为fgets保证了读取的字符串以null字符结尾
    // 或者直接使用length = strlen(buffer)也是可以的

    // 扩展缓冲区
    capacity *= 2;
    char *new_buffer = realloc(buffer, capacity * sizeof(char));
    if (new_buffer == NULL) {
      free(buffer);
      return NULL;  // 内存分配失败
    }
    buffer = new_buffer;
  }
  if (length == 0) {
    // 如果没有读取到任何字符，说明是EOF或错误
    free(buffer);
    return NULL;
  }

  return NULL;  // 返回已读取的数据，即使没有换行符
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  game_state_t *state = (game_state_t *) malloc(sizeof(game_state_t));
  if (state == NULL) {
    return NULL;
  }

  state->num_rows = 0;
  state->board = NULL;
  state->num_snakes = 0;
  state->snakes = NULL;

  while (true) {
    // 读取每一行
    char *line = read_line(fp);
    if (line == NULL) {
      break;  // 读取结束或出错
    }

    // 扩展棋盘行指针数组
    char **new_board = (char **) realloc(state->board, (state->num_rows + 1) * sizeof(char *));
    if (new_board == NULL) {
      free(new_board);
      free(state->board);
      free_state(state);
      return NULL;  // 内存分配失败
    }
    
    state->board = new_board;
    state->board[state->num_rows] = line;
    (state->num_rows)++;
  }
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // 获取蛇的尾巴位置和字符
  unsigned int cur_row = state->snakes[snum].tail_row;
  unsigned int cur_col = state->snakes[snum].tail_col;
  char cur_char = get_board_at(state, cur_row, cur_col);

  // 追踪蛇的身体直到找到头部
  while (1) {
    unsigned int next_row = get_next_row(cur_row, cur_char);
    unsigned int next_col = get_next_col(cur_col, cur_char);
    char next_char = get_board_at(state, next_row, next_col);
    if (is_head(next_char)) {
      state->snakes[snum].head_row = next_row;
      state->snakes[snum].head_col = next_col;
      return;
    }
    else {
      cur_row = next_row;
      cur_col = next_col;
      cur_char = next_char;
    }
  }
  printf("find_head() went wrong!");
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // 遍历棋盘，找到所有蛇的尾巴，初始化蛇结构体数组

  unsigned int total_snakes = 0;
  unsigned int board_rows = state->num_rows;
  snake_t* snakes = NULL;

  // 遍历棋盘的每一个位置
  for (unsigned int i=0; i<board_rows; ++i) {
    size_t board_cols = strlen(state->board[i]);
    for (unsigned int j=0; j<board_cols; ++j) {
      if (is_tail(get_board_at(state, i, j))) {
        // 发现蛇的尾巴，则扩展state->snakes数组，初始化新蛇的信息
        // 注意reallocate可能返回NULL，因此需要一个临时指针new_snakes接收返回值，以防止直接覆盖snakes指针导致内存泄漏
        // 如果不这样做，运行会报错 Segmentation fault (core dumped)
        snake_t* new_snakes = (snake_t*) realloc(snakes, (total_snakes+1) * sizeof(snake_t));
        if (new_snakes == NULL) {
          free(snakes);
          return NULL;
        }
        snakes = new_snakes;  // 更新snakes指针
        snakes[total_snakes].tail_row = i;
        snakes[total_snakes].tail_col = j;
        snakes[total_snakes].live = true;
        state->snakes = snakes;
        state->num_snakes = total_snakes+1;
        find_head(state, total_snakes);
        total_snakes++;
      }
    }
  }
  
  return state;
}