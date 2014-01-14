#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "strings.h"

/**
* parse
*    ddd message(bla()laalaa) asd
*/
char * parseMessageTag(char * message, char * tag)
{
	char *pointer = NULL;
	int messageLength = strlen(message);
	int tagLength = strlen(tag);
	if (messageLength > 0 && tagLength > 0)
	{ 
		char *pfound = strstr(message, tag); //pointer to the first character found 	
		if (pfound != NULL)
		{
			int dposfound = (messageLength - strlen(pfound));
			int endPos = -1;
			if(message[dposfound + tagLength] != '(') // tag must be followed by (value)
				return pointer;
			int i = dposfound + tagLength + 1;
			int parenDepth = 1;
			while( i < messageLength )
			{
				char c = message[i];
				//printf(" char: %c  \n", c);
				if(c == '(')
					parenDepth++;	
				if(c == ')')
					parenDepth--;
				
				if(parenDepth == 0)
				{
					endPos = i;
					i = messageLength; // break
				}

				i++;
			}	
			if(endPos > -1){
				pointer = substring(message, dposfound + tagLength + 2, (endPos - (dposfound+tagLength+1) )); 
			}
			//printf(" pos: %d  - %d - %d  end: %d result: %s \n", 
			//	dposfound, strlen(message), strlen(pfound), endPos, pointer );
		}
	}
	return pointer;
}


/**
* substring
*
* Description: return a new string from the input.
* Must call free(var); on returned string.
*/
char *substring(char *string, int position, int length)
{
        char *pointer;
        int c;
        pointer = malloc(length+1);

        if (pointer == NULL)
        {
                printf("Unable to allocate memory.\n");
                //exit(EXIT_FAILURE);
        }

        for (c = 0 ; c < position -1 ; c++)
                string++;
        for (c = 0 ; c < length ; c++)
        {
                *(pointer+c) = *string;
                string++;
        }
        *(pointer+c) = '\0';
        return pointer;
}


/**
* removeChar
*
*/
void removeChar(char str[], char ch)
{
	char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != ch);
    }
    *pw = '\0';
}


/**
* replaceChar
*
*/
//char* replaceChar(char*, char ch, char to)
//{

//	return 0;
//}
