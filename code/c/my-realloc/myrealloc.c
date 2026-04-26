#include <stdio.h>
#include "myrealloc.h"
#include <unistd.h>


void *init_segment(size_t size){
	header_t *header = init_header(size);
	return get_data_ptr(header);
}

header_t *init_header(size_t size){
	size_t total_size = sizeof(header_t) + size;

	// CRUCIAL LINE !
	// sbrk() is a syscall which asks the kernel for space
	
	header_t *header = sbrk(total_size);
	if(header == (void*)-1) return NULL;

	header->size = size;
	header->is_free = 0;
	header->next = NULL;

	return header;
}

/* Extracts the data from a header_t pointer */
void *get_data_ptr(header_t *header){
	//this method receives a pointer to a header and returns
	//a pointer to the actual data
	//to get the data, we must move forward by the size of the header
	
	//casting to a char pointer makes its size of 1 byte
	return (char*)header + sizeof(header_t);
}

/* Extracts the header from a data pointer */
header_t *get_header_ptr(void *ptr){
	//this method receives a pointer to the first element of data
	//and returns a pointer to its header
	//to get the header, we must move back by the size of the header
	//
	// HEADER - DATA
	//          ^ the users gives this pointer
	
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
