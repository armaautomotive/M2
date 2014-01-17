#include <stdio.h>
#include <unistd.h>
#include "module.h"
     
// Called for each message this module (program) receives.
int messageHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
	printf("World module handler: %ul %s  from: %s v3 \n\n", msg_id, arguments, caller);
	
	sendCallback(caller, msg_id, "message");
 	
	return 1;
}

int main(void)
{
//	puts("World starting... ");
	receiveMessages();  // start receiving messages for this module (program).

        int i = 10;
        while(i > 0){ // Run loop (keeps the process in memory)
                //puts("World module.");
                sleep(1);
                i--;
        }

	return 0;
}

