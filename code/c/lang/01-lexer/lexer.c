#include <stdio.h>
#include <stdlib.h>

FILE *get_file_stream(char *name) {
  FILE *f = fopen(name, "rb"); // mode: read bynary
  return f;
}

long get_file_size(FILE *f){
  // moves the cursor from start to the end
  // of the file
  int end = fseek(f, 0, SEEK_END);
  if(end == -1){
    return end;
  }
  // how many bytes are there
  long size = ftell(f);
  // return cursor to the start
  fseek(f, 0, SEEK_SET);
  return size;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,"No file was given\n");
    return 2;
  }

  FILE *f = get_file_stream(argv[1]);
  if (f == NULL) {
    perror("Filename is missing!");
    return 2;
  }

  long size_tmp = get_file_size(f);
  if(size_tmp < 0){
    perror("File size not available!");
    fclose(f);
    return 2;
  }

  // recast file size as a size_t 
  // (is the type accepted by malloc())
  size_t size = (size_t)size_tmp;

  // + 1 because we need the '\0' char at the end 
  char *buf = malloc(size + 1);
  if(buf == NULL){
    perror("Cannot allocate memory!");
    fclose(f);
    return 2;
  }

  // reads into the filestream (f), 1 byte at the time
  // and copies into buf for size times
  size_t flen = fread(buf, 1, size, f);
  if(flen < size){
    if(ferror(f) != 0){
      fprintf(
          stderr, 
          "Impossible to read file: some connection error occurred\n"
          );
      fclose(f);
      free(buf);
      return 2;
    }
  }

  // adds the null terminator
  buf[flen] = '\0';

  // prints out the buffer in stdio
  for(size_t i = 0; i < flen; i++){
    printf("%c", buf[i]);
  }

  free(buf);
  fclose(f);
  return 0;
}
