#include <unistd.h>

void from_digit_to_ascii(int digit){
	write(1, &digit, 1);
}

int main(void){
	for(int i = 0; i <=9; i++)
		from_digit_to_ascii(i);
	
	return 0;
}
