#include <unistd.h>

void from_digit_to_ascii(int digit){
	char ascii = digit + '0';
	write(1, &digit, 1);
}

<<<<<<< HEAD
=======
void from_ascii_to_digit(char c){
	int digit = c - '0';
	write(1, &digit, 1);
}

>>>>>>> 51f0839 (update)
int main(void){
	for(int i = 0; i <=9; i++)
		from_digit_to_ascii(i);
	
	return 0;
}
