#include <stdio.h>
#include <stdlib.h>
#include "asan.h"

int main(void)
{
    char *buf;

    puts("Heap overflow example");
    
    buf = asan_malloc(8);
    asan_store1_check(&buf[8]);
    buf[8] = 0;

    return 0;
}
    

    