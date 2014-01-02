#include <stdio.h>
#include <unistd.h>
#include "module.h"

// Callbacks are not currently implemented
int callbackHandler(char * caller, char * message_name, char * arguments)
{
        puts("hello module callback");                    
        return 1;
}

int main(void)
{
	puts("Hello starting... ");
	receiveMessages();  // start receiving messages for this module (program).

        int i = 10;
        while(i > 0){
		puts("Hello module. v4");

		sendMessage("world", " world !!!");

                sleep(1);
                i--;
        }

	return 0;
}
