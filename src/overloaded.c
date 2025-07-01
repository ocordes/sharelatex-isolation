/* overloaded.c

   written by: Oliver Cordes 2012-08-02
   changed by: Oliver Cordes 2025-06-30

*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>

#include "file_stat.h"
#include "logfile.h"
#include "params.h"

#include "isolation.h"
#include "whitelist.h"


int is_flag_readable( int flag )
{
  return ( ((flag & O_RDONLY) == O_RDONLY) || ((flag & O_RDWR) == O_RDWR));
}


int is_flag_writable( int flag )
{
  return (((flag & O_WRONLY) == O_WRONLY) || ((flag & O_RDWR) == O_RDWR));
} 


int allowed_to_read( const char *filename )
{
  if ( read_whitelist == NULL ) {
    logfile_write( "read_whitelist is NULL, allowing all read access" );
    return 1; // no whitelist, so allow everything
  }

  if ( whitelist_check( read_whitelist, filename ) )
  {
    logfile_write( "read access to '%s' is granted", filename );   
    return 1;
  }

  logfile_write( "read access to '%s' is denied", filename );
  return 0;
}


int allowed_to_write( const char *filename )
{
  if ( write_whitelist == NULL )
    return 1; // no whitelist, so allow everything

  if ( whitelist_check( write_whitelist, filename ) )
  {
    logfile_write( "write access to '%s' is granted", filename ); 
    return 1;
  }

  logfile_write( "write access to '%s' is denied", filename );
  return 0;
}


int check_granted( const char *filename, int flag )
{
  char *dirc;
  char *basec;
  char *dirfilename;
  char *basefilename;
  char *realpathname;
  char *realfilename;

  int  granted = 0;


  // check if the filename is NULL
  if ( filename == NULL )
  {
    logfile_write( "check_granted: filename is NULL, access denied" );
    return 0; // access denied
  }
  
  dirc = strdup(filename);
  basec = strdup(filename);

  dirfilename = dirname(dirc);
  basefilename = basename(basec);

  // dirfilename must be a real directory
  realpathname = realpath(dirfilename, NULL);
    

  asprintf(&realfilename, "%s/%s", realpathname, basefilename);
  free(dirc);
  free(basec);
  logfile_write( "check_granted: checking access for '%s' (realpath='%s')", filename, realfilename );


  if ( is_flag_readable( flag ) )
  {
    if (allowed_to_read(realfilename))
    {
      granted = 1; // if read access is allowed, we grant the access
    }
  }

  if ( granted && is_flag_writable(flag))
  {
    if (!allowed_to_write(realfilename))
      granted = 0; // if write access is not allowed, we deny the access
  }

  // free realfilename after usage
  free(realfilename);

  return granted;
}


int fcheck_granted(const char *filename, const char *mode)
{
  int mode_read = 0;
  int mode_write = 0;
  int flag;

  mode_read = (strchr(mode, 'r') != NULL) || (strchr(mode, 'R') != NULL);
  mode_write = (strchr(mode, 'w') != NULL) || (strchr(mode, 'W') != NULL) ||
               (strchr(mode, 'a') != NULL) || (strchr(mode, 'A') != NULL);

  if (mode_read)
  {
    if (mode_write)
    {
      flag = O_RDWR; // read and write
    }
    else
    {
      flag = O_RDONLY; // read only
    }
  } else {
    if (mode_write)
    {
      flag = O_WRONLY; // write only
    }
    else
    {
      logfile_write( "fcheck_granted: no read or write access requested, access granted" );
      return 1; // no read or write access requested, so we grant the access
    } 
  }

  logfile_write( "fcheck_granted: checking access for '%s' with mode '%s' (flag=%i)", filename, mode, flag ); 

  return check_granted( filename, flag );
}


/* open/close/read/write overloading functions */

int open(const char *filename, int flag, ...)
{
  va_list ap;
  int res;
  void *arg;

  va_start(ap, flag);
  arg = va_arg(ap, void *);
  va_end(ap);

  /* do some sanity checks */
  if (orig_open == NULL)
  {
    printf( "Preinit function open called!\n" );
    orig_open = dlsym( RTLD_NEXT, "open" );
    return orig_open( filename, flag, arg );
  }

  if ( !check_granted( filename, flag ) )
  {
    logfile_write( "open: access to '%s' denied", filename );
    errno = EACCES; // set errno to access denied
    return -1; // access denied
  } else {
    logfile_write( "open: access to '%s' granted", filename );
  } 

  res = orig_open( filename, flag, arg );

  logfile_write( "open: filename='%s' mode=%i (%s) fd=%i", 
		 filename, flag, fstat_flag_str( flag ), res );

  file_stat_open( filename, res );
  return( res );
} 



int _close( int fd )
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


ssize_t _read(int fd, void *buf, size_t count)
{
  ssize_t res;

  res = orig_read( fd, buf, count );
  
  logfile_write( "read: fd=%i req=%i read=%i", fd, count, res ); 
  file_stat_read( fd, res );

  return res;
}


ssize_t _write(int fd, const void *buf, size_t count)
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


  if (!check_granted(filename, flag))
  {
    logfile_write("open: access to '%s' denied", filename);
    errno = EACCES; // set errno to access denied
    return -1;      // access denied
  }
  else
  {
    logfile_write("open: access to '%s' granted", filename);
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

    if (!fcheck_granted(filename, mode))
    {
      logfile_write("fopen: access to '%s' denied", filename);
      errno = EACCES; // set errno to access denied
      return NULL;      // access denied
    }
    else
    {
      logfile_write("fopen: access to '%s' granted", filename);
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


int _fclose( FILE *file )
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



size_t _fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t res;
  
  res = orig_fread( ptr, size, nmemb, stream );
  logfile_write( "fread: req=%i read=%i", (size*nmemb), res ); 
  file_stat_fread( stream, res );

  return res;
}


size_t _fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
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

DIR *_opendir(const char *name)
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