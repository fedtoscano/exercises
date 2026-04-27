#include "myrealloc.h"
#include <stdio.h>
#include <unistd.h>

/*
 * Initializes a header for a memory segment
 * */
header_t *init_header(size_t size) {
  // segment total size (header + data)
  size_t total_size = sizeof(header_t) + size;

  // CRUCIAL LINE !
  // sbrk() is a syscall which asks the kernel for space
  header_t *header = sbrk(total_size);
  if (header == (void *)-1)
    return NULL;

  // sets header's properties
  header->size = size;
  header->is_free = 0;
  header->next = NULL;

  if (head == NULL) {
    head = header;
		tail = header;
  } else {
    tail = header;
  }

  return header;
}

/*
 * Initializes a memory segment
 * */
void *init_segment(size_t size) {
  // initializes header
  header_t *header = init_header(size);

  // returns pointer to data
  return get_data_ptr(header);
}

/* Extracts the data from a header_t pointer */
void *get_data_ptr(header_t *header) {
  // this method receives a pointer to a header and returns
  // a pointer to the actual data
  // to get the data, we must move forward by the size of the header
  //
  //  HEADER - DATA
  //  ^ the users gives this pointer

  // casting to a char pointer makes its size of 1 byte
  return (char *)header + sizeof(header_t);
}

/* Extracts the header from a data pointer */
header_t *get_header_ptr(void *ptr) {
  // this method receives a pointer to the first element of data
  // and returns a pointer to its header
  // to get the header, we must move back by the size of the header
  //
  //  HEADER - DATA
  //           ^ the users gives this pointer

  return (header_t *)((char *)ptr - sizeof(header_t));
}

void *my_malloc(size_t bytes) { return init_segment(bytes); }

void my_free(void *ptr) {
  header_t *h = get_header_ptr(ptr);
  h->is_free = 1;
}

void *my_calloc(size_t size) {
  void *segment = my_malloc(size);
  for (size_t i = 0; i < size; i++)
    ((char *)segment)[i] = 0;
  return segment;
}

void *my_realloc(void *ptr, size_t new_size) {
  // if ptr is NULL, acts like a malloc()
  if (ptr == NULL)
    return my_malloc(new_size);

  // if new_size is 0, acts like a free()
  if (new_size == 0) {
    my_free(ptr);
    return NULL;
  }

  // initalizes new segment
  void *new_ptr = my_malloc(new_size);
  if (!new_ptr)
    return NULL;

  header_t *old_header = get_header_ptr(ptr);
  size_t copy_size =
      (old_header->size < new_size) ? old_header->size : new_size;

  // this char* casting allows us to copy byte by byte
  //	(sizeof char = 1byte);
  char *source = ptr;
  char *dest = new_ptr;

  // copies data into the new segment
  for (size_t i = 0; i < copy_size; i++)
    dest[i] = source[i];
  my_free(ptr);

  return new_ptr;
}

void print_segment(void *s) {
  header_t *header = get_header_ptr(s);
  printf("\n");
  printf("==============================\n");
  printf("HEADER\n");
  printf("Header size: %zu\n", header->size);
  printf("Header is_free: %d\n", header->is_free);
  printf("------------------------------\n");
  printf("DATA\n");
  printf("Data ptr: %p\n", s);
  printf("==============================\n");
  printf("\n");
}

int main(void) {
  void *ptr = my_malloc(8);
  print_segment(ptr);
  my_free(ptr);

  void *realloc_ptr = my_realloc(ptr, 20);
  print_segment(realloc_ptr);

  my_free(realloc_ptr);

  void *calloc_segment = my_calloc(35);
  print_segment(calloc_segment);

  my_free(calloc_segment);
  return 0;
}
