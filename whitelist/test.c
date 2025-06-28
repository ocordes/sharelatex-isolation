#include <stdlib.h>
#include <stdio.h>  

#include <string.h>

#include "whitelist.h"


void test_whitelist(whitelist *wl, const char *name) {
    int res;

    res = whitelist_check(wl, name);
    if (res == 1) {
        printf("Item '%s' is in the whitelist.\n", name);
    }
    else {
        printf("Item '%s' is NOT in the whitelist.\n", name);
    }
}


int main() {
    whitelist *wl;
    whitelist *bl;

    wl = whitelist_from_string("/etc:/usr:.:/bin/ls");
    if (!wl) {
        fprintf(stderr, "Failed to create whitelist\n");
        return 1;    }
    
    bl = whitelist_from_string("name5:name6,name7,name8");
    if (!bl) {
        fprintf(stderr, "Failed to create blacklist\n");
        whitelist_destroy(wl);
        return 1;
    }

    whitelist_show(wl);
    whitelist_show(bl); 

    test_whitelist(wl, "/etc/hallo");
    test_whitelist(wl, "/etC/hallo");
    test_whitelist(wl, "/bin/ls");
    test_whitelist(wl, "/bin/ls2");
    test_whitelist(wl, "/usr/bin/ls");

    whitelist_destroy(wl);
    whitelist_destroy(bl);


    // Check if specific items are in the whitelist

    return 0;
}