#include <unistd.h>

int main(void){
	char *s = "Hello World!";
	int i = 0;

	while(s[i] != '\0'){
		write(1, &s[i], 1);
		i++;
	}
	return 0;
}
