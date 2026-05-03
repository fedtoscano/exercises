#include "myrealloc.h"
#include <stdio.h>
#include <unistd.h>


header_t *find_and_remove_free_block(size_t size){
  header_t *current = free_list;
  header_t *prev = NULL;
  
  while(current != NULL){
    if(current->size >= size){
      if(prev == NULL){
        free_list = current->next;
      } else {
        prev->next = current->next;
      }
      current->next = NULL;
      return current;
    }
    prev = current;
    current = current->next;
  }
  
  return NULL;
}

void add_to_free_list(header_t *h){
  h->next = free_list;
  free_list = h;
}

void remove_from_free_list(header_t *h){
  (void)h;
}


/*
 * Initializes a header for a memory segment
 * */
header_t *init_header(size_t size) {
  size_t total_size = sizeof(header_t) + size;

  // CRUCIAL LINE !
  // first, searches in the free_list if there is a block available
  // for the size requested. If is not found, allocates new memory with sbrk();
  //
  // sbrk() is a syscall which asks the kernel for space
  //
  header_t *header = find_and_remove_free_block(size) ?: sbrk(total_size);
  if (header == (void *)-1) return NULL;

  header->size = size;
  header->is_free = 0;
  header->next = NULL;
  header->prev = NULL;

  if (head == NULL) {
    head = header;
    tail = header;
  }
  else {
    tail->next = header; //introduces the new header
    header->prev = tail; //links the new header with the previous tail
    tail = header;       //moves tail pointer to new header
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

void *my_malloc(size_t bytes) { 
  return init_segment(bytes); 
}

void my_free(void *ptr) {
  header_t *h = get_header_ptr(ptr);
  h->is_free = 1;
  add_to_free_list(h);
}

void *my_calloc(size_t size) {
  void *segment = my_malloc(size);

  // this is weird, better would be:
  //
  // memset(segment, 0, size);
  //
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

unsigned long get_segment_size(header_t *h){
  return sizeof(header_t) + h->size;
}

void print_segment(void *s) {
  header_t *header = get_header_ptr(s);
  printf("\n");
  printf("==============================\n");
  printf("HEADER\n");
  printf("header ptr: %p\n", header);
  printf("Header size: %zu\n", sizeof(header_t));
  printf("Header is_free: %d\n", header->is_free);
  printf("------------------------------\n");
  printf("DATA\n");
  printf("Data ptr: %p\n", s);
  printf("Data size: %zu\n", header->size);
  printf("==============================\n");
  printf("sizeof entire segment: %zu\n", get_segment_size(header));
  printf("==============================\n");
  printf("\n");
}

int main(void) {
  void *ptr = my_malloc(34);
  void *ptr2 = my_malloc(34);
  void *ptr3 = my_malloc(34);
  void *ptr4 = my_malloc(34);
  void *ptr5 = my_malloc(34);

  printf("=== INITIAL ALLOCATIONS ===\n");
  print_segment(ptr);
  print_segment(ptr5);
  printf("\nhead -> %p\n", head);
  printf("tail -> %p\n", tail);
  printf("free_list -> %p\n\n", free_list);

  printf("=== FREE ptr, ptr2, ptr3 ===\n");
  my_free(ptr);
  my_free(ptr2);
  my_free(ptr3);
  printf("free_list -> %p\n\n", free_list);

  printf("=== REALLOC ===\n");
  void *ptr_new = my_malloc(34);
  print_segment(ptr_new);
  printf("free_list -> %p\n", free_list);

  return 0;
}
