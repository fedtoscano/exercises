#include <stdio.h>
#include <stdlib.h>
typedef struct node {
  int value;
  struct node *prev;
  struct node *next;
} node;

node *node_init(int value) {
  node *n = malloc(sizeof(node));
  if (n == NULL) {
    printf("n points to NULL!\n");
    return NULL;
  }

  n->value = value;
  n->prev = NULL;
  n->next = NULL;
  return n;
}

node *append_after(node *to, int value) {
  printf("Appending value %d to node with value %d\n", value, to->value);
  node *new = node_init(value);
  if (to->next) {
    new->next = to->next;
    new->next->prev = new;
  }

  new->prev = to;
  to->next = new;

  return new;
}

node *append_before(node *to, int value) {
  printf("Prepending value %d to node with value %d\n", value, to->value);
  node *new = node_init(value);

  if (to->prev) {
    new->next = to;
    to->prev->next = new;
    new->prev = to->prev;
  }

  to->prev = new;

  return new;
}

void print_node(node *n) {
  printf("Node->value: %d\n", n->value);
  printf("Node->prev: %p\n", n->prev);
  printf("Node->next: %p\n", n->next);
}

node *node_search(node *root, int value) {
  printf("Searching node with value %d\n", value);
  while (root->value != value) {
    if (root->next == NULL) {
      printf("Node with given vlaue not found!\n");
      return NULL;
    }

    root = root->next;
  }
  print_node(root);
  return root;
}

void print_list(node *root) {
  char *sep = ", ";
  printf("List node values: ");
  while (root != NULL) {
    printf("%d", root->value);
    if (root->next != NULL)
      printf("%s", sep);
    root = root->next;
  }

  printf("\n");
}

void delete_node(node *n) {
  printf("Deleting node with value %d\n", n->value);
  if (n->next)
    n->next->prev = n->prev;
  if (n->prev)
    n->prev->next = n->next;
  free(n);
}

void append_values(node *to, int values[], int length){
  node *iterator = to;
  for(int i = 0; i < length; i++){
    node *new = append_after(iterator, values[i]);
    iterator = new;
  }
}

void prepend_values(node *to, int values[], int length){
  node *iterator = to;
  for(int i = 0; i < length; i++){
    node *new = append_before(to, values[i]);
    iterator = new;
  }
}

node *init_list(int values[], int length) {
  node *first = node_init(values[0]);

  node *iterator = first;
  for (int i = 1; i <= length - 1; i++) {
    node *new = append_after(iterator, values[i]);
    iterator = new;
  }

  return first;
}

int main(void) {

  int values[] = {0, 1, 2, 3, 56};
  int length = sizeof(values) / sizeof(values[0]);
  node *n = init_list(values, length);

  int appends[] = {38, 39, 27, 40, 38, 19};
  append_values(n, appends, 6);
  print_list(n);

  int prepends[] = {42, 8, 1, 3};

  prepend_values(
      node_search(n, 27),
      prepends,
      4
  );

  print_list(n);

  // int value = 1;
  // printf("Initalizing node list with value %d\n", value);
  // node *n = node_init(value);
  // node *nn = append_after(n, 2);
  //
  // print_list(n);
  // node *nnn = append_before(nn, 84);
  //
  // print_list(n);
  //
  // append_after(nnn, 96);
  // print_list(n);
  //
  // delete_node(nnn);
  // print_list(n);
  //
  // node_search(n, 84);
  //
  // return 0;
}
