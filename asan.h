#ifndef ASAN_H
#define ASAN_H

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

void asan_load8_check(void *);

void asan_load4_check(void *);

void asan_load2_check(void *);

void asan_load1_check(void *);

void asan_store8_check(void *);

void asan_store4_check(void *);

void asan_store2_check(void *);

void asan_store1_check(void *);

void *asan_malloc(size_t);

void asan_free(void *);

#ifdef __cplusplus
}
#endif

#endif /* ASAN_H */
