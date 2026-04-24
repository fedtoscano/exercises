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
  // initialize new node with given value
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

void node_print(node *root) {
  char *sep = ", ";
  printf("Node list values: ");
  while (root) {
    printf("%d", root->value);
    if (root->next != NULL) printf("%s", sep);
    root = root->next;
  }

  printf("\n");
}

// return first node with value
node *node_search(node *root, int value) {
  printf("Searching node with value %d...\n", value);

  node *temp = root;
  while (temp->value != value) {
    if (temp->next == NULL) return NULL;
    temp = temp->next;
  }

  printf("Address of found node: %p\n", root);
  return temp;
}

// delete first node with value
void node_delete(node *root, int value) {
  if(root->value == value){
    //this should be handled?
    printf("Cannot delete the first node!\n");
    return;
  }

  // find the previous node
  node *prev = root;
  while (prev->next->value != value) {
    if (!prev) {
      printf("No node with given value!\n");
      return;
    }
    prev = prev->next;
  }

  // if we have to delete last node
  if (prev->next->next == NULL) {
    free(prev->next);
    return;
  }

  //if the node is inside the list
  node *next = prev->next->next;
  free(prev->next);
  prev->next = next;
  printf("Node with value %d removed\n", value);
}

int main(void) {
  node *n = node_init(0);
  for (int i = 0; i <= 4; i++) node_append(n, i + 1);
  node_print(n);

  node *search = node_search(n, 3);
  search->value = 48;
  printf("modified found node's value\n");
  node_print(n);

  node_delete(n, 0);
  node_print(n);
}
