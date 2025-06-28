/* isolation.c
 * 
 * This file is part of the ShareLaTeX Isolation project.
 * 
 * It implements the isolation functionality for the project.
 * 
 * written by: Oliver Cordes 2025-06-28 
 * changed by: Oliver Cordes 2025-06-28
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isolation.h"
#include "whitelist.h"


whitelist *read_whitelist = NULL;
whitelist *write_whitelist = NULL;


void isolation_init(void) {
    char *e;
    
    // Initialize the isolation library
    e = getenv(ENV_READ_WHITELIST);
    if (e) {
        read_whitelist = whitelist_from_string(e);
    }
    else {
        printf("Environment variable '%s' not set. Using default read whitelist: '/etc:/usr:..'\n", ENV_READ_WHITELIST);
        read_whitelist = whitelist_from_string("/etc:/usr:.");
    }
    if (!read_whitelist) {
        printf("Misconfiguration or missing environment variable '%s'!\n", ENV_READ_WHITELIST);
    }    
    
    e = getenv(ENV_WRITE_WHITELIST);
    if (e) {
        write_whitelist = whitelist_from_string(e);
    }
    else {
        printf("Environment variable '%s' not set. Using default write whitelist: '.'\n", ENV_WRITE_WHITELIST);
        // Default write whitelist is the current directory, which is usually safe for writing.
        write_whitelist = whitelist_from_string(".");
    }
    if (!write_whitelist) {
        printf("Misconfiguration or missing environment variable '%s'!\n", ENV_WRITE_WHITELIST);
    }

    printf("Isolation library initialized.\n");
}

void isolation_done(void) {
    // Cleanup the isolation library
    printf("Isolation library cleaned up.\n");
}

