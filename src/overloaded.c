/* overloaded.c

   written by: Oliver Cordes 2012-08-02
   changed by: Oliver Cordes 2025-09-09
  
*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
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


/* the new open function */

int open(const char *filename, int flag, ...)
{
  va_list ap;
  int res;
  int arg;

  va_start(ap, flag);
  arg = va_arg(ap, int);
  va_end(ap);

  /* do some sanity checks */
  if (orig_open == NULL)
  {
    printf("Preinit function open called!\n");
    orig_open = dlsym(RTLD_NEXT, "open");
    return orig_open(filename, flag, arg);
  }

  if ( !check_granted(filename, flag))
  {
    if (shareiso_dry_run)
    {
      logfile_write("open: dry run, access to '%s' denied", filename);      
    }
    else
    {
      logfile_write("open: access to '%s' denied", filename);
    
      errno = EACCES; // set errno to access denied
      return -1; // access denied
    }
  } 
  else 
  {
    logfile_write("open: access to '%s' granted", filename);
  } 

  res = orig_open(filename, flag, arg);

  logfile_write("open: filename='%s' mode=%i (%s) fd=%i", 
		 filename, flag, fstat_flag_str(flag), res);

  return res;
} 


/* the new open64 function */

int open64(const char *filename, int flag, ...)
{
  va_list ap;
  int     res;
  int     arg;


  /* copy the extra argument */
  va_start(ap, flag);
  arg = va_arg(ap, int);
  va_end(ap);


  /* do some sanity checks */
  if (orig_open64 == NULL)
  {
    printf("Preinit function open64 called!\n");
    orig_open64 = dlsym(RTLD_NEXT, "open64");
    return orig_open64(filename, flag, (mode_t)arg);
  }


  logfile_write("open64: checking access for '%s' with flag=%i mode=%o", filename, 
     flag, arg);
     
  if (!check_granted(filename, flag))
  {
    if (shareiso_dry_run)
    {
      logfile_write("open64: dry run, access to '%s' denied", filename);
    }
    else
    {
      // if we are not in dry run mode, we deny the access
      logfile_write("open64: access to '%s' denied", filename);
      errno = EACCES; // set errno to access denied
      return -1;      // access denied
    }
  }
  else
  {
    logfile_write("open64: access to '%s' granted", filename);
  }

  res = orig_open64(filename, flag, (mode_t)arg);  

  logfile_write("open64: filename='%s' mode=%i (%s) fd=%i", 
		 filename, flag, fstat_flag_str(flag), res);


  return res;
}


/* the new fopen function */

FILE *fopen(const char *filename, const char *mode) 
{
  FILE *file;

  /* do some sanity checks */
  if (orig_fopen == NULL)
  {
    printf("Preinit function fopen called (filename=%s)!\n", filename);
    orig_fopen = dlsym(RTLD_NEXT, "fopen");
    return orig_fopen(filename, mode);
  }

  if (!fcheck_granted(filename, mode))
  {
    if (shareiso_dry_run)
    {
      logfile_write("fopen: dry run, access to '%s' denied", filename);
    }
    else
    {
      // if we are not in dry run mode, we deny the access
      logfile_write("fopen: access to '%s' denied", filename);
      errno = EACCES; // set errno to access denied
      return NULL;      // access denied
    }
  }
  else
  {
    logfile_write("fopen: access to '%s' granted", filename);
  }

  file = orig_fopen(filename, mode);

  return file;
}


/* fopen64 overloading functions */

FILE *fopen64(const char *filename, const char *mode)
{
  FILE *file;

  /* do some sanity checks */
  if (orig_fopen64 == NULL)
  {
    printf( "Preinit function fopen64 called (filename=%s)!\n", filename);
    orig_fopen64 = dlsym(RTLD_NEXT, "fopen64");
    return orig_fopen64(filename, mode);
  }

  file = orig_fopen64( filename, mode );
  if ( file != NULL )
    {
      logfile_write( "fopen64: filename='%s' mode='%s' result=OK", filename, mode );
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


/* overwrite stat */


int _stat(const char *pathname, struct stat *statbuf)
{
  int result;

  result = orig_stat(pathname, statbuf);

  logfile_write("stat: pathname='%s' result=%i", pathname, result);

  return result;
}

int _statx(int dirfd, const char *pathname, int flags,
          unsigned int mask, struct statx *statxbuf)
{
  int result;

  if (check_granted(pathname, O_RDONLY) == 0) {
    if (shareiso_dry_run) {
      logfile_write("statx: dry run, access to '%s' denied", pathname);
    } else {
      logfile_write("statx: access to '%s' denied", pathname);
      
      errno = EACCES; // set errno to access denied
      return -1;      // access denied
    }
  }

  result = orig_statx(dirfd, pathname, flags, mask, statxbuf);

  logfile_write("statx: dirfd=%i pathname='%s' flags=%i mask=%u result=%i",
                dirfd, pathname, flags, mask, result);

  return result;
}
