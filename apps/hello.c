#include <stdio.h>
#include <unistd.h>
#include "module.h"

// Callbacks are not currently implemented
int callbackHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
        //puts("hello module callback");                    
        printf(" callback  %ul from: %s \n", msg_id, caller);

	//sendMessage("world", "AHAHAHAAHAHAHAHAH");

	return 1;
}

int main(void)
{
	puts("Hello starting... ");
	receiveMessages();  // start receiving messages for this module (program).

        int i = 10;
        while(i > 0){
		puts("Hello module. v4");

		sendMessage("world", -1, "world !!!");

                sleep(1);
                i--;
        }

	return 0;
}
