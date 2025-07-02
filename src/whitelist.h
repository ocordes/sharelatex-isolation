/* whitelist.h

written by: Oliver Cordes 2025-06-27  
changed by: Oliver Cordes 2025-06-27

*/

#ifndef WHITELIST_H
#define WHITELIST_H 1   

typedef struct{
    int count; // Number of whitelisted items
    char **items; // Array of whitelisted items
    int capacity; // Capacity of the items array
} whitelist;

extern whitelist* whitelist_create(int initial_capacity);
extern void whitelist_destroy(whitelist *wl);
extern int whitelist_add(whitelist *wl, const char *name);
extern int whitelist_remove(whitelist *wl, const char *name);
extern void whitelist_show(whitelist *wl);

extern whitelist* whitelist_from_string(const char *str);

extern int whitelist_check(whitelist *wl, const char *name);



#endif /* WHITELIST_H */