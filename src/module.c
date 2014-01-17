#include <signal.h>
#include "module.h"
#include "common.h"
#include "strings.h"
#include "vector.h"

/**
* initalize
*
* Description: Called by the constructor. 
*/
void initalize( void )
{
	//puts("library module initalize");
	//running = 0;
	//message_reader_thread = NULL;
	message_id = 0;

	// This is tricky, starting the receive thread may mean that the lib is not linked to the module if no calls to it are made.	
	//receiveMessages();	

	if (signal (SIGINT, termination_handler) == SIG_IGN)
		signal (SIGINT, SIG_IGN);
	if (signal (SIGHUP, termination_handler) == SIG_IGN)
		signal (SIGHUP, SIG_IGN);
	if (signal (SIGTERM, termination_handler) == SIG_IGN)
		signal (SIGTERM, SIG_IGN);	
}

void termination_handler (int signum)
{
	//printf("terminate %d \n", signum);
	deconstructor();
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
* @return: long message id. Use to match callback.
*/
//long sendMessage(char * name, char * arguments)
//{
//	return sendMessage(name, ++message_id, arguments);
//}
int sendMessage(char * name, long msg_id, char * arguments)
{
	int result = 0;
	if(msg_id == -1)
		msg_id = ++message_id;
	char queue_name[512]; //argv[0];
	strcpy(queue_name, __progname);
	//printf(" queue name: %s \n", queue_name);
	// TODO: find destination pipe file for a given function name using the kernel.

	char to_queue_name[512];
	strcpy(to_queue_name, "/");
	strcat(to_queue_name, name);

	// Fails if not admin?????

	mqd_t mq;
	char buffer[MAX_SIZE];

	int    flags = O_WRONLY; //  | O_CREAT; // O_WRONLY   O_RDWR 
        mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

        struct mq_attr attr;
        attr.mq_flags   = 0;
        attr.mq_maxmsg  = 10;
        attr.mq_msgsize = 1024;
        attr.mq_curmsgs = 0;

        mq = mq_open(to_queue_name, flags, mode, &attr);
	//mq = mq_open(to_queue_name, O_WRONLY, 0666, NULL);	
	//mq = mq_open(to_queue_name, O_WRONLY);	
	if(mq == -1)
	{
		// ask kernel for destination
		printf("  sendMessage %s mq_open %s (%d) \n", to_queue_name, strerror(errno), errno);
		return 0;
	}

	// Format message: caller(CALLER_MODULE_NAME) recipient(TARGET_MODULE_NAME) message(MESSAGE_DATA) 
        memset(buffer, 0, MAX_SIZE);

	strcpy(buffer, "from(");
	strcat(buffer, queue_name);
	strcat(buffer, ") ");

	strcat(buffer, "to(");
	strcat(buffer, name);
	strcat(buffer, ") ");

	strcat(buffer, "msg_id(");
        char msg_id_buffer[20];
        sprintf(msg_id_buffer, "%lu", msg_id);	
        strcat(buffer, msg_id_buffer);
        strcat(buffer, ") ");	

        strcat(buffer, "message(");
        strcat(buffer, arguments);
        strcat(buffer, ") ");

        /* send the message */
        if(0 <= mq_send(mq, buffer, MAX_SIZE, 0)){
		result = 1;
	}

	/* cleanup */
	CHECK((mqd_t)-1 != mq_close(mq));

	return result;
} 


long getFreeMsgId()
{
	return ++message_id; 
}
 

/**
* sendCallback
*
*/
int sendCallback(char * name, long msg_id, char * arguments)
{
	char queue_name[512]; //argv[0];
        strcpy(queue_name, __progname);
        //printf(" queue name: %s \n", queue_name);
        // TODO: find destination pipe file for a given function name using the kernel.

        char to_queue_name[512];
        strcpy(to_queue_name, "/");
        strcat(to_queue_name, name);

        mqd_t mq;
        char buffer[MAX_SIZE];

	//int    flags = O_RDWR | O_CREAT;
	//mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	
	//struct mq_attr attr;
	//attr.mq_flags   = 0;
	//attr.mq_maxmsg  = 10;
	//attr.mq_msgsize = 1024;
	//attr.mq_curmsgs = 0;

	//mq = mq_open(to_queue_name, flags, mode, &attr);
       	mq = mq_open(to_queue_name, O_WRONLY, 0666, NULL); 
	/* open the message queue */
        //mq = mq_open(to_queue_name, O_WRONLY);
	//CHECK((mqd_t)-1 != mq);
	if(mq == -1){
		printf("  sendCallback %s %s (%d) \n", to_queue_name, strerror(errno), errno);
		return 0;
	}

        memset(buffer, 0, MAX_SIZE);
        //strcpy(buffer, arguments);

	strcpy(buffer, "from(");
        strcat(buffer, queue_name);
        strcat(buffer, ") ");

        strcat(buffer, "to(");
        strcat(buffer, name);
        strcat(buffer, ") ");

        strcat(buffer, "msg_id(");
        char msg_id_buffer[20];
        sprintf(msg_id_buffer, "%lu", msg_id);
        strcat(buffer, msg_id_buffer);
        strcat(buffer, ") ");

        strcat(buffer, "callback(");
        strcat(buffer, arguments);
        strcat(buffer, ") ");

        /* send the message */
        CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

        /* cleanup */
        CHECK((mqd_t)-1 != mq_close(mq));
	return 1;
}


/**
* callbackHandler
*
* Description: This is an abstract function and should be overidden by modules. 
* 	It is called when a message sent returns a response from the receiving module.
*	This allows for asyncronus module development.
*
* Params: 
*/
int callbackHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
	//puts("library callback");	
	return 1;
}


/**
* shutdownModule
*
* Description: internal function 
*/
void shutdownModule( void )
{
	//puts("library module shutdown");
	pthread_cancel(message_reader_thread);
}


void deconstructor( void )
{
	// Abstract deconstructor implemented by modules, called when shutting down.
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
* receive messages  Thread  
*
* call to ...
*/
void *messageReader( void *ptr )
{
	char my_queue_name[512]; //argv[0];
        strcpy(my_queue_name, "/");
	strcat(my_queue_name, __progname);	

	mqd_t mq;
	struct mq_attr attr;
	char buffer[MAX_SIZE + 1];
	int must_stop = 0;

	/* initialize the queue attributes */
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10; // > 10 requires admin
	attr.mq_msgsize = MAX_SIZE;
	attr.mq_curmsgs = 0;

	/* create the message queue */

	mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;	

	//mq = mq_open(my_queue_name, O_CREAT | O_RDONLY, 0666, &attr); // O_RDONLY = ok   O_RDWR = fails
	mq = mq_open(my_queue_name, O_CREAT | O_RDONLY, mode, &attr);	
	CHECK((mqd_t)-1 != mq);
	printf(" queue opened: %s \n ", my_queue_name);

	do {
		ssize_t bytes_read;

		/* receive the message */
		bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
		CHECK(bytes_read >= 0);

		buffer[bytes_read] = '\0';
		
		// TODO: parse and supply correct parameters to handler.
		//char caller[100] = "";
		//char *message_name;
		//message_name = parseMessageTag(buffer, "message");
		char * to = NULL;
		char * from = NULL;
		char * message = NULL;
		char * callback = NULL;
		char * msg_id_s = NULL;
		long msg_id = 0;
		message = parseMessageTag(buffer, "message");
		callback = parseMessageTag(buffer, "callback");
		to = parseMessageTag(buffer, "to");
		from = parseMessageTag(buffer, "from");
		msg_id_s = parseMessageTag(buffer, "msg_id");
		msg_id = atol (msg_id_s);
		//free(message_name); 

		//printf("msg: %s \n", (char*)message);
		if( message != NULL )
		{
			messageHandler(from, to, msg_id, message);
		} else if ( callback != NULL )
		{
			callbackHandler(from, to, msg_id, callback);
		}
		//printf("Received: %s\n", buffer);
		free(message);
		free(callback);
		free(to);
		free(from);
		free(msg_id_s);
	} while (!must_stop);

	/* cleanup */
	CHECK((mqd_t)-1 != mq_close(mq));	
	CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

	return NULL;
}


/**
* messageHandler
*
* Description: This is an abstract function and should be overidden by modules. 
*	It is called by messages received in the current module.
*/
int messageHandler(char * caller, char * message_name, long msg_id, char * arguments)
{
	//printf("lib messageHandler: abstract function: received: %s \n", arguments);
	return 0;
}

