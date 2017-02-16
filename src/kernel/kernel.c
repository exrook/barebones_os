#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "lib.h"
#include "memory.h"

uint32_t* multiboot_info_p;

void print_memmap();
void print_hex();

void kernel_main() {
	terminal_initialize();
	terminal_setcolor(make_color(COLOR_BLACK, COLOR_GREEN));

	terminal_writestring("\nYOYOYO\n");
	// terminal_writestring("Hello, kernel World!\nTest\nNewline\nSupport\nYEEE");
	setup_memory(multiboot_info_p);
	print_memmap();
	terminal_writestring("\nKERNEL:");
	print_hex64((uint64_t)kernel_main);
}

//typedef struct __attribute__((packed)) {
//	uint64_t mem_addr;
//	uint64_t mem_length;
//	uint32_t type;
//} meminfo_entry; 

void print_memmap() {
	uint32_t flags = multiboot_info_p[0];
	uint32_t lower_mem_size = multiboot_info_p[1];
	uint32_t upper_mem_size = multiboot_info_p[2];
	uint32_t boot_device = multiboot_info_p[3];
	uint32_t cmdline = multiboot_info_p[4];
	uint32_t mmap_length = multiboot_info_p[11];
	uint32_t mmap_addr = multiboot_info_p[12];
	uint32_t bootloader_name = multiboot_info_p[16];
	terminal_setcolor(make_color(COLOR_BLACK, COLOR_WHITE));
	terminal_writestring("Flags:\n");
	print_bin32(flags);
	terminal_writestring("\n");
	if (flags|(1<<0)) {
		terminal_writestring("Lower and Upper memory limits present\n");
		terminal_writestring("Lower mem size: ");
		print_dec32(lower_mem_size);
		terminal_writestring(" KiB\nUpper mem size: ");
		print_dec32(upper_mem_size);
		terminal_writestring(" KiB\n");
	}
	if (flags|(1<<2)) {
		terminal_writestring("Bootloader cmdline: \"");
		terminal_writestring((char*)(uint64_t) cmdline);
		terminal_writestring("\"\n");
	}
	if (flags|(1<<6)) {
		terminal_writestring("BIOS Memory Map present at: ");
		print_hex64(mmap_addr);
		//terminal_writestring("\nLength: ");
		//print_dec32(mmap_length);

		//terminal_writestring(" Bytes\n");
		//terminal_setcolor(make_color(COLOR_WHITE,COLOR_GREEN));
		//uint32_t i = 0;
		//uint8_t* mmap = &(((uint8_t*) mmap_addr)[0]);
		//while (i < mmap_length) {
		//	uint32_t size = mmap[i];
		//	meminfo_entry* entry = &mmap[i+4];
		//	terminal_writestring("Entry Size: ");
		//	print_dec32(size);
		//	terminal_writestring(" Bytes Mem address: ");
		//	print_hex64(entry->mem_addr);
		//	terminal_writestring("\nMem Size: ");
		//	print_hex64(entry->mem_length);
		//	terminal_writestring(" Bytes Type: ");
		//	print_dec32(entry->type);
		//	terminal_writestring("\n");
		//	i = i + size + 4;
		//}
		uint32_t i = 0;
		meminfo_entry* map_res = get_reserved_areas_addr();
		while (i < get_reserved_areas_len()) {
			meminfo_entry* entry = &map_res[i];
			terminal_writestring("Mem address: ");
			print_hex64(entry->mem_addr);
			terminal_writestring("\nMem Size: ");
			print_hex64(entry->mem_length);
			terminal_writestring(" Bytes Type: ");
			print_dec32(entry->type);
			terminal_writestring("\n");
			i += 1;
		}
		i = 0;
		map_res = get_free_areas_addr();
		while (i < get_free_areas_len()) {
			meminfo_entry* entry = &map_res[i];
			terminal_writestring("Mem address: ");
			print_hex64(entry->mem_addr);
			terminal_writestring("\nMem Size: ");
			print_hex64(entry->mem_length);
			terminal_writestring(" Bytes Type: ");
			print_dec32(entry->type);
			terminal_writestring("\n");
			i += 1;
		}
	}
	terminal_writestring("\n\n\nmmmm\n\n\n");
	terminal_writestring((char*) (uint64_t)bootloader_name);
}
