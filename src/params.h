/* params.h

   written by: Oliver Cordes 2012-07-31
   changed by: Oliver Cordes 2025-07-04


*/


#ifndef __params_h 
#define __params_h 1

extern char *shareiso_logging_name;
extern int   shareiso_trunc_logfile;
extern int   shareiso_dry_run;

void params_init( void );
void params_done( void );

#endif
