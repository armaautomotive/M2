#include <stdio.h>
#include <unistd.h>
#include "../src/module.h"

int callback(char * caller, char * message_name, char * arguments)
{
        puts("hello module callback");                    
        return 1;
}

int main(void)
{
	puts("Hello starting... ");
	receiveMessages();

        int i = 20;
        while(i > 0){

		puts("Hello module. v2");

		sendMessage("world", " world !!!");

                sleep(1);
                i--;
        }

	//shutdownModule();
	return 0;
}
