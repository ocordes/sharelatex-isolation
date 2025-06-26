/* file_stat.h

   written by: Oliver Cordes 2012-07-27
   changed by: Oliver Cordes 2025-06-26


*/


#ifndef __file_stat_h
#define __file_stat_h 1

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


typedef struct{
  char    *filename;
  unsigned long long int reads;
  unsigned long long int writes;
  int      open;
  int      open64;
} _fstat_file_info;




typedef struct{
  int nonfree;
  int fileid;
  int fd;
} _fstat_file_open;


typedef struct{
  int   nonfree;
  int   fileid;
  FILE *file;
} _fstat_file_fopen;
	       

/* overloaded function variables */

extern int     (*orig_open)     (const char *filename, int flag, ...);
extern int     (*orig_close)    (int fd);
extern ssize_t (*orig_read)     (int fd, void *buf, size_t count);
extern ssize_t (*orig_write)    (int fd, const void *buf, size_t count);

extern FILE*   (*orig_fopen)    (const char *filename, const char *mode);
extern int     (*orig_fclose)   ( FILE *file );
extern size_t  (*orig_fread)    (void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t  (*orig_fwrite)   (const void *ptr, size_t size, size_t nmemb, FILE *stream);

extern int     (*orig_open64)   (const char *filename, int flag, ...);
extern FILE*   (*orig_fopen64)  (const char *filename, const char *mode);

extern DIR*    (*orig_opendir)  (const char *name);




/* module functions */

void file_stat_init(void);
void file_stat_done( void );

/* overloaded functions for the classical open functions */
void file_stat_open( const char *filename, int fd );
void file_stat_read( int fd, unsigned long long int reads );
void file_stat_write( int fd, unsigned long long int writes );
void file_stat_close( int fd );

void file_stat_open64( const char *filename, int fd );


void file_stat_fopen( const char *filename, FILE *file);
void file_stat_fclose( FILE *file);
void file_stat_fread( FILE *file, unsigned long long int reads );
void file_stat_fwrite( FILE *file, unsigned long long int writes );


void file_stat_fopen64( const char *filename, FILE *file);



char *fstat_filename_file( FILE *file );

char *fstat_flag_str( int flag );

#endif
