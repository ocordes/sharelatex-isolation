#define _GNU_SOURCE

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    printf("%s\n", realpath("test2.c", NULL));
    printf("%s\n", canonicalize_file_name("test22.c"));

    return 0;
}   