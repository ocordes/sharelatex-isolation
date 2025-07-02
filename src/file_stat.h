/* file_stat.h

   written by: Oliver Cordes 2012-07-27
   changed by: Oliver Cordes 2025-07-02


*/


#ifndef __file_stat_h
#define __file_stat_h 1

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>



/* overloaded function variables */

extern int     (*orig_open)     (const char *filename, int flag, ...);
extern FILE*   (*orig_fopen)    (const char *filename, const char *mode);
extern int     (*orig_open64)   (const char *filename, int flag, mode_t mode);
extern FILE*   (*orig_fopen64)  (const char *filename, const char *mode);

extern DIR*    (*orig_opendir)  (const char *name);




/* module functions */


char *fstat_flag_str( int flag );

#endif
