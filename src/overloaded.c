/* overloaded.c

   written by: Oliver Cordes 2012-08-02
   changed by: Oliver Cordes 2025-06-28

*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "file_stat.h"
#include "logfile.h"
#include "params.h"

#include "whitelist.h"


/* open/close/read/write overloading functions */ 

int open ( const char *filename, int flag, ...)
{
  va_list ap;
  int     res;
  void   *arg;

  va_start( ap, flag );
  arg = va_arg(ap, void *);
  va_end( ap );

  /* do some sanity checks */
  if ( orig_open == NULL )
    {
      printf( "Preinit function open called!\n" );
      orig_open = dlsym( RTLD_NEXT, "open" );
      return orig_open( filename, flag, arg );
    }

  res = orig_open( filename, flag, arg );

  logfile_write( "open: filename='%s' mode=%i (%s) fd=%i", 
		 filename, flag, fstat_flag_str( flag ), res );

  file_stat_open( filename, res );
  return( res );
} 



int close( int fd )
{
  int res;

  /* do some sanity checks */
  if ( orig_close == NULL )
    {
      printf( "Preinit function close called!\n" );
      orig_open = dlsym( RTLD_NEXT, "close" );
      return orig_close( fd );
    }

  res = orig_close( fd );
  logfile_write( "close: fd=%i result=%i", fd, res );
  file_stat_close( fd );
  return res;
}


ssize_t read(int fd, void *buf, size_t count)
{
  ssize_t res;

  res = orig_read( fd, buf, count );
  
  logfile_write( "read: fd=%i req=%i read=%i", fd, count, res ); 
  file_stat_read( fd, res );

  return res;
}


ssize_t write(int fd, const void *buf, size_t count)
{
  ssize_t res;

  res = orig_write( fd, buf, count );

  logfile_write( "write: fd=%i req=%i read=%i", fd, count, res ); 
  file_stat_write( fd, res );

  return res;
}



/* open64/close64/read64/write64 overloading functions */


int open64( const char *filename, int flag, ...)
{
  va_list ap;
  int     res;
  void   *arg;

  va_start( ap, flag );
  arg = va_arg(ap, void *);
  va_end( ap );


  /* do some sanity checks */
  if ( orig_open64 == NULL )
    {
      printf( "Preinit function open64 called!\n" );
      orig_open64 = dlsym( RTLD_NEXT, "open64" );
      return orig_open64( filename, flag, ap );
    }

  res = orig_open64( filename, flag, ap );

  logfile_write( "open: filename='%s' mode=%i (%s) fd=%i", 
		 filename, flag, fstat_flag_str( flag ), res );

  file_stat_open64( filename, res );

  return( res );
}




/* fopen/fclose/fread/fwrite overloading functions */

FILE *fopen ( const char *filename,
	      const char *mode) 
{
  FILE *file;

  /* do some sanity checks */
  if ( orig_fopen == NULL )
    {
      printf( "Preinit function fopen called (filename=%s)!\n", filename );
      orig_fopen = dlsym( RTLD_NEXT, "fopen" );
      return orig_fopen( filename, mode );
    }

  file = orig_fopen( filename, mode );
  if ( file != NULL )
    {
      logfile_write( "fopen: filename='%s' mode='%s' result=OK", filename, mode );
      file_stat_fopen( filename, file );
    }
  else
    {
      logfile_write( "fopen: filename='%s' mode='%s' result=FAIL", filename, mode );
    }

  return file;
}


int fclose( FILE *file )
{
  int res;

  char *s;

  /* do some sanity checks */
  if ( orig_fclose == NULL )
    {
      printf( "Preinit function fclose called!\n" );
      orig_fclose = dlsym( RTLD_NEXT, "fclose" );
      return orig_fclose( file );
    }

  res = orig_fclose( file );
  s = fstat_filename_file( file );
  if ( s == NULL )
    logfile_write( "fclose: result=%i filename='NULL'", res );
  else
    logfile_write( "fclose: result=%i filename='%s'", res, s  );
  file_stat_fclose( file );

  return res;
}



size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t res;
  
  res = orig_fread( ptr, size, nmemb, stream );
  logfile_write( "fread: req=%i read=%i", (size*nmemb), res ); 
  file_stat_fread( stream, res );

  return res;
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t res;

  res = orig_fwrite( ptr, size, nmemb, stream );
  logfile_write( "fwrite: req=%i write=%i", (size*nmemb), res ); 
  file_stat_fwrite( stream, res );

  return res;
}


/* fopen64 overloading functions */

FILE *fopen64 (const char *filename,
	        const char *mode)
{
  FILE *file;

  /* do some sanity checks */
  if ( orig_fopen64 == NULL )
    {
      printf( "Preinit function fopen64 called (filename=%s)!\n", filename );
      orig_fopen64 = dlsym( RTLD_NEXT, "fopen64" );
      return orig_fopen64( filename, mode );
    }

  file = orig_fopen64( filename, mode );
  if ( file != NULL )
    {
      logfile_write( "fopen64: filename='%s' mode='%s' result=OK", filename, mode );
      file_stat_fopen64( filename, file );
    }
  else
    {
      logfile_write( "fopen64: filename='%s' mode='%s' result=FAIL", filename, mode );
    }


  return file;
}



/* opendir/closedir/readdir */

DIR *opendir(const char *name)
{
  DIR *dir;

  /* do some sanity checks */
  if ( orig_opendir == NULL )
    {
      printf( "Preinit function opendir called (name=%s)!\n", name );
      orig_opendir = dlsym( RTLD_NEXT, "opendir" );
      return orig_opendir( name );
    }

  dir = orig_opendir( name );
  if ( dir != NULL )
    {
      logfile_write( "opendir: name='%s' result=OK", name );
    }
  else
    {
      logfile_write( "opendir: name='%s' result=FAIL", name );
    }

  return dir;
}