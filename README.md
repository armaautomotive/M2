M2
==

M2 is an application platform for running programs, written in several languages, that communicate with each other using message passing. 

Programs that include the M2 library then run as modules where they are compiled when changed and run as needed to support load. 

Languages that are currently supported are C/C++ with javascript and others under development.  

Management and visualisation tools allow for modules to be connected together and visualize data flowing between them 
as well as identify error that occur in modules and their connections. 


<!--
A kernel manages the compilation and running of applications as modules who communicate with each other using messages.

Modules are native applications that implement a shared library allowing them to: be compiled and reloaded when source changes (kernel does this), communicate using IPC and report on message and error activity. Application modules can be written in C. Javascript, c++ and Java will follow shortly.

Visualisation of applications (modules), their code for editing, errors and messages flowing between them are currently absent but represent a critical element to the system.
-->

<h3>About:</h3>

The framework allows for systems to be built and maintained with special conideration for secuity, 
reliability and clarity along with real time flexibility for change.

Systems comprising multiple modules can run interrupted while individual modules are changed and reloaded as connections can be designed to re-route traffic. 

Load introspection is enhanced because modules report on message activity and errors. 

Errors in modules can be redirected around alternate paths and bottlenecked modules can spin up multiple instances for more capacity.

New design patterns can be explored for simplified code with the aim of improved security, reliability and maintainability.


<h3>Message API:</h3>

Message passing is asynchronous and implemented through a set of message passing functions.

<table>
  <tr>
    <td> <i>receiveMessages()</i> </td>
    <td>
      - starts a thread to read the modules message queue. 
	Messages are sent to the modules messageHandler() function.
    </td>
    <td> <i>messageHandler()</i> </td>
    <td>
      - abstract function overridden by the module receives incoming messages.
    </td>
    <td> <i>sendMessage()</i> </td>
    <td> 
      - sends a message to one or more other modules directly by name or with 
	help from the kernel if no recipients are found.
    </td>
    <td> <i>sendCallback()</i> </td>
    <td>
      - sends response message to calling module.
    </td>
    <td><i>callbackHandler()</i> </td>
    <td>
      - receives a response message from receiving modules after a sendMessage call.
    </td>
  </tr>
</table>

<h3>Installation:</h3>

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
