#include<netinet/in.h>    
#include<stdio.h>    
#include<stdlib.h>    
#include<sys/socket.h>    
#include<sys/stat.h>    
#include<sys/types.h>    
#include<unistd.h>    
#include "module.h"
   
//vector v;

// Callbacks are not currently implemented
int callbackHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
        //puts("hello module callback");
        //printf("webserver callback  %ul from: %s \n", msg_id, caller);

	// load socket from memory and write callback message. then close.
	int new_socket = msg_id;

	write(new_socket, arguments, strlen(arguments));

	//write(new_socket, "hello world!\n", 13);
        close(new_socket);

        return 1;
}
 
int main() {    
   int create_socket, new_socket;    
   socklen_t addrlen;    
   int bufsize = 1024;    
   char *buffer = malloc(bufsize);    
   struct sockaddr_in address;    

   //vector_init(&v);


   receiveMessages();  // start receiving messages for this module (program).
 
   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
      printf("The socket was created\n");
   }
    
   address.sin_family = AF_INET;    
   address.sin_addr.s_addr = INADDR_ANY;    
   address.sin_port = htons(10000);    
    
   if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){    
      printf("Binding Socket\n");
   }
    
    
   while (1) {    
      if (listen(create_socket, 10) < 0) {    
         perror("server: listen");    
         exit(1);    
      }    
    
      if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
         perror("server: accept");    
         exit(1);    
      }    
    
      if (new_socket > 0){    
         printf("The Client is connected...\n");
      }
        
      recv(new_socket, buffer, bufsize, 0);    
      printf("%s\n", buffer);   

      //vector_add(&v, new_socket); 
      long msg_id = sendMessage("world", new_socket, buffer); 
      // Store new_socket with msg_id      

      //write(new_socket, "HTTP/1.1 200 OK\n", 16);
      //write(new_socket, "Content-length: 46\n", 17);
      //write(new_socket, "Content-Type: text/html\n\n", 25);
      //write(new_socket, "<html><body><H1>Hello world</H1></body></html>",46);
      
      //write(new_socket, "hello world\n", 12);    
      //close(new_socket);    
   }    
   close(create_socket);    
   return 0;    
}
