#include <stdint.h>
typedef struct __attribute__((packed)) {
	uint64_t mem_addr;
	uint64_t mem_length;
	uint32_t type;
} meminfo_entry;
void setup_memory(uint32_t* multiboot_info_p );
meminfo_entry* get_free_areas_addr();
uint8_t get_free_areas_len();
meminfo_entry* get_reserved_areas_addr();
uint8_t get_reserved_areas_len();
