/* file_stat.c

   written by: Oliver Cordes 2012-07-27
   changed by: Oliver Cordes 2025-07-02


*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "file_stat.h"
#include "logfile.h"
#include "params.h" 


/* global variables */

#define max_entries 65536

/* hold the original variables */

int     (*orig_open)     (const char *filename, int flag, ...)                      = NULL;
FILE*   (*orig_fopen)    (const char *filename, const char *mode)                   = NULL;
int     (*orig_open64)   (const char *filename, int flag, mode_t mode)              = NULL;
FILE*   (*orig_fopen64)  (const char *filename, const char *mode)                   = NULL;

DIR*    (*orig_opendir)  (const char *name)                                         = NULL;






/* some flag rountines */


void append_flag( char *s, char *ns )
{
  if ( strlen( s ) != 0 )
    strcat( s, "|" );
  
  strcat( s, ns );
}

char *fstat_flag_str( int flag )
{
  static char aflags[1000];

  aflags[0] = '\0';

  if (  ( flag & O_RDWR ) == O_RDWR )
    {
      append_flag( aflags, "O_WRONLY" );
      flag &= ~O_WRONLY;
    }
  else
    if ( ( flag & O_WRONLY ) == O_WRONLY )
      {
	append_flag( aflags, "O_WRONLY" );
	flag &= ~O_WRONLY;
      }
    else
      if ( ( flag & O_RDONLY ) == O_RDONLY )
	{
	  append_flag( aflags, "O_RDONLY" );
	  flag &= ~O_RDONLY;
	}

  if ( ( flag & O_APPEND ) == O_APPEND )
    {
      append_flag( aflags, "O_APPEND" );
      flag &= ~O_APPEND;
    }
  if ( ( flag & O_ASYNC ) == O_ASYNC )
    {
      append_flag( aflags, "O_ASYNC" );
      flag &= ~O_ASYNC;
    }
  /*if ( ( flag & O_CLOEXEC ) == O_CLOEXEC )
    {
      append_flag( aflags, "O_CLOEXEC" );
      flag &= ~O_CLOEXEC;
      } */
  /*if ( ( flag & O_CREATE ) == O_CREATE )
    {
      append_flag( aflags, "O_CREATE" );
      flag &= ~O_CREATE;
      } */
  /*if ( ( flag & O_DIRECT ) == O_DIRECT )
    {
      append_flag( aflags, "O_DIRECT" );
      flag &= ~O_DIRECT;
      } */
  /*if ( ( flag & O_DIRECTORY ) == O_DIRECTORY )
    {
      append_flag( aflags, "O_DIRECTORY" );
      flag &= ~O_DIRECTORY;
      } */
  if ( ( flag & O_EXCL ) == O_EXCL )
    {
      append_flag( aflags, "O_EXCL" );
      flag &= ~O_EXCL;
    }
  if ( ( flag & O_LARGEFILE ) == O_LARGEFILE )
    {
      append_flag( aflags, "O_LARGEFILE" );
      flag &= ~O_LARGEFILE;
      }
  /*if ( ( flag & O_NOATIME ) == O_NOATIME )
    {
      append_flag( aflags, "O_NOATIME" );
      flag &= ~O_NOATIME;
      } */
  if ( ( flag & O_NOCTTY ) == O_NOCTTY )
    {
      append_flag( aflags, "O_NOCTTY" );
      flag &= ~O_NOCTTY;
    }
  /*if ( ( flag & O_NOFOLLOW ) == O_NOFOLLOW )
    {
      append_flag( aflags, "O_NOFOLLOW" );
      flag &= ~O_NOFOLLOW;
      } */
  if ( ( flag & O_NONBLOCK ) == O_NONBLOCK )
    {
      append_flag( aflags, "O_NONBLOCK" );
      flag &= ~O_NONBLOCK;
    }
  if ( ( flag & O_SYNC ) == O_SYNC )
    {
      append_flag( aflags, "O_SYNC" );
      flag &= ~O_SYNC;
    }
  if ( ( flag & O_TRUNC ) == O_TRUNC )
    {
      append_flag( aflags, "O_TRUNC" );
      flag &=  ~O_TRUNC;
    }

  return aflags;
}
