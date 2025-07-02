/* logfile.c

   written: Oliver Cordes 2012-08-01
   updated: Oliver Cordes 2025-07-02


*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <errno.h>
#include <time.h>
#include <stdarg.h>

#include <unistd.h>

#include "file_stat.h"
#include "params.h"


FILE *logging_logfile = NULL;


char last_timestamp[50];


void logfile_printf( FILE *file, char *s )
{
  fwrite( s, strlen( s ), 1, file );
}


void write_timestamp ( FILE *file )
{
  static struct tm    timep;
  static time_t       times;

  static char last_timestamp[50];
  
  times = time( NULL );
  localtime_r( &times, &timep );
  
  
  snprintf( last_timestamp, 50, "%02i.%02i.%i %02i:%02i.%02i ", timep.tm_mday,
	  timep.tm_mon+1, timep.tm_year+1900, timep.tm_hour,
	  timep.tm_min, timep.tm_sec );
  logfile_printf( file, last_timestamp );
}


void logfile_init ( void )
{

  if ( iostat_logging_name != NULL )
    {
      if ( iostat_trunc_logfile == 1 )
	logging_logfile = orig_fopen( iostat_logging_name, "w+" );
      else
	logging_logfile = orig_fopen( iostat_logging_name, "a+" );
      if ( logging_logfile != NULL )
	{
	  logfile_printf( logging_logfile,
		   "-------------------------------------------------------------------------------\nlogfile opened at: ");
	  write_timestamp( logging_logfile );
	  logfile_printf( logging_logfile, "\n" ); 
	  fflush( logging_logfile );
	}
    }
}


void logfile_done ( void )
{
  FILE *file;

  if ( logging_logfile != NULL )
    {
      file = logging_logfile;
      logging_logfile = NULL; 

      logfile_printf( file,  "logfile closed at: " );
      write_timestamp( file );
      logfile_printf( file,
	       "\n-------------------------------------------------------------------------------\n"
	       ); 
      fclose( file ); 
      } 
}


void logfile_write( char *format, ... )
{
    char dummy[1000];

    va_list ap;

    if ( logging_logfile != NULL )
      {
	va_start( ap, format );
	vsnprintf( dummy, 1000, format, ap );
	va_end( ap );

	/* rewind to end, if someone cut the file for saving operations */
	fseek( logging_logfile, 0, SEEK_END );

	write_timestamp( logging_logfile);
	logfile_printf( logging_logfile, dummy );
	logfile_printf( logging_logfile, "\n" );
	fflush( logging_logfile );
      }

}
