#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "vga.h"

uint32_t* multiboot_info_p;

uint64_t** const pml4t = (void*) 0x1000;
uint64_t** const pdpt = (void*) 0x2000;
uint64_t** const pdt = (void*) 0x3000;
//uint64_t** const pt = (void*) 0x4000;
uint64_t** const kpdpt = (void*) 0x5000;
uint64_t** const kpdt = (void*) 0x6000;
uint64_t** const kpt = (void*) 0x7000;
// Sign Extension
// 63 62 61 60-59 58 57 56-55 54 53 52-51 50 49 48
// |              Sign Extension                 |
// 47 46 45 44-43 42 41 40-39 38 37 36-35 34 33 32
// |          PML4T          |        PDPT
// 31 30 29 28-27 26 25 24-23 22 21 20-19 18 17 16
//      |            PDT           |      PT
// 15 14 13 12-11 10  9  8- 7  6  5  4- 3  2  1  0
//            |             Offset               |
// We want to map 0xffffffff80000000-0xffffffffffffffff to
// 0x0000000000000000-0x0000000080000000
//  f f f f f f f f 8 0 0 0 0 0 0 0
// |  SE   |PML4|

void memclear(uint64_t** mem, const size_t len);
void pt_fill(uint64_t** ptp, uint64_t base) {
	for (uint64_t i = 0; i < 512; i++) {
		ptp[i] = (uint64_t*)(0x1000*i + base + 0x3);
	}
}

void page_init() {
	uint32_t flags = multiboot_info_p[0];
	uint32_t lower_mem_size = multiboot_info_p[1];
	uint32_t upper_mem_size = multiboot_info_p[2];
	uint32_t mmap_length = multiboot_info_p[11];
	uint32_t mmap_addr = multiboot_info_p[12];
	if (!(flags|(1<<6))) { // Check for BIOS Memory Map, if not return
		return;
	}
	memclear(kpdpt, 512);
	memclear(kpdt , 512);
	pt_fill(kpt, 0x0);
	pml4t[511] = (uint64_t*) ((uint64_t)kpdpt + 0x3); // OR with flags for 0x10 (R/w) and 0x01 (present)
	kpdpt[510]   = (uint64_t*) ((uint64_t)kpdt + 0x3);
	kpdt[0]    = (uint64_t*) ((uint64_t)kpt + 0x3);
	kernel_main();
	return;
}
void memclear(uint64_t** mem, const size_t len) {
	for (size_t i = 0; i < len; i++) {
		mem[i] = 0x0;
	}
}
