#ifndef MY_REALLOC_H
#define MY_REALLOC_H

#include <stdio.h>

typedef struct block_header {
	int is_free;										// 1: is free, 0: not free
	size_t size;										// the actual Segment size
	char padding[4];
	struct block_header *next;			//pointer to the next segmnent
} header_t;

typedef struct {
	header_t *header;
	void *data;
} Segment;

// pointer to the first header 
static header_t *head = NULL;
static header_t *tail = NULL;

header_t *init_header(size_t size);
void *init_segment(size_t size);
void *get_data_ptr(header_t *header);
header_t *get_header_ptr(void *ptr);

void *my_malloc(size_t size);
void *my_calloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t new_size);
void print_segment(void *s);
#endif
