#include <stdint.h>
#include <stdbool.h>

#include "memory.h"

void* _first_free_real_page;
static void* next_free_page;

typedef uint64_t pml4_entry; // Page Map Level 4
typedef pml4_entry pml4t[512];

typedef uint64_t pdp_entry; // Page Directory Pointer
typedef pdp_entry pdpt[512];

typedef uint64_t pd_entry; // Page Directory
typedef pd_entry pdt[512];

typedef uint64_t pt_entry; // Page Table
typedef pt_entry pt[512];

void setup_page_tables();
static void* get_next_page();
void set_pml4t_entry(pml4t* root_pagemap, uint64_t virt_address, pdpt* entry, uint16_t flags, bool nx);
void set_pdpt_entry(pdpt* pagemap, uint64_t virt_address, pdt* pdt_addr, uint16_t flags, bool nx);
void set_pdt_entry(pdt* pagemap, uint64_t virt_address, pt* pt_addr, uint16_t flags, bool nx);
void set_pt_entry(pdt* pagemap, uint64_t virt_address, void* phys_address, uint16_t flags, bool nx);

#define PG_FLAG_PRESENT 1<<0 // Whether this entry is enabled (1) or not (0)
#define PG_FLAG_WRITEABLE 1<<1 // Whether this page can be written to (1) or not (0)
#define PG_FLAG_USER 1<<2 // Whether access to this page or page table is restricted to supervisor code (0) or not (1)
#define PG_FLAG_PAGE_WRITE_THROUGH 1<<3 // Whether this page or page table is write through (1) or writeback (0)
#define PG_FLAG_PAGE_LEVEL_CACHE_DISABLE 1<<4 //Whether this page or page table is cacheable (0) or not (1)
#define PG_FLAG_ACCESSED 1<<5 // Whether the page or page table has been accessed (1) or not (0), set by processor
#define PG_FLAG_DIRTY 1<<6 // Whether this page has been written (1) or not (0), only on lowest level entries
#define PG_FLAG_PAGE_SIZE 1<<7 // Whether this entry points to a large page (1) or not (0), only valid on PD and PDP entries
#define PG_FLAG_PAT_PTE 1<<7   // Page Attribute Table bit, for PTE, can allow for more control over caching
#define PG_FLAG_PAT_PDE 1<<12  // Page Attribute Table bit, for PDE, can allow for more control over caching
#define PG_FLAG_PAT_PDPE 1<<12 // Page Attribute Table bit, for PDPE, can allow for more control over caching
#define PG_FLAG_GLOBAL 1<<8 // Whether the TLB entry for this page should be invalidated when the root page table is changed, only set for lowest level entries
#define PG_FLAG_AVL1 1<<9  // Avaliable for use by system software
#define PG_FLAG_AVL2 1<<10 // Avaliable for use by system software
#define PG_FLAG_AVL3 1<<11 // Avaliable for use by system software

static uint8_t free_areas_len;
static meminfo_entry free_areas[128];

static uint8_t reserved_areas_len;
static meminfo_entry reserved_areas[128];

pml4t* kernel_pml4t;
pdpt* kernel_pdpt;
pdt* kernel_pdt;

void setup_memory(uint32_t* multiboot_info_p ) {
	uint32_t flags = multiboot_info_p[0];
	uint32_t lower_mem_size = multiboot_info_p[1];
	uint32_t upper_mem_size = multiboot_info_p[2];
	uint32_t boot_device = multiboot_info_p[3];
	uint32_t cmdline = multiboot_info_p[4];
	uint32_t mmap_length = multiboot_info_p[11];
	uint32_t mmap_addr = multiboot_info_p[12];
	uint32_t bootloader_name = multiboot_info_p[16];
	if (flags|(1<<6)) { // Check for bios memory map, location is in mmap_addr, length is in mmap_length
		free_areas_len = 0;
		reserved_areas_len = 0;
		uint32_t i = 0;
		uint8_t* mmap = &(((uint8_t*)(uint64_t) mmap_addr)[0]);
		while (i < mmap_length) {
			uint32_t size = mmap[i];
			meminfo_entry* entry = (meminfo_entry*) &mmap[i+4];
			if (entry->type != 1) {// If 1 indicates ram is avaliable, otherwise reserved
				free_areas[free_areas_len++] = *entry;
			} else {
				reserved_areas[reserved_areas_len++] = *entry;
			}
			i = i + size + 4;
		}
	}
	setup_page_tables();
}

void setup_page_tables() {
	next_free_page = _first_free_real_page;
	kernel_pml4t = get_next_page();
	kernel_pdpt = get_next_page();
	kernel_pdt = get_next_page();
	set_pml4t_entry(kernel_pml4t, 0xffffffff80000000, kernel_pdpt, PG_FLAG_PRESENT|PG_FLAG_WRITEABLE, false);
	set_pdpt_entry(kernel_pdpt, 0xffffffff80000000, kernel_pdt, PG_FLAG_PRESENT|PG_FLAG_WRITEABLE, false);
	void* last_kernel_addr;
	void* last_2M_page = (void*)(((uint64_t) _first_free_real_page - 4096) & 0xffffffffffe00000);
	void* last_4K_page = (_first_free_real_page - 4096);

	for (uint64_t i = 0; i < (uint64_t)last_2M_page/0x200000; i++) {
		set_pdt_entry(kernel_pdt, 0xffffffff80000000, (void*) (0x200000 * i), PG_FLAG_PRESENT|PG_FLAG_WRITEABLE|PG_FLAG_PAGE_SIZE, false);
	}
}

void static inline _clear_page(void* page) {
	uint64_t* pg = page;
	for (int i = 0; i < (4096/sizeof(uint64_t)); i++ ) {
		pg[i] = 0;
	}
}

static void* get_next_page() {
	void* page = next_free_page;
	next_free_page += 4096;
	_clear_page(page);
	return page;
}

void set_pml4t_entry(pml4t* root_pagemap, uint64_t virt_address, pdpt* pdpt_addr, uint16_t flags, bool nx) {
	virt_address &= (0xff8000000000); //Mask all but 9 bits that make up PML4T
	virt_address >>= 39; // Shift out all the zeros
	uint64_t entry = (uint64_t)pdpt_addr & (0xffffffffff000); // Mask out space for flags, since address must be 4K aligned (2^12)
	flags &= 0xfff;
	entry |= flags;
	entry |= ((uint64_t)nx) << 63; // Add in nx flag in rightmost bit
	(*root_pagemap)[virt_address] = entry;
}

void set_pdpt_entry(pdpt* pagemap, uint64_t virt_address, pdt* pdt_addr, uint16_t flags, bool nx) {
	virt_address &= (0x7fc0000000); //Mask all but 9 bits that make up PDPT
	virt_address >>= 30; // Shift out all the zeros
	uint64_t entry = (uint64_t)pdt_addr & (0xffffffffff000); // Mask out space for flags, since address must be 4K aligned (2^12)
	flags &= 0xfff; // Make sure we only have 12 bits of flags
	entry |= flags;
	entry |= ((uint64_t)nx) << 63; // Add in nx flag in rightmost bit
	(*pagemap)[virt_address] = entry;
}

void set_pdt_entry(pdt* pagemap, uint64_t virt_address, pt* pt_addr, uint16_t flags, bool nx) {
	virt_address &= (0x3fe00000); //Mask all but 9 bits that make up PDT
	virt_address >>= 21; // Shift out all the zeros
	uint64_t entry = (uint64_t)pt_addr & (0xffffffffff000); // Mask out space for flags, since address must be 4K aligned (2^12)
	flags &= 0xfff; // Make sure we only have 12 bits of flags
	entry |= flags;
	entry |= ((uint64_t)nx) << 63; // Add in nx flag in rightmost bit
	(*pagemap)[virt_address] = entry;
}

void set_pt_entry(pdt* pagemap, uint64_t virt_address, void* page_address, uint16_t flags, bool nx) {
	virt_address &= (0x1ff000); //Mask all but 9 bits that make up PT
	virt_address >>= 12; // Shift out all the zeros
	uint64_t entry = (uint64_t)page_address & (0xffffffffff000); // Mask out space for flags, since address must be 4K aligned (2^12)
	flags &= 0xfff; // Make sure we only have 12 bits of flags
	entry |= flags;
	entry |= ((uint64_t)nx) << 63; // Add in nx flag in rightmost bit
	(*pagemap)[virt_address] = entry;
}

void insert_page_table(pml4t* root_pagemap, uint64_t virt_address, void* phys_address, uint8_t size, uint64_t flags, bool nx) {
	uint64_t pml4t_index = (virt_address & 0xff8000000000) >> 39;
	uint64_t pdpt_index = (virt_address & 0x7fc0000000) >> 39;
	uint64_t pdt_index = (virt_address & 0x3fe00000) >> 39;
	uint64_t pt_index = (virt_address & 0x1ff000) >> 39;
	pdpt* pdpt_addr;
	pdt* pdt_addr;
	pt* pt_addr;
	if ((*root_pagemap)[pml4t_index] & PG_FLAG_PRESENT) { // Check if a pml4 entry already exists
		pdpt_addr = (pdpt*)((*root_pagemap)[pml4t_index] & (0x000FFFFFFFFFF000));
	} else { // Create a new pdpt
		pdpt_addr = get_next_page();
	}
	if (size == 2) { // If size == 2, make a 1G page
		set_pdpt_entry(pdpt_addr, virt_address, phys_address, flags | PG_FLAG_PAGE_SIZE, nx);
		return;
	}
	if ((*pdpt_addr)[pdpt_index] & PG_FLAG_PRESENT) {
		pdt_addr = (pdt*)((*pdpt_addr)[pdpt_index] & (0x000FFFFFFFFFF000));
	} else {
		pdt_addr = get_next_page();
	}
}

meminfo_entry* get_free_areas_addr() {
	return free_areas;
}
uint8_t get_free_areas_len() {
	return free_areas_len;
}
meminfo_entry* get_reserved_areas_addr() {
	return reserved_areas;
}
uint8_t get_reserved_areas_len() {
	return reserved_areas_len;
}


