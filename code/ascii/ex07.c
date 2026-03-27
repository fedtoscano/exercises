#include <unistd.h>
#include <stdio.h>

void from_ascii_to_digit(char c){
	int n = c;
	write(1, &n, 4);
}

int main(void){
	for(int i = 0; i <=9; i++){
		char a = i;
		printf("%c\n", a);
		from_ascii_to_digit(a);
	}
	return 0;
}
