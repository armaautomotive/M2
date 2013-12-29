M2
==

Modular Application Framework


M2 is a runtime modular application framework. A kernel manages the compilation and running of applications as modules who communicate with each other using messages.

Modules are native applications that implement a shared library allowing them to: be compiled and reloaded when source changes (kernel does this), communicate using IPC and report on message and error activity. Application modules can be written in C. Javascript, c++ and Java will follow shortly.

Why:

The framework allows for applications to be built and maintained in different ways.
Systems comprising multiple modules can still run while individual modules are changed and reloaded. This reduces the steps required to deploy changes.

Load introspection is enhanced because modules report on message activity and errors. Errors in modules can be redirected around alternate paths and bottlenecked modules can spin up multiple instances for more capacity.

New design patterns for simplified code with the aim of improved security, reliability and maintainability.

Performance. Modules applications run as services with a run loop and functions called when messages arrive. Processes are resident in memory so no threads are spawned on demand.


Message API:

Message passing is asynchronous and implemented through a set of message passing functions.

receiveMessages() - starts a thread to read the modules message queue. Messages are sent to the modules messageHandler() function.
messageHandler() - abstract function overridden by the module receives incoming messages.
sendMessage() - sends a message to one or more other modules directly by name or with help from the kernel if no recipients are found.
callback() - receives a response message from receiving modules after a sendMessage call.


Installation:

Download M2 and extract it into: /var/m2

Start the kernel:
> cd /var/m2
> bin/m2

All module applications are located in m2/apps. Place the following module files.

hello.c
#include <stdio.h>
#include <unistd.h>
#include "../src/module.h"

int callback(char * caller, char * message_name, char * arguments)
{
        puts(“hello module callback");
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
        return 0;
}


world.c
#include <stdio.h>
#include <unistd.h>
#include "../src/module.h"

int messageHandler(char * caller, char * message_name, char * arguments)
{
        printf("World module handler: %s  v2 \n\n", arguments);
        return 1;
}


int main(void)
{
        puts("World starting... ");
        receiveMessages();
        int i = 20;
        while(i > 0){ // Run loop
                sleep(1);
                i--;
        }
        return 0;
}

You should see the modules print output to the terminal.


This project is only in early stage and will not be usable for production work. 
