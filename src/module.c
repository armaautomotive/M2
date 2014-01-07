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
long sendMessage(char * name, long msg_id, char * arguments)
{
	if(msg_id == -1)
		msg_id = ++message_id;
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
	//CHECK((mqd_t)-1 != mq);
	if((mqd_t)-1 == mq)
	{
		// ask kernel for destination
		printf(" sendMessage destination not found... \n");
		return -1;
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
        CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

	/* cleanup */
	CHECK((mqd_t)-1 != mq_close(mq));

	return message_id;
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

        /* open the mail queue */
        mq = mq_open(to_queue_name, O_WRONLY);
        CHECK((mqd_t)-1 != mq);

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
	attr.mq_maxmsg = 100;
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

