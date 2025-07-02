/* whitelist.c
 * 
 * This file is part of the ShareLaTeX Isolation project.
 * It implements the whitelist functionality for the project.
 *
 * written by: Oliver Cordes 2025-06-27  
 * changed by: Oliver Cordes 2025-07-02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 

*/

#define _GNU_SOURCE
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "whitelist.h"


whitelist* whitelist_create(int initial_capacity)
{
    // This function creates a new whitelist with the specified initial capacity.
    whitelist *wl = malloc(sizeof(whitelist));
    if (!wl) {
        return NULL; // Memory allocation failed
    }
    
    wl->count = 0;
    wl->capacity = initial_capacity;
    wl->items = malloc(initial_capacity * sizeof(char *));
    
    if (!wl->items) {
        free(wl);
        return NULL; // Memory allocation failed
    }
    
    return wl;
}

void whitelist_destroy(whitelist *wl)
{
    // This function frees the memory allocated for the whitelist.
    if (wl) {
        for (int i = 0; i < wl->count; i++) {
            free(wl->items[i]); // Free each item
        }
        free(wl->items); // Free the items array
        free(wl); // Free the whitelist structure
    }
}


int whitelist_add(whitelist *wl, const char *name)
{
    // This function adds a name to the whitelist.
    if (wl->count >= wl->capacity) {
        // Resize the items array if necessary
        int new_capacity = wl->capacity * 2;
        char **new_items = realloc(wl->items, new_capacity * sizeof(char *));
        if (!new_items) {
            return -1; // Memory allocation failed
        }
        wl->items = new_items;
        wl->capacity = new_capacity;
    }

    // Add the new name to the whitelist
    wl->items[wl->count] = strdup(name);
    if (!wl->items[wl->count]) {
        return -1; // Memory allocation failed
    }
    
    wl->count++;
    return 0; // Success
}

int whitelist_remove(whitelist *wl, const char *name)
{
    // This function removes a name from the whitelist.
    for (int i = 0; i < wl->count; i++) {
        if (strcmp(wl->items[i], name) == 0) {
            free(wl->items[i]); // Free the memory for the item
            wl->items[i] = wl->items[wl->count - 1]; // Move the last item to the current position
            wl->count--; // Decrease the count
            return 0; // Success
        }
    }
    return -1; // Name not found in the whitelist
}

void whitelist_show(whitelist *wl) {
    // This function prints the items in the whitelist.
    for (int i = 0; i < wl->count; i++) {
        printf("%i: %s\n", (i+1), wl->items[i]);
    }
}


int is_directory(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        return 0; // Path does not exist or error accessing it
    }
    return S_ISDIR(path_stat.st_mode);
}


// whitelist_from_string creates a whitelist from a string.
// The string is expected to be a comma-/or semi-colon-separated list of names.
// If the name does not end with '/', it appends a '/' if the name is a directory.
// Otherwise, it uses the name as is as a file name.
whitelist *whitelist_from_string(const char *str) {
    
    char *sdir = NULL; // Variable to hold the real path of the directory

    whitelist *wl = whitelist_create(10); // Create a new whitelist with initial capacity
    if (!wl) {
        return NULL; // Memory allocation failed
    }
    
    char *token, *ntoken;
    char *str_copy = strdup(str); // Duplicate the string to avoid modifying the original
    if (!str_copy) {
        whitelist_destroy(wl);
        return NULL; // Memory allocation failed
    }
    
    token = strtok(str_copy, ":,"); // Tokenize the string by semicolon and comma
    while (token != NULL) {
        if (token[strlen(token) - 1] == '/') {
            asprintf(&ntoken, "%s", token); // If the token ends with '/', use it as is
        } else {
            // If the token does not end with '/', append a '/' to it
            if (is_directory(token) == 0) {
                // If the token is not a directory
                asprintf(&ntoken, "%s", token);
            } else {
                // If the token is a directory, append a '/'
                sdir = realpath(token, NULL);
                asprintf(&ntoken, "%s/", sdir);
                free(sdir); // Free the realpath result
            } 
        }
    
        if (whitelist_add(wl, ntoken) != 0) {
            free(str_copy);
            free(ntoken); // Free the token string
            whitelist_destroy(wl);
            return NULL; // Memory allocation failed
        }
        token = strtok(NULL, ":, ,");
    }
    
    free(str_copy); // Free the duplicated string
    return wl; // Return the created whitelist
}


int whitelist_check(whitelist *wl, const char *name) {
    // This function checks if the given name is in the whitelist.
    for (int i = 0; i < wl->count; i++) {
        char *dupname = strdup(name); // Duplicate the name to avoid modifying the original

        if (strlen(dupname) < strlen(wl->items[i])) {
            // If the name is shorter than the whitelist item, it cannot match
            free(dupname);
            continue;
        }
        // check if item is a directory
        if (wl->items[i][strlen(wl->items[i]) - 1] == '/') {
            dupname[strlen(wl->items[i])] = '\0'; // Cut the string to the length of the whitelist item
        }
        
        // Compare the name with the whitelist item
        if (strcmp(wl->items[i], dupname) == 0)
            return 1; // Name found in the whitelist
        

        // Free the duplicated name after use
        free(dupname); // Free the duplicated name
    }
    return 0; // Placeholder implementation, always returns 0 (not whitelisted).
}