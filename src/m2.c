/**
* M2 App engine kernel
*
* This program will instantiate and monitor all source files
* within child directories that extend the framework header. 
* 
* Each source file is compiled and run in its own process where
* it interacts with other processes through IPC. Source files can
* be modified and will be compiled then deployed automatically with
* no downtime (Thanks the plan). 
*
* Features:
* Source files in child directories become modules and run as services managed by the kernel where they communicate with each other.
* todo: scan files and compile, then run if not allready running.
*	module Naming mechanism for ipc based on file and path, 
*	instance shut down if no memory and used least of all modules.
*       run old version if new one fails repeatedly.
*       compiled binaries are .xxx hidden
*	ipc message passing interface for standardization, plug and play modules.
*	IPC messages stored in module queue. run in order, or multiple instances can be started.
* 	activity(messages and data)/error logging to local module database. (perhaps sqlite)
*	start more module instances if load is high???
*	app - view modules overview, system resources, navigate individual modules including definitions and activity/errors.
*
* Message Passing: 
*   messages are ipc calls and include standard information for plug and play . 
*	messages can be directed to other modules by module name, by function name in the same directory or function name globally, with different performance. 
*
* Why:
*	Real time flexibility to change elements in a running system without interruption.
*	Asyncronus module messages, scales like node.js with possibility to paralelize.
*	Code reusability, developer introspection of live data and errors.
*
* Copyright: 2013
*/

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "vector.h"
#include "m2.h"
#include "strings.h"
#include "processes.h"

//char* substring(char*, int, int);
extern void initalize(void) __attribute__((constructor));
extern void shutdown (void) __attribute__((destructor));

vector modules;

void initalize (void )
{
	//vector_init(&modules);
}

void shutdown(void)
{
	printf("%sM2 shutting down.%s\n", KRED, KNRM);
	//vector_free(&modules);
}


/*
* isFileModule
* 
* Description: a file is a module if it is a supported language and includes the module library.
*/
int isFileModule(char *fileName)
{
	int result = 1;
	char ch, file_name[25];
	FILE *fp;
	fp = fopen(fileName,"r"); // read mode
	if( fp == NULL )
	{
		perror("Error while opening the file.\n");
		return 0;
		//exit(EXIT_FAILURE);
	}
	//printf("The contents of %s file are :\n", fileName);
 	//while( ( ch = fgetc(fp) ) != EOF )
	//	printf("%c",ch);
 
	fclose(fp);
	return result;
}


/**
* needsCompile
*
* Description: returns 1 if the file binary is older than the source.
*/
int needsCompile(char * file)
{
	char *moduleExecutable = NULL;
        char *dot = strrchr(file, '.');
	if (dot && !strcmp(dot, ".c"))
        {
                moduleExecutable = substring(file, 0, strlen(file) - 2);
        
		int sourceTime = 0;
		int binaryTime = 0;
		int errorTime = 0;	
	
		struct stat fst;
                bzero(&fst, sizeof(fst));
                if (stat(file, &fst) == 0) {
			sourceTime = fst.st_mtime;
		}

		bzero(&fst, sizeof(fst));
		if (stat(moduleExecutable, &fst) == 0) { 
			binaryTime = fst.st_mtime;
		}

		
		// error file
		char errorFile[1024];
		strcpy(errorFile, file);
		strcat(errorFile, ".error");
		
		bzero(&fst, sizeof(fst));
		if (stat(errorFile, &fst) == 0) {
			errorTime = fst.st_mtime;
		}	

		free(moduleExecutable);

		// src: 1388700545 err: 1388701822
		//printf(" src: %d err: %d   \n", sourceTime, errorTime);
		if( errorTime > 0 && sourceTime < errorTime ){ // false if error file is newer
			return 0;
		}

		if( sourceTime > binaryTime ){
			return 1;
		}
	}
	// TODO: other languages...
	return 0;
}


/**
* compileModule
*
* Description: compiles module source into binary.
*	Modules can be c and other supported languages.
*	If a source file includes the module header it can be compiled manually.
*	if the directory contains a make file that will be tried first.
*/
int compileModule(char *file)
{
	char *path, *canon_path;
	path = getcwd(NULL, 512);

	char *moduleExecutable = NULL;
	char *dot = strrchr(file, '.');
	if (dot && !strcmp(dot, ".c"))  // Ends with '.c'
	{
		moduleExecutable = substring(file, 0, strlen(file) - 2); 

		char compileCommand[1024];
		strcpy(compileCommand, "gcc -L");
		strcat(compileCommand, path);
		strcat(compileCommand, "/lib -Wl,-rpath=");
		strcat(compileCommand, path);
		strcat(compileCommand, "/lib -I");
		strcat(compileCommand, path);
		strcat(compileCommand, "/src -Wall -o "); 
		strcat(compileCommand, moduleExecutable);
		strcat(compileCommand, " ");
		strcat(compileCommand, "./src/strings.c ");
		strcat(compileCommand, file);
		strcat(compileCommand, " -lmodule -pthread -lrt"); 	
		strcat(compileCommand, " 2>&1");
		printf("\ncompile: %s \n", compileCommand);

		char *response = NULL;
		int r2 = runCommand(compileCommand, &response);
		//printf("response: %d - %s \n", r2, response);
		if(r2)
		{
			printf("Error: %s \n", response);
			char errorFile[1024];
			strcpy(errorFile, file);
			strcat(errorFile, ".error");
			//printf(" error : %s \n", errorFile);
			FILE *fp;
			fp = fopen (errorFile,"w");
			fprintf(fp,"%s\n\n", compileCommand);
			fprintf(fp,"%s\n", response);
			fclose(fp);
		}
		free(response);

		//int r = system(compileCommand);
		//printf("  res: %d  - %d  \n", r,  WIFEXITED(r) );	

		free(moduleExecutable);


		if(r2 == 0)
			return 1;
	}
	// TODO: other languages
	return 0;
}


/**
* isModuleRunning
*
* Description: given a source file check running processes to see if it's running.
*/
int isModuleRunning(char * file)
{
	char *moduleExecutable = NULL;
	char *dot = strrchr(file, '.');
        if (dot && !strcmp(dot, ".c"))  // Ends with '.c'
        {
                moduleExecutable = substring(file, 0, strlen(file) - 2);

		//char *str = strrchr(moduleExecutable, '/');
		//int index = strlen(moduleExecutable)-strlen(str);		
		//char * processName;
		//processName = substring(moduleExecutable, index + 2, strlen(str) );
		char processName[512];
		strcpy(processName, moduleExecutable);

		pid_t pid = proc_find(processName);	
		//printf("process '%s' pid: %d \n", processName, pid);
		
		free(moduleExecutable);
		if (pid != -1) {
			return 1;
		}
	}
	// TODO: other languages
	return 0;
}


/**
* runModule
*
* Description: given a module source file, execute run command for it's binary.
*	TODO: Running should copy the binary to a hidden numbered copy '.name0', '.name1'.
*/
int runModule(char *file)
{
        char *moduleExecutable = NULL;
	char *dot = strrchr(file, '.');
        if (dot && !strcmp(dot, ".c"))  // Ends with '.c' 
        {
                moduleExecutable = substring(file, 0, strlen(file) - 2);

		char compileCommand[1024];
		strcpy(compileCommand, "");
		strcat(compileCommand, moduleExecutable);
		strcat(compileCommand, " &>/dev/null & ");
		//strcat(compileCommand, moduleExecutable);
		//strcat(compileCommand, ".out &");
		//printf("%sRun: %s%s\n", KGRN, compileCommand, KNRM);

		system(compileCommand);
		free(moduleExecutable);
		return 1;
	}
	// TODO: other languages
	return 0;
}


/**
* stopModule
*
* Description: given a module source file stop any processes running.
*/
int stopModule(char *file)
{
	char *moduleExecutable = NULL;
	char *dot = strrchr(file, '.');
        if (dot && !strcmp(dot, ".c"))  // Ends with '.c'
        {
                moduleExecutable = substring(file, 0, strlen(file) - 2);
               
		char processName[512];
                strcpy(processName, moduleExecutable);

                pid_t pid = proc_find(processName);
		if(pid != -1){
			// Shut down.
			//char cmd[512];
			//strcpy(cmd, "kill ");
			//char spid[125];
 
			//strcat(cmd, "",pid);
			char cmd[512];
			sprintf(cmd, "kill %d", pid);	
			printf("KILL: %s\n", cmd);

			system(cmd);	
		}
		free(moduleExecutable); 
        }
	// TODO: other languages
	return 1;
}


/**
* scanFiles
*
* Description: read files in apps directory. Compile and run if nessissary.
*	TODO: monitor activity and errors. Spin up and shutdown multiple module instances if needed.
*/
void scanFiles(char *path)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir(path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0))
			{
				continue;
			}

			char currFile[ 1024 ];
			strcpy(currFile, path);
			strcat(currFile, "/");
			strcat(currFile, dir->d_name);

			//printf("%s type: %d \n", currFile, dir->d_type);
			if(strstr(currFile, ".c") != NULL && isFileModule(currFile)){
				//printf("module: %s \n", currFile);
				int r = 1;				

				// Does it need to be compiled?
				if(needsCompile(currFile)){
					if(isModuleRunning(currFile)){ // Shut down
						stopModule(currFile);
					}
					if(!isModuleRunning(currFile)){	// Can't compile if it's running
						r = compileModule(currFile);
						if(!r){
							// write error log
						}
					}
				}

				// Is it running?
				if(!isModuleRunning(currFile) && r){	
					runModule(currFile);
				}
			}	

			// children
			if(dir->d_type == DT_DIR)
			{   
				scanFiles(currFile);
			}
		}
		closedir(d);
	}
	
}


main()
{
	printf("%sM2 App Engine kernel starting! %s \n", KRED, KNRM);

	int running = 1;
	while(running)
	{	
		scanFiles("./apps");

		//running = 0;
		sleep(1);
	}
	return 0;
}

