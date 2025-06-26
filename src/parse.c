/* parse.c
   
   written by: Oliver Cordes 2012-07-31
   changed by: Oliver Cordes 2025-06-26


*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char parse_name[] = "parselib";

void parseCommandLine(char* cmdLine, char*** argv, int* argc)
{
    int count = 1;

    char *s;

    /* count the number of arguments from a duplicate string */
    s = strdup(cmdLine);
    
    char* match = strtok( s, " ");
    while(match != NULL){
        count++;
        match = strtok( NULL, " ");
    }
    free( s );
    
    /* allocate array */
    *argv = malloc( sizeof(char*) * ( count+1 ) );
    (*argv)[count] = 0;
    /* copy a placeholder for the program name */
    **argv = (char*) parse_name;  

    /* split directly the cmdLine */
    if (count > 1){
        int i=1;
        match = strtok(cmdLine, " ");
        //printf("hallo: %s\n", match);
        do{
	  (*argv)[i++] = match;
            match = strtok(NULL, " ");
            //printf("hallo: %s\n", match);
        } while(match != NULL);
     }

    *argc = count;
}
