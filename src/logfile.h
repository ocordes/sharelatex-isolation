/* logfile.h

   written: Oliver Cordes 2012-08-01
   updated: Oliver Cordes 2012-08-02

*/


#ifndef __logfile_h
#define __logfile_h 1


void logfile_init ( void );
void logfile_done ( void );

void logfile_write( char *format, ... );


#endif
