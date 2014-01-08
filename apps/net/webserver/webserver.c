#include<netinet/in.h>    
#include<stdio.h>    
#include<stdlib.h>    
#include<sys/socket.h>    
#include<sys/stat.h>    
#include<sys/types.h>    
#include<unistd.h>    
#include "module.h"
#include "vector.h"
     
int create_socket;
int new_socket;
vector v;
pthread_t cleanup_thread;
 
struct socket_descriptor {
	void *socket_desc;
	int socket;
	long created;
	int closed;
	long msg_id;
};

struct socket_descriptor* new_socket_descriptor(void *_s,long _msg) {
	struct socket_descriptor* ret = (struct socket_descriptor*)malloc(sizeof(struct socket_descriptor));
	ret->socket_desc = _s; //  *(int*)_s;
	ret->socket = *(int*)_s;
	ret->created = time(NULL);
	ret->closed = 0;
	ret->msg_id = _msg;
	return ret;
}
 
void deconstructor( void ){
	printf("WebServer Deconstructor \n");
	close(create_socket);
	int i;
	for (i = vector_count(&v) - 1; i >= 0; i--) {
		struct socket_descriptor * d = (struct socket_descriptor *)vector_get(&v, i);
		if(d->closed == 0){
			int sock = *(int*)d->socket_desc;
			close(sock);	
		}	
	}	
	pthread_cancel(cleanup_thread);	
	exit(0);
}
 
void *cleanupThread( void * ptr )
{
	while(1)
	{
		//printf("Webserver cleanup  \n");
		int i;
		for (i = vector_count(&v) - 1; i >= 0; i--) {
			struct socket_descriptor * d = (struct socket_descriptor *)vector_get(&v, i);
			
			printf(" d id:%lu  cr:%lu  cl:%d  \n", d->msg_id, d->created, d->closed );        
			if(d->closed == 1){  
				vector_delete(&v, i); // crashes
			}	
		}
		sleep(10);
	}
	return NULL;
}

// Callbacks are not currently implemented
int callbackHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
        //puts("hello module callback");
        //printf("webserver callback  %ul from: %s \n", msg_id, caller);

	// load socket from memory and write callback message. then close.
	//int new_socket = -1; // msg_id;

	struct socket_descriptor * desc = NULL;
	
	//int count = vector_count(&v);
	//printf(" temp %d \n", count);	
	int i;
	for (i = 0; i < vector_count(&v); i++) {
		//printf(" temp %d \n", i);		
		struct socket_descriptor * d = NULL;
		d = (struct socket_descriptor *)vector_get(&v, i);	
		if(d){
			//printf(" find socket %d == %d   \n", d->socket, msg_id);
			if(d->msg_id == msg_id && d->closed == 0){
				//desc = d;
				//desc = &d;
				//desc = new_socket_descriptor(d->socket, d->closed);
				desc = (struct socket_descriptor *)vector_get(&v, i); 

				i = vector_count(&v); // break for loop 
			}
		}
	}
	
	if(desc != NULL){
		int sock = *(int*)desc->socket_desc;
		write(sock, arguments, strlen(arguments));
		//write(new_socket, "hello world!\n", 13);
        	close(sock);
		//vector_delete(&v, 1);
		desc->closed = 1;
	} else {
		write(msg_id, "failed", strlen("failed"));	
		close(msg_id);
	}

        return 1;
}

 
int main() {    
   //int create_socket, new_socket;    
   //int new_socket;
   socklen_t addrlen;    
   int bufsize = 1024;    
   char *buffer = malloc(bufsize);    
   struct sockaddr_in address;    

   vector_init(&v);

   const char *message1 = "Cleanup thread";
   pthread_create(&cleanup_thread, NULL, cleanupThread, (void*) message1);

   receiveMessages();  // start receiving messages for this module (program).
 
   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
      printf("The socket was created\n");
   } else {
      printf("Socket could not be created. \n");
   }

   int yes=1;
   //char yes='1'; // use this under Solaris
   if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      perror("setsockopt");
      exit(1);
   }
    
   address.sin_family = AF_INET;    
   address.sin_addr.s_addr = INADDR_ANY;    
   address.sin_port = htons(10000);    
    
   if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){    
      printf("Binding Socket\n");
   } else {
      printf("Bind Failed! \n");
      exit(0);
   }
    
    
   while (1) {    
      if (listen(create_socket, 10) < 0) {    
         perror("server: listen -");    
         exit(1);    
      }    
    
      if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
         perror("server: accept -");    
         exit(1);    
      }    
    
      if (new_socket > 0){    
         //printf("The Client is connected...\n");
      }

        
      recv(new_socket, buffer, bufsize, 0);    
      //printf("%s\n", buffer);   

	//printf(" new_socket: %d \n ", new_socket );
	
	//socklen_t len = sizeof(address);
	//if (getsockname(new_socket, (struct sockaddr *)&address, &len) != -1)
	//{
	//	printf("port number %d\n", ntohs(address.sin_port));
	//}
	long msg_id = getFreeMsgId(); // ntohs(address.sin_port) + rand(); 	

      struct socket_descriptor * d = new_socket_descriptor(/*socket_handle*/&new_socket, /*msg_id*/msg_id);
      vector_add(&v, d); 

	int s = *(int*) d->socket_desc;
      //long msg_id = 
      int sent = sendMessage("webbroker", /*msg_id*/msg_id, buffer); 
	if(!sent){
		// Store new_socket with msg_id      

	      //write(new_socket, "HTTP/1.1 200 OK\n", 16);
	      //write(new_socket, "Content-length: 46\n", 17);
	      //write(new_socket, "Content-Type: text/html\n\n", 25);
	      //write(new_socket, "<html><body><H1>Hello world</H1></body></html>",46);
      
	      	write(s, "Bello world\n", 12);    
	      	close(s); 
	}   
   }    
   close(create_socket);    
   return 0;    
}
