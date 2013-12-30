#include <stdio.h>
#include "module.h"
 
int main(void)
{
	puts("This is a sample test...");
	receiveMessages();

	int i = 100;
	while(i > 0){
		sleep(1);
		i--;
	}

	return 0;
}
