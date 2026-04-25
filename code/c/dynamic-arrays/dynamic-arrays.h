#ifndef DYNAMIC_ARRAYS_H
#define DYNAMIC_ARRAYS_H

#include <stdio.h>

typedef struct {
	//pointer to the actual data of the arrays
	int *data;

	//metadata
	int capacity;
	int size;
} Numbers;

Numbers numbers_init(int capacity); void numbers_append(Numbers *n, int value);
void numbers_destroy(Numbers *n);

#endif
