#include <unistd.h>

int is_printable(int i){
	return i >=32 && i <=126;
}

void write_printable_characters(void){
	//assuming that the printable characters are from 32(space) to 126(~);
	for(int i = 0; i <= 127; i++)
		if(is_printable(i)) write(1, &i, 1);
	
}
int main(void){
	write_printable_characters();
	return 0;
}
