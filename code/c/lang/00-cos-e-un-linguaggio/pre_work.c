#include <stddef.h>
#include <stdio.h>

enum Tag { VALUE, BINARY, CALL };

struct BYNARY_STRUCT {
  void *left;
  void *right;
};

struct CALL_STRUCT {
  void *args;
  int argc;
};

union NodeValue {
  double value;
  struct CALL_STRUCT c;
  struct BYNARY_STRUCT b;
};

typedef struct Node {
  enum Tag tag;
  union NodeValue V;
} Node;

int main(void) {
  Node N;
  printf("Size of Node: %lu \n", sizeof(N));
  printf("Offset of Union: %lu", 
      offsetof(struct Node, V)
      );

  return 0;
}
