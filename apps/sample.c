#include <stdio.h>
#include "../src/m2.h"
 
int main(void)
{
	puts("This is a shared library test...");
	foo();

	int i = 100;
	while(i > 0){
		sleep(1);
		i--;
	}

	return 0;
}
