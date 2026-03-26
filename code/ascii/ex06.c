#include <unistd.h>

void from_digit_to_ascii(int digit){
	write(1, &digit, 1);
}

void from_ascii_to_digit(char c){
	write(1, (int*)c, 1);
}

int main(void){
	from_digit_to_ascii(76);
	from_ascii_to_digit('a');
	return 0;
}
