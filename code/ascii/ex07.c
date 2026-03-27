#include <unistd.h>
#include <stdio.h>

void from_ascii_to_digit(char c){
  //converting char to int
	int n = c;
	write(1, &n, 4);
}

//a char is a 1byte number that is mapped to ASCII table
int main(void){
	for(char c = '0'; c <= '9'; c++) from_ascii_to_digit(c);
	return 0;
}
