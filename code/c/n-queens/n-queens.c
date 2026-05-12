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
    if (b[x][i])
      return 0;

    // vertical
    if (b[i][y])
      return 0;

    // diagonal
    int d1 = x + y - 1;
    if(d1 >= 0 && d1 < N && b[i][d1]) return 0;

    //diagonal
    int d2 = i + y - x;
    if(d1 >= 0 && d2 < N && b[i][d2]) return 0;
  }

  return 1;
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
