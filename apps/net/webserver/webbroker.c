#include <stdio.h>
#include <unistd.h>
#include "module.h"
   
// Called for each message this module (program) receives.
int messageHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
//	printf("World module handler: %ul %s  from: %s v3 \n\n", msg_id, arguments, caller);
	
	//sleep(5);

	char * msg = " asd       asdasd asd as dasd "; 
	//sprintf(msg, "message %s %lu \n", arguments, msg_id);

	sendCallback(caller, msg_id, msg); 
	
	return 1;
}

int main(void)
{
//	puts("World starting... ");
	receiveMessages();  // start receiving messages for this module (program).

        int i = 100;
        while(i > 0){ // Run loop (keeps the process in memory)
                //puts("World module.");
                sleep(1);
                i--;
        }

	return 0;
}

