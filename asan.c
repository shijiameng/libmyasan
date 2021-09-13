#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "shadow_mem.h"
#include "asan.h"

#define PADDING(s, a) (((a) - ((s) & ((a) - 1))) & ((a) - 1))
#define REDZONE_SZ      32
#define QUARANTINE_LIST_SIZE	8

static uint8_t *shadow_mem_high = NULL;
static uint8_t *shadow_mem_gap = NULL;
static uint8_t *shadow_mem_low = NULL;

static void *quarantine_list[QUARANTINE_LIST_SIZE] = { NULL };
static int q_idx = 0;

static uint8_t *mem2shadow(void *addr)
{
	return (uint8_t *) MEM2SHADOW((uintptr_t) addr);
}

static void poison_shadow_byte_one_addr(void *addr)
{
	*mem2shadow(addr) |= 1 << ((uintptr_t)addr & 7);
}

static void clear_shadow_byte_one_addr(void *addr)
{
	*mem2shadow(addr) &= ~(1 << ((uintptr_t)addr & 7));
}


static void poison(void *addr, size_t length)
{
	uint8_t *p = addr;
	int i;

	for (i = 0; i < length; i++, p++) {		
		poison_shadow_byte_one_addr(p);
	}
}

static void unpoison(void *addr, size_t length)
{
	uint8_t *p = addr;
	int i;

	for (i = 0; i < length; i++, p++) {
		clear_shadow_byte_one_addr(p);
	}
}

static void report_error(void *addr, size_t access_size, int is_write)
{
	fprintf(stderr, "ASan check failure at %p!\n", addr);
	fprintf(stderr, "Access size: %lu, mode: %c\n", 
					access_size, is_write ? 'W' : 'R');
	abort();
}

static void check_shadow(void *addr, size_t length, int mode)
{
	int8_t shadow_value = *(int8_t *)mem2shadow(addr);
	if (shadow_value == -1) {
		report_error(addr, length, mode);
	} else if (shadow_value != 0) {
		int8_t last_accessed_byte = (((uintptr_t)addr) & 7) + length - 1;
		if (last_accessed_byte >= shadow_value) {
			report_error(addr, length, mode);
		} 
	}
}

void asan_load8_check(void *addr) {
	check_shadow(addr, 8, 0);
}

void asan_load4_check(void *addr) {
	check_shadow(addr, 4, 0);
}

void asan_load2_check(void *addr) {
	check_shadow(addr, 2, 0);
}

void asan_load1_check(void *addr) {
	check_shadow(addr, 1, 0);
}

void asan_store8_check(void *addr) {
	check_shadow(addr, 8, 1);
}

void asan_store4_check(void *addr) {
	check_shadow(addr, 4, 1);
}

void asan_store2_check(void *addr) {
	check_shadow(addr, 2, 1);
}

void asan_store1_check(void *addr) {
	check_shadow(addr, 1, 1);
}

static void asan_init(void)
{
	unsigned flags = MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE;
	
	shadow_mem_high = mmap((void *) SHADOW_MEM_HIGH_BEG, SHADOW_MEM_HIGH_SZ,
			PROT_READ | PROT_WRITE, flags, -1, 0);

	assert(shadow_mem_high != MAP_FAILED);

	shadow_mem_gap = mmap((void *) SHADOW_GAP_BEG, SHADOW_GAP_SZ, 
			PROT_NONE, flags, -1, 0);

	assert(shadow_mem_gap != MAP_FAILED);

	shadow_mem_low = mmap((void *) SHADOW_MEM_LOW_BEG, SHADOW_MEM_LOW_SZ, 
			PROT_READ | PROT_WRITE, flags, -1, 0);

	assert(shadow_mem_low != MAP_FAILED);

	/* poison quarantine list */
	poison(quarantine_list, QUARANTINE_LIST_SIZE * sizeof(void *));
	poison(&q_idx, sizeof(int));
}


static void asan_deinit(void)
{
	munmap(shadow_mem_high, SHADOW_MEM_HIGH_SZ);
	munmap(shadow_mem_gap, SHADOW_GAP_SZ);
	munmap(shadow_mem_low, SHADOW_MEM_LOW_SZ);

	shadow_mem_high = shadow_mem_gap = shadow_mem_low = NULL;
}

void *asan_malloc(size_t size)
{
	void *p = NULL;
	uint8_t *front_redzone, *rare_redzone, *valid_space;
	size_t actual_size = size + PADDING(size, 8);

	p = malloc(actual_size + 2 * REDZONE_SZ);
	if (!p) {
		return NULL;
	}

	front_redzone = p;
	valid_space = front_redzone + REDZONE_SZ;
	rare_redzone = valid_space + actual_size;

	/* save size */
	*(size_t *)(valid_space - sizeof(size_t)) = actual_size;

	/* poison front red zone */
	poison(front_redzone, REDZONE_SZ);

	/* unpoison valid memory */
	unpoison(valid_space, actual_size);

	/* poison rare red zone */
	poison(rare_redzone, REDZONE_SZ);

    return (void *)valid_space;
}

void asan_free(void *addr)
{
	size_t actual_size = *(size_t *)((uintptr_t)addr - sizeof(size_t));
	void *actual_base = (void *)((uintptr_t)addr - REDZONE_SZ);

	/* poison free'ed space */
	poison(addr, actual_size);

	quarantine_list[q_idx++] = actual_base;
	if (q_idx >= QUARANTINE_LIST_SIZE) {
		q_idx = 0;
	}

	if (quarantine_list[q_idx] != NULL) {
		free(quarantine_list[q_idx]);
		quarantine_list[q_idx] = NULL;
	}
}


__attribute__((constructor)) static void asan_onload(void)
{
	asan_init();
}

__attribute__((destructor)) static void asan_onexit(void)
{
	asan_deinit();
}
