/* shareiso.c

   written by: Oliver Cordes 2025-06-28
   changed by: Oliver Cordes 2025-09-09

*/

#define _GNU_SOURCE
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/limits.h>

#include "config.h"

#include "file_stat.h"
#include "isolation.h"
#include "logfile.h"
#include "params.h"


// get the original cmdline of the attached process
char *get_cmdline()
{
  ssize_t i, n;
  char cmdline[ARG_MAX];
  int cmdline_fd = open("/proc/self/cmdline", O_RDONLY);
  n = read(cmdline_fd, cmdline, sizeof cmdline);
  for (i = 0; i < n; ++i)
    if (!cmdline[i])
      cmdline[i] = ' ';
  close(cmdline_fd);

  return strdup(cmdline);
}


void copy_orig_function( void **func, char *funcname )
{
  char *err;

  (*func) = dlsym(RTLD_NEXT, funcname);
  err = dlerror();
  if (err != NULL)
    {
      printf("Can't locate function '%s' (msg='%s')! Override ignored!\n", funcname, err);
    }
}


void __attach(void) __attribute__((constructor));
void __detach(void) __attribute__((destructor));

void __attach(void)
{
  char *s;

  /* copy the original functions */
  copy_orig_function((void*) &orig_open, "open");
  copy_orig_function((void*) &orig_open64, "open64");

  copy_orig_function((void*) &orig_fopen, "fopen");
  copy_orig_function((void*) &orig_fopen64, "fopen64");
  
  copy_orig_function( (void*) &orig_opendir, "opendir");

  copy_orig_function( (void*) &orig_stat, "stat");
  copy_orig_function( (void*) &orig_statx, "statx");

  // read the original command line of the attached process
  s = get_cmdline();

  fprintf(stderr, "LD_PRELOAD: sharelatex-isolation library (Version %s) for '%s' loaded\n", VERSION, s);
  free(s);

  // initialize the isolation library
  // this must be done before the original functions are copied
  isolation_init();

  params_init();  
  logfile_init(); 
}

void __detach(void)
{
  logfile_done(); 
  params_done(); 
  isolation_done();
}  
