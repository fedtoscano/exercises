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
	// HEADER - DATA
	// ^ the users gives this pointer
	
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


void *my_malloc(size_t bytes){
	return init_segment(bytes);
}

void my_free(void *ptr){
	header_t *h = get_header_ptr(ptr);
	h->is_free = 1;
}

void print_segment(void *s){
	header_t *header = get_header_ptr(s);
	printf("==============================\n");
	printf("HEADER\n");
	printf("Header size: %zu\n", header->size);
	printf("Header is_free: %d\n", header->is_free);
	printf("------------------------------\n");
	printf("DATA\n");
	printf("Data ptr: %p\n", s);
	printf("==============================\n");
}

int main(void){
	void *ptr = my_malloc(8);
	print_segment(ptr);
	return 0;
}
