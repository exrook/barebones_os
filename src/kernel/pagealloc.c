#include "pagealloc.h"
// Allocate new memory for page allocator when there less than 2 spaces for
// pages remaining
typedef struct {
	uintptr_t addr;
	// Page Physical Address
} page;
// Table of pointers to buddy tables, one for each level, each table holds
// over
typedef struct {
       union {
		uint16_t count;
 		struct {
			uint16_t;
			uint16_t flags;
		};
       };
} page_table;
// page used_pages[1024][];
// page free_pages[];
void* alloc_pagev1() {
	return 0;
}
void init_allocatorv1() {}

void* alloc_page() {
	return alloc_pagev1();
}

void** alloc_pages() {
	return 0;
}
