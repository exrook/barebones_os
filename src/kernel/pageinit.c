#include <stdint.h>
uint64_t** const pml4t = (void*) 0x1000;
uint64_t** const pdpt = (void*) 0x2000;
uint64_t** const pdt = (void*) 0x3000;
uint64_t** const pt = (void*) 0x4000;
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

void pt_fill(uint64_t** ptp, uint64_t* base) {
	for (int i = 0; i < 512; i++) {
		pt[i] = (0x1000*i + base);
	}
}

void page_init() {
	pml4t[511] = (void*) pdpt;
	pt[0] = 0x0000;
	pt_fill(pt, (void*) 0x0);
	return;
}
