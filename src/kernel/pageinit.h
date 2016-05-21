#ifndef __K_PAGEINIT_H__
#define __K_PAGEINIT_H__

#include <stdint.h>
uint64_t** const pml4t = (void*) 0x1000;
uint64_t** const pdpt = (void*) 0x2000;
uint64_t** const pdt = (void*) 0x3000;
uint64_t** const pt = (void*) 0x4000;
uint64_t** const kpdpt = (void*) 0x5000;
uint64_t** const kpdt = (void*) 0x6000;
uint64_t** const kpt = (void*) 0x7000;

#endif
