#include <stdio.h>
#include "myrealloc.h"
#include <unistd.h>


void *init_segment(size_t size){
	header_t *header = init_header(size);
	return get_data_ptr(header);
}

header_t *init_header(size_t size){
	size_t total_size = sizeof(header_t) + size;

	header_t *header = sbrk(total_size);
	if(header == (void*)-1) return NULL;

	header->size = size;
	header->is_free = 0;
	header->next = NULL;

	return header;
}

/* Extracts the data from a header_t pointer */
void *get_data_ptr(header_t *header){
	//casting to a char pointer makes its size of 1 byte
	return (char*)header + sizeof(header_t);
}

/* Extracts the header from a data pointer */
header_t *get_header_ptr(void *ptr){
	return (header_t *)((char *)ptr - sizeof(header_t));
}



int main(void){
	void *current_brk = sbrk(0);
	printf("Current BRK: %p\n", current_brk);

	void *new_brk = sbrk(4096);
	if(new_brk == (void*)-1){
		perror("sbrk failed");
		return 1;
	}
	printf("New BRK: %p\n", new_brk);
	return 0;
}
