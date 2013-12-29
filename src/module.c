#include "module.h"
#include "common.h"

/**
* initalize
*
* Description: Called by the constructor. 
*/
void initalize( void )
{
	puts("library module initalize");
	//running = 0;
	//message_reader_thread = NULL;
	
	//receiveMessages();	

	
}

/**
* sendMessage
*
* Description: send a message by name, if not found call kernel to search for best.
* 	* - send to all modules in apps/system.
*	.* - send to all modules in child directories of current module.
*	dir.path.modulename - call module by name
*
* Payload: caller, target, message
*/
void sendMessage(char * name, char * arguments)
{
	//puts("sendMessage");
	char queue_name[512]; //argv[0];
	strcpy(queue_name, __progname);
	//printf(" queue name: %s \n", queue_name);
	// TODO: find destination pipe file for a given function name using the kernel.

	char to_queue_name[512];
	strcpy(to_queue_name, "/");
	strcat(to_queue_name, name);

	mqd_t mq;
	char buffer[MAX_SIZE];

	/* open the mail queue */
	mq = mq_open(to_queue_name, O_WRONLY);	
	CHECK((mqd_t)-1 != mq);

        memset(buffer, 0, MAX_SIZE);
	strcpy(buffer, arguments);	

        /* send the message */
        CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

	/* cleanup */
	CHECK((mqd_t)-1 != mq_close(mq));

	/*
	// FIFO
	FILE *fp;
        if((fp = fopen("test_fifo", "w")) == NULL)
        {
                printf("SHIT");
        }
	fprintf(fp, "%s", arguments);
	fflush(fp); // flush any buffered IO to the pipe
	fclose(fp);
	*/
} 
 
void foo(void)
{
	puts("Hello, I'm a shared library");
	callback(NULL, NULL, NULL);
}


/**
* callback
*
* Description: This is an abstract function and should be overidden by modules. 
* 	It is called when a message sent returns a response from the receiving module.
*	This allows for asyncronus module development.
*
* Params: 
*/
int callback(char * caller, char * message_name, char * arguments)
{
	puts("library callback");	
	return 1;
}


void shutdownModule( void )
{
	//puts("library module shutdown");
	pthread_cancel(message_reader_thread);
}

/**
*
* Description: called by modules to start receiving messages.
* 	Runs a thread to accept IPC messages and call the messageHandler function of the module.
*/
int receiveMessages()
{
	//running = 1;
	//pthread_t thread1;

	const char *message1 = "Thread 1";
	int r = pthread_create( &message_reader_thread, NULL, messageReader, (void*) message1);

	return r;
}

/**
* receive messages   - receiveMessages()
*
* call to ...
*/
//void *print_message_function( void *ptr );

void *messageReader( void *ptr )
{
	// TODO: read fifo pipe file from module name
	char my_queue_name[512]; //argv[0];
        strcpy(my_queue_name, "/");
	strcat(my_queue_name, __progname);	

	mqd_t mq;
	struct mq_attr attr;
	char buffer[MAX_SIZE + 1];
	int must_stop = 0;

	/* initialize the queue attributes */
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_SIZE;
	attr.mq_curmsgs = 0;

	/* create the message queue */
	mq = mq_open(my_queue_name, O_CREAT | O_RDONLY, 0644, &attr);
	CHECK((mqd_t)-1 != mq);

	do {
		ssize_t bytes_read;

		/* receive the message */
		bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
		CHECK(bytes_read >= 0);

		buffer[bytes_read] = '\0';
		//if (! strncmp(buffer, MSG_STOP, strlen(MSG_STOP)))
		//{
			//must_stop = 1;
		//}
		//else
		//{
			messageHandler(NULL, NULL, buffer);
			//printf("Received: %s\n", buffer);
		//}
	} while (!must_stop);

	/* cleanup */
	CHECK((mqd_t)-1 != mq_close(mq));	
	CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

	/*
	// FIFO
	FILE *fd = fopen ("test_fifo", "r");
	char buffer[1024];
	while (fgets(buffer, 50, fd) != NULL)
		puts(buffer);

	fclose(fd);
	*/
	return NULL;
}


/**
* messageHandler
*
* Description: This is an abstract function and should be overidden by modules. 
*
*
*/
int messageHandler(char * caller, char * message_name, char * arguments)
{
	printf("lib messageHandler: abstract function: received: %s \n", arguments);
	return 1;
}

