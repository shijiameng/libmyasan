#ifndef SHADOW_MEM_H
#define SHADOW_MEM_H

#include <stdint.h>

#if defined(__x86_64__)
#   define SHADOW_OFFSET 0x7fff8000ull
#   define HIGH_MEM_BEG 0x10007fff8000ull
#   define HIGH_MEM_END 0x7fffffffffffull
#   define LOW_MEM_BEG 0x000000000000ull
#   define LOW_MEM_END 0x00007fff7fffull
#elif defined(__i386__)
#   define SHADOW_OFFSET 0x20000000ul
#   define HIGH_MEM_BEG 0x40000000ul
#   define HIGH_MEM_END 0xfffffffful
#   define LOW_MEM_BEG 0x00000000ul
#   define LOW_MEM_END 0x1ffffffful
#elif defined(__aarch64__)
#   define SHADOW_OFFSET 0x1000000000ull
#   define HIGH_MEM_BEG 0x2000000000ull
#   define HIGH_MEM_END 0x7fffffffffull
#   define LOW_MEM_BEG 0x0000000000ull
#   define LOW_MEM_END 0x0fffffffffull
#else
#   error("A system architecture must be specified!")
#endif

#define MEM2SHADOW(addr)            (((addr) >> 3) + SHADOW_OFFSET)
#define SHADOW_MEM_SZ(beg, end)     ((end) - (beg) + 1)

#define SHADOW_MEM_HIGH_BEG     MEM2SHADOW(HIGH_MEM_BEG)
#define SHADOW_MEM_HIGH_END     MEM2SHADOW(HIGH_MEM_END)
#define SHADOW_MEM_HIGH_SZ		SHADOW_MEM_SZ(SHADOW_MEM_HIGH_BEG, SHADOW_MEM_HIGH_END)

#define SHADOW_MEM_LOW_BEG 		MEM2SHADOW(LOW_MEM_BEG)
#define SHADOW_MEM_LOW_END	    MEM2SHADOW(LOW_MEM_END)
#define SHADOW_MEM_LOW_SZ		SHADOW_MEM_SZ(SHADOW_MEM_LOW_BEG, SHADOW_MEM_LOW_END)

#define SHADOW_GAP_BEG          (SHADOW_MEM_LOW_END + 1)
#define SHADOW_GAP_END          (SHADOW_MEM_HIGH_BEG - 1)
#define SHADOW_GAP_SZ			SHADOW_MEM_SZ(SHADOW_GAP_BEG, SHADOW_GAP_END)


#endif /* SHADOW_MEM_H */
