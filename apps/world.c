#include <stdio.h>
#include <unistd.h>
#include "../src/module.h"

// Called for each message this module (program) receives.
int messageHandler(char * caller, char * message_name, char * arguments)
{
	printf("World module handler: %s  v2 \n\n", arguments);
	return 1;
}


int main(void)
{
	puts("World starting... ");
	receiveMessages();  // start receiving messages for this module (program).

        int i = 20;
        while(i > 0){ // Run loop (keeps the process in memory)
                //puts("World module.");
                sleep(1);
                i--;
        }

	return 0;
}

