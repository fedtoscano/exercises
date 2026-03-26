#include <unistd.h>

void print_ascii_digits(int range){
	for(int i = 0; i <= range; i++) write(1, &i, 1);
}

int main(void){
	print_ascii_digits(9);
	return 0;
}
