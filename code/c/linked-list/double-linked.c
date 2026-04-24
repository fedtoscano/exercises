#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int value;
	struct node *prev;
	struct node *next;
} node;

node *node_init(int value){
	node *n = malloc(sizeof(node));
	if(n == NULL){
		printf("n points to NULL!\n");
		return NULL;
	}

	n->value = value;
	n->prev =  NULL;
	n->next =  NULL;
	return n;
}

node *append_before(node *to, int value);
node *append_after(node *to, int value);
node *node_search(int value);
void print_node(node *n);
void delete_node(node *n);

int main(void){
	node *n = node_init(0);

	return 0;
}
