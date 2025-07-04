/* params.c

   written by: Oliver Cordes 2012-07-31
   changed by: Oliver Cordes 2025-07-04


*/


#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "parse.h"



#define environment_name "SHAREISO_PARAM"


char *shareiso_logging_name       = NULL;
int   shareiso_trunc_logfile      = 0;
int   shareiso_dry_run            = 0;




void params_init(void)
{
	char   *e, *s;
  	int    nargc;
  	char **nargv;
  	int    opt;
  	int   i;

  	e = getenv(environment_name);
  
  	if (e != NULL)
	{
		// save the environment variable
	  	
		s = strdup(e);
      	parseCommandLine(s, &nargv, &nargc);

      	while ((opt = getopt(nargc, nargv, "l:nt")) != -1) 
		{
	  		switch(opt)
	    	{
	    		case 'l':
	      			shareiso_logging_name = strdup(optarg);
	      			break;
	    		case 't':
	      			shareiso_trunc_logfile = 1;
	    		case 'n':
	      			shareiso_dry_run = 1;
	      		break;
	    	}
		}
      
      optind = 0; 

	  free(s);
    }
}



void params_done(void)
{
	if (shareiso_logging_name != NULL)
		free(shareiso_logging_name);
}
