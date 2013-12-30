#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>

#ifndef module_h__
#define module_h__

//extern int running;
pthread_t message_reader_thread;
extern char *__progname;

extern void initalize(void) __attribute__((constructor)); 
extern void shutdownModule (void) __attribute__((destructor));

extern void sendMessage(char * name, char * arguments);
extern int receiveMessages();
extern void *messageReader( void *ptr );
extern int messageHandler(char * caller, char * message_name, char * arguments);

extern int sendCallback(char * name, char * arguments);
extern int callbackHandler(char * caller, char * message_name, char * arguments);
 
#endif  // module_h__




