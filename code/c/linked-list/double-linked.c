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
		if(root->next == NULL){
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
    if (root->next != NULL) printf("%s", sep);
    root = root->next;
  }

  printf("\n");
}

void delete_node(node *n){
  printf("Deleting node with value %d\n", n->value);
  if(n->next) n->next->prev = n->prev;
  if(n->prev) n->prev->next = n->next;
  free(n);
}

int main(void) {
  node *n = node_init(1);
  node *nn = append_after(n, 2);

  print_list(n);
  node *nnn = append_before(nn, 84);

  append_after(nnn, 96);
  print_list(n);

  delete_node(nnn);
  print_list(n);
  
  return 0;
}
