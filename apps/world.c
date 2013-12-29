#include <stdio.h>
#include <unistd.h>
#include "../src/module.h"

int messageHandler(char * caller, char * message_name, char * arguments)
{
	printf("World module handler: %s  v2 \n\n", arguments);
	return 1;
}


int main(void)
{
	puts("World starting... ");
	receiveMessages();

        int i = 20;
        while(i > 0){ // Run loop
                //puts("World module.");
                sleep(1);
                i--;
        }

	return 0;
}

