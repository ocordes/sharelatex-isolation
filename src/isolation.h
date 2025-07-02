/* isolation.h
 * 
 * This file is part of the ShareLaTeX Isolation project.
 * 
 * It implements the isolation functionality for the project.
 * 
 * written by: Oliver Cordes 2025-06-28 
 * changed by: Oliver Cordes 2025-07-02
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
*/

#ifndef __isolation_h
#define __isolation_h 1

#include "whitelist.h"

// environment variables
#define ENV_READ_WHITELIST "SHAREISO_READ_WHITELIST"
#define ENV_WRITE_WHITELIST "SHAREISO_WRITE_WHITELIST"

extern whitelist *read_whitelist;
extern whitelist *write_whitelist;

void isolation_init(void);
void isolation_done(void);

#endif // isolation_h