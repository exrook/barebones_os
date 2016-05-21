#ifndef __K_PAGEALLOC_H__
#define __K_PAGEALLOC_H__
#include <stdint.h>
extern void* _end; 
void* alloc_page();
void** alloc_pages(int count);
void init_alloc();
#endif
