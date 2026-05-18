#include <stdio.h>

#define N 4

typedef int Board[N][N];

int queen_is_safe(Board b, int row, int col) {
  // horizontal
  for (int i = 0; i < col; i++) {
    if (b[row][i]) return 0;
  }
  // diagonal up-left
  for (int i = 1; row - i >= 0 && col - i >= 0; i++) {
    if (b[row - i][col - i]) return 0;
  }
  // diagonal down-left
  for (int i = 1; row + i < N && col - i >= 0; i++) {
    if (b[row + i][col - i]) return 0;
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

int solve(Board b, int col) {
  // if col is greater than number of cols (4 in our case), return
  if (col >= N) return 1;

  //we check every row for given col
  for (int row = 0; row < N; row++) {
    if (queen_is_safe(b, row, col)) {

      //place a queen
      b[row][col] = 1;
      
      //if the queen is safe, skip iteration
      if (solve(b, col + 1)) return 1;

      // otherwise, clear the house and place queen next house
      b[row][col] = 0;
    }

    // if queen is not safe increase row counter
  }
  return 0;
}

int main() {
  Board b = {0};
  // we start from col = 0
  int solved = solve(b, 0);
  if(solved) print_board(b);
  else printf("Queen board was not solved!\n");
  return 0;
}
