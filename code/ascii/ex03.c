#include <unistd.h>

void write_uppercase_alphabet(void){
	for(int i = 65; i <=90; i++) write(1, &i, 1);
}

int main(void){
	write_uppercase_alphabet();
	return 0;
}
