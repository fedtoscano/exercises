#include <stdlib.h>
#include "dynamic-arrays.h"

Numbers numbers_init(int capacity){
	Numbers N = { 0 };
	N.capacity = capacity;
	N.data = malloc(N.capacity * sizeof(int));
	N.size = 0;
	return N;
}

void numbers_append(Numbers *n, int value){
	if(n->size >= n->capacity){
		//if the maximum capacity is reached, double the capacity
		n->capacity *=2;
		n->data = realloc(n->data, n->capacity * sizeof(int));
	}

	/*
	 * This is the crucial step of manually adding a value to an array 
	 * in C. The operations are executed in this order:
	 *		1. n->data[n->size] = value
	 *				assigns the value to the current index
	 *		2. size ++
	 *				increments the size value
	 * */
	n->data[n->size++] = value;
}

void numbers_destroy(Numbers *n){
	free(n->data);
}

int main (void){
	return 0;
}
