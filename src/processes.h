#ifndef _PROCESSES_INCLUDE
#define _PROCESSES_INCLUDE
/* --- */


int runCommand(char *command, char **output);
pid_t proc_find(const char* name);

#endif
