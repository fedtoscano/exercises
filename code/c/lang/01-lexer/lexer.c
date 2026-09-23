#include <stdio.h>
#include <stdlib.h>

FILE *get_file_stream(char *name) {
  FILE *f = fopen(name, "rb");
  return f;
}

long get_file_size(FILE *f){
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
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

  size_t size = (size_t)size_tmp;

  char *buf = malloc(size + 1);
  if(buf == NULL){
    perror("Cannot allocate memory!");
    return 2;
  }

  size_t flen = fread(buf, 1, size, f);
  buf[flen] = '\0';

  for(size_t i = 0; i < flen; i++){
    printf("%c", buf[i]);
  }

  free(buf);
  fclose(f);
  return 0;
}
