#ifndef MY_REALLOC_H
#define MY_REALLOC_H

#include <stdio.h>

typedef struct block_header {
	int is_free;										// 1: is free, 0: not free
	size_t size;										// the actual Segment size
	char padding[4];
	struct block_header *next;			//pointer to the next segmnent
	struct block_header *prev;			//pointer to the previous segmnent
} header_t;

typedef struct {
	header_t *header;
	void *data;
} Segment;

static header_t *head = NULL;					//first element
static header_t *tail = NULL;					//last element
static header_t *free_list = NULL;		///empty headers (for re-use)

header_t *init_header(size_t size);
void *init_segment(size_t size);
unsigned long get_segment_size(header_t *h);
void *get_data_ptr(header_t *header);
header_t *get_header_ptr(void *ptr);

void *my_malloc(size_t size);
void *my_calloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t new_size);
void print_segment(void *s);

header_t *find_free_block(size_t size);
void add_to_free_list(header_t *h);
void remove_from_free_list(header_t *t);
#endif
