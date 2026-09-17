#include <stddef.h>
#include <stdio.h>

enum Tag { VALUE, BINARY, CALL };

struct BYNARY_STRUCT { // 16byte
  void *left;
  void *right;
};

struct CALL_STRUCT { // 12byte
  void *args;
  int argc;
};

union NodeValue {
  double value;
  struct CALL_STRUCT c;
  struct BYNARY_STRUCT b;
};

typedef struct Node {
  enum Tag tag;      // 4byte
  union NodeValue V; // 16byte
} Node;

int main(void) {
  Node N;
  N.V.value = 3.14;

  printf("Size of Node: %lu \n", sizeof(N));
  printf("Offset of Union: %lu \n", offsetof(struct Node, V));

  printf("size=%zu align=%zu\n", sizeof(union NodeValue),
         _Alignof(union NodeValue));
  printf("size=%zu align=%zu\n", sizeof(Node), _Alignof(Node));


  return 0;
}
