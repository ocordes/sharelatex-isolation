/* params.c

   written by: Oliver Cordes 2012-07-31
   changed by: Oliver Cordes 2025-06-26


*/

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>

#include "parse.h"



#define environment_name "IOSTAT_PARAM"


char *iostat_logging_name       = NULL;
int   iostat_trunc_logfile      = 0;
char *iostat_statistics_name    = NULL;
int   iostat_statistics_append  = 0;
int   iostat_closing_statistics = 0;

int   iostat_verbose            = 0;




void params_init( void )
{
  char   *e, *s;
  int    nargc;
  char **nargv;
  int    opt;
  int   i;

  e = getenv( environment_name );
  
  if ( e != NULL )
    {
		// save the environment variable
	  	
		s = strdup(e);
      	parseCommandLine( s, &nargv, &nargc );

      	while ( ( opt = getopt( nargc, nargv, "acs:l:tv" ) ) != -1 ) 
		{
	  		switch( opt )
	    	{
	    		case 'a':
	      			iostat_statistics_append = 1;
	      			break;
	    		case 'c':
	      			iostat_closing_statistics = 1;
	      			break;
	    		case 's':
	      			iostat_statistics_name = strdup( optarg );
	      			break;
	    		case 'l':
	      			iostat_logging_name = strdup( optarg );
	      			break;
	    		case 't':
	      			iostat_trunc_logfile = 1;
	    		case 'v':
	      			iostat_verbose = 1;
	      		break;
	    	}
		}
      
      optind = 0; 

	  free(s);
    }
}



void params_done( void )
{
  if ( iostat_logging_name != NULL )
    free( iostat_logging_name );
  if ( iostat_statistics_name != NULL )
    free( iostat_statistics_name );
}
