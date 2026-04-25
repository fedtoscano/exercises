#include <stdio.h>
#include <stdlib.h>
#include "dynamic-arrays.h"

int check_valid_index(Numbers *n, int index){
	if(index < 0 ||index > n->size){
		printf("Given index is not valid! (%d)\n", index);
		return -1;
	}

	return 0;
}

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

int numbers_get(Numbers *n, int index){
	if(check_valid_index(n, index) == -1) return -1;
	return n->data[index];
}

void numbers_remove_at(Numbers *n, int index){
	if(check_valid_index(n, index) == -1) return;
	while(index <= n->size - 1) {
		n->data[index] = n->data[index + 1];
		index ++;
	}

	n->size--;
}

void numbers_insert_at(Numbers *n, int value, int index){
	if(check_valid_index(n, index) == -1) return;
	if(n->size + 1 > n->capacity){
		n->capacity *=2;
		n->data = realloc(n->data, n->capacity * sizeof(int));
	}

	if(index == n->size) numbers_append(n, value);
	else {
		n->size++;
		for(int i = n->size -1; i >= index; i--){
			n->data[i + 1] = n->data[i];
		}

		n->data[index] = value;
	}
}

void numbers_print(Numbers *n){
	printf("=====================\n");
	printf("NUMBERS STRUCT INFO\n");
	printf("=====================\n");

	printf("Capacity: %d\n", n->capacity);
	printf("Size: %d\n", n->size);

	int i = 0;
	char *sep = ", ";
	printf("Data: ");
	while(i <= n->size -1){
		printf("%d", n->data[i]);
		if(i != n->size - 1) printf("%s", sep);
		i++;
	}
	printf("\n");
}

void numbers_destroy(Numbers *n){
	free(n->data);
}

int main (void){
	Numbers n = numbers_init(5);
	for(int i = 1; i < 6; i++) numbers_append(&n, i);
	numbers_insert_at(&n, 12, 2);
	numbers_print(&n);
	numbers_remove_at(&n, 2);
	numbers_print(&n);
	return 0;
}
