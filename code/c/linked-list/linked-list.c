#include <stdio.h>
#include <stdlib.h>

// Struct definition
typedef struct node {
  int value;
  struct node *next;
} node;

node *node_init(int value) {
  node *n = malloc(sizeof(node));
  if (n == NULL) {
    printf("pointer n refers to Null");
    return NULL;
  }

  // populates new node
  n->value = value;
  n->next = NULL;

  return n;
}

node *node_append(node *root, int value) {
  //initialize new node with given value
  node *next = node_init(value);

  // if root == NULL return return first node only
  if (root == NULL) return next;

  // appends 'next' node searching for the 
  // last node of the list
  node *temp = root;
  while (temp->next) temp = temp->next;
  temp->next = next;

  return temp;
}

// separator = ", "
void node_print(node *root);

// return node with value
node *node_search(node *root, int value);

// delete first node with value
void node_delete(node *root, int value);

int main(void) {
  node *n = node_init(5);
  printf("%p", n->next);
}

