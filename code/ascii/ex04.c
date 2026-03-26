#include <unistd.h>

void print_lowercase_alphabet(void){
	for(int i = 97; i <=122; i++) write(1, &i, 1);
}

int main(void){
	print_lowercase_alphabet();
	return 0;
}
