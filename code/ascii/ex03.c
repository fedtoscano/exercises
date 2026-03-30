#include <unistd.h>

void write_uppercase_alphabet(void){
	for(int i = 65; i <=90; i++) write(1, &i, 1);

	
	//alternative with char
	char a = 'A';
	while(a <= 'Z'){
		write(1, &a, 1);
		a++;
	}

}

int main(void){
	write_uppercase_alphabet();
	return 0;
}
