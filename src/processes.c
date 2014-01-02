#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "processes.h"


/**
* runCommand
*
* Description:
*
* @param: command - string to execute
* @param: output - pointer to string for returning the command response.
*		output must be freed by caller.
* @return: int - zero if successfull.
*/
int runCommand(char *command, char **output)
{
	int result = 0;
	FILE *fp;
	int status;
	char path[1024];
	char temp_output[2048];
	temp_output[0] = 0;

	/* Open the command for reading. */
	fp = popen(command, "r");
	if (fp == NULL) {
		printf("failed \n");
		return 1;
		//printf("Failed to run command\n" );
		//exit;
	}

	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		//printf("- %s", path);
		strcat(temp_output, path);
	}

	*output = (char *)malloc(strlen(temp_output)+1);
	strcpy(*output, temp_output);

	//printf(" --- output: %s \n" , *output);
	//printf(" --- 2 %s \n", temp_output);

	/* close */
	result = pclose(fp)/256;	
	return result;
}


// move this into another file
pid_t proc_find(const char* name)
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");
        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                //printf("%s %s == %s   %s\n", KRED, first, name , KNRM);
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }
    }

    closedir(dir);
    return -1;
}
