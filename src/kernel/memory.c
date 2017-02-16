#include <stdint.h>

#include "memory.h"

void* _first_free_real_page;
static void* next_free_page;

typedef uint64_t* pml4_entry; // Page Map Level 4
typedef pml4_entry* pml4t[512];

typedef uint64_t* pdp_entry; // Page Directory Pointer
typedef pdp_entry* pdpt[512];

typedef uint64_t* pd_entry; // Page Directory
typedef pd_entry* pdt[512];

typedef uint64_t* pt_entry; // Page Table
typedef pt_entry* pt[512];

void setup_page_tables();
void* get_next_page();
void set_pml4t_entry(pml4t root_pagemap, void* virt_address, pdpt entry, uint16_t flags);

static uint8_t free_areas_len;
static meminfo_entry free_areas[128];

static uint8_t reserved_areas_len;
static meminfo_entry reserved_areas[128];

pml4t* kernel_pml4t;
pdpt* kernel_pdpt;

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
		uint8_t* mmap = &(((uint8_t*) mmap_addr)[0]);
		while (i < mmap_length) {
			uint32_t size = mmap[i];
			meminfo_entry* entry = &mmap[i+4];
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
	set_pml4t_entry(kernel_pml4t, 0xffffffff80000000, kernel_pdpt, 0x03);
}

void* get_next_page() {
	void* page = next_free_page;
	next_free_page += 4096;
	return page;
}

void set_pml4t_entry(pml4t root_pagemap, void* virt_address, pdpt entry, uint16_t flags) {

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


