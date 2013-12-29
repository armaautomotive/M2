/**
*
*
*
*/

#include<stdio.h>
#include<string.h>
 
main()
{
   char source[] = "C program";
   char destination[50];
 
   strcpy(destination, source);
 
   printf("Source string: %s\n", source);
   printf("Destination string: %s\n", destination);

	sleep(120);
 
   return 0;
}
