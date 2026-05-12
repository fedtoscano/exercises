#include "n-queens.h"
#include <stdio.h>

/*
 *
 * {
 *  {X, 0, 0, 0},
 *  {0, 0, 0, 0},
 *  {0, 0, 0, 0},
 *  {0, 0, 0, 0},
 * }
 *
 * */

#define N 8
typedef int Board[N][N];

int queen_is_safe(Board *b, int x, int y) {
  for (int i = 0; i < N; i++) {
    // horizontal
    if (b[x][y + i])
      return 1;

    // vertical
    if (b[x + i][y])
      return 1;

    // diagonal
    if (b[x + i][y + i])
      return 1;
  }

  return 0;
}

void print_board(Board b) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d", b[i][j]);
    }
    printf("\n");
  }
}

int n_queens() {
  Board b;
  for (int i = 0; i < N; i++) {
    int y = 0;
    // place queen
    b[i][y] = 1;
    while (!queen_is_safe(&b, i, y)) {
      // move the queen from left to right
      b[i][y] = 0;
      b[i][++y] = 1;
    }
  }

  print_board(b);
  return 0;
}

int main() {
  n_queens();
  return 0;
}
