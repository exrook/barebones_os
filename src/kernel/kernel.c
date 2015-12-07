#include <stddef.h>
#include <stdint.h>
#include "vga.h"
#include "lib.h"

void kernel_main() {
	terminal_initialize();

	terminal_writestring("Hello, kernel World!\n");
}
