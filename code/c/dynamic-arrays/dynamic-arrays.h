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

Numbers numbers_init(int capacity); 
int check_valid_index(Numbers *n, int index);
int numbers_get(Numbers *n, int index);
int numbers_contains(Numbers *n, int value);
int numbers_index_of(Numbers *n, int value);
void numbers_append(Numbers *n, int value);
void numbers_destroy(Numbers *n);
void numbers_remove_at(Numbers *n, int index);
void numbers_insert_at(Numbers *n, int value, int index);
void numbers_print(Numbers *n);
void numbers_shrink(Numbers *n);
void numbers_clear(Numbers *n);

Numbers numbers_filter(Numbers *n, int (*pred)(int));
Numbers numbers_map(Numbers *n, int (*pred)(int));
#endif
