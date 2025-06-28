/* iostat.c

   written by: Oliver Cordes 2012-07-27
   changed by: Oliver Cordes 2025-06-28

*/

#define _GNU_SOURCE
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>

#include "config.h"

#include "file_stat.h"
#include "isolation.h"
#include "logfile.h"
#include "params.h"

int get_cmdline()
{
  ssize_t i, n;
  char cmdline[ARG_MAX];
  int cmdline_fd = open("/proc/self/cmdline", O_RDONLY);
  n = read(cmdline_fd, cmdline, sizeof cmdline);
  for (i = 0; i < n; ++i)
    if (!cmdline[i])
      cmdline[i] = ' ';
  cmdline[n - 1] = '\n';
  write(STDOUT_FILENO, cmdline, n);
  return 0;
}

void copy_orig_function( void **func, char *funcname )
{
  char *err;

  (*func) = dlsym( RTLD_NEXT, funcname );
  err = dlerror();
  if ( err != NULL )
    {
      printf( "Can't locate function '%s' (msg='%s')! Override ignored!\n", funcname, err );
    }
}


void __attach(void) __attribute__((constructor));
void __detach(void) __attribute__((destructor));

void __attach(void)
{
  printf("LD_PRELOAD: sharelatex-isolation library (Version %s) loaded\n", VERSION);

  // initialize the isolation library
  // this must be done before the original functions are copied
  isolation_init();

  /* copy the original functions */
  copy_orig_function( (void*) &orig_open, "open" );
  copy_orig_function( (void*) &orig_close, "close" );
  copy_orig_function( (void*) &orig_read, "read" );
  copy_orig_function( (void*) &orig_write, "write" );

  copy_orig_function( (void*) &orig_open64, "open64" );

  copy_orig_function( (void*) &orig_fopen, "fopen" );
  copy_orig_function( (void*) &orig_fclose, "fclose" );
  copy_orig_function( (void*) &orig_fread, "fread" );
  copy_orig_function( (void*) &orig_fwrite, "fwrite" );

  copy_orig_function( (void*) &orig_fopen64, "fopen64" );


  copy_orig_function( (void*) &orig_opendir, "opendir" );
  

  params_init();  
  logfile_init(); 
  file_stat_init(); 
}

void __detach(void)
{
  file_stat_done();
  logfile_done(); 
  params_done(); 
  isolation_done();
}  
