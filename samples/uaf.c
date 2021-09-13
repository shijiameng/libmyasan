#include <stdio.h>
#include <stdlib.h>
#include "asan.h"

int main(void)
{
    char *buf;

    puts("Use-After-Free example");
    
    buf = asan_malloc(8);
    asan_free(buf);

    asan_load1_check(&buf[0]);
    printf("buf[0]=%d\n", buf[0]);

    return 0;
}