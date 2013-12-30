M2
==

M2 is a runtime modular application framework. A kernel manages the compilation and running of applications as modules who communicate with each other using messages.

Modules are native applications that implement a shared library allowing them to: be compiled and reloaded when source changes (kernel does this), communicate using IPC and report on message and error activity. Application modules can be written in C. Javascript, c++ and Java will follow shortly.

Visualisation of applications (modules), their code for editing, errors and messages flowing between them are currently absent but represent a critical element to the system.

<h3>Why:</h3>

The framework allows for applications to be built and maintained in different ways.

Systems comprising multiple modules can run interrupted while individual modules are changed and reloaded. This reduces the steps required to deploy changes.

Load introspection is enhanced because modules report on message activity and errors. 
Errors in modules can be redirected around alternate paths and bottlenecked modules can spin up multiple instances for more capacity.

New design patterns can be explored for simplified code with the aim of improved security, reliability and maintainability.

Performance. Modules applications run as services with a run loop and functions called when messages arrive. 
Processes are resident in memory so no threads are spawned on demand.


<h3>Message API:</h3>

Message passing is asynchronous and implemented through a set of message passing functions.

<i>receiveMessages()</i> - starts a thread to read the modules message queue. Messages are sent to the modules messageHandler() function.

<i>messageHandler()</i> - abstract function overridden by the module receives incoming messages.

<i>sendMessage()</i> - sends a message to one or more other modules directly by name or with help from the kernel if no recipients are found.

<i>sendCallback()</i> - sends response message to calling module.

<i>callbackHandler()</i> - receives a response message from receiving modules after a sendMessage call.


Installation:

Download M2 and extract it into: /var/m2

Start the kernel:
```
> cd /var/m2
> bin/m2
```

All module applications are located in m2/apps. Place the following module files.

hello.c
```
#include <stdio.h>
#include <unistd.h>
#include "module.h"

// Callbacks are not currently implemented
int callbackHandler(char * caller, char * message_name, char * arguments)
{
        puts("hello module callback");
        return 1;
}

int main(void)
{
        puts("Hello starting... ");
        receiveMessages();  // start receiving messages for this module (program).

        int i = 20;
        while(i > 0){
                puts("Hello module. v3");

                sendMessage("world", " world !!!");

                sleep(1);
                i--;
        }

        return 0;
}
```

world.c
```
#include <stdio.h>
#include <unistd.h>
#include "module.h"

// Called for each message this module (program) receives.
int messageHandler(char * caller, char * message_name, char * arguments)
{
        printf("World module handler: %s  v3 \n\n", arguments);
        sendCallback(caller, "message");
        return 1;
}


int main(void)
{
        puts("World starting... ");
        receiveMessages();  // start receiving messages for this module (program).

        int i = 20;
        while(i > 0){ // Run loop (keeps the process in memory)
                //puts("World module.");
                sleep(1);
                i--;
        }

        return 0;
}
```

You should see the modules print output to the terminal. Hello will issue a message to world every second. 
You can edit the module source file while the system is running and when you save you will see the module process shutdown as the update is
compiled and started. 


This project is only in early stage and will not be usable for production applications. 
