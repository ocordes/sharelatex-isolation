/* params.h

   written by: Oliver Cordes 2012-07-31
   changed by: Oliver Cordes 2025-07-02


*/


#ifndef __params_h 
#define __params_h 1

extern char *iostat_logging_name;
extern int   iostat_trunc_logfile;
extern int   iostat_statistics_append;

extern int   iostat_closing_statistics;

extern int   iostat_verbose;

void params_init( void );
void params_done( void );

#endif
