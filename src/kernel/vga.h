#include <stdint.h>
#include <stddef.h>

enum vga_color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 2,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

uint8_t make_color(enum vga_color fg, enum vga_color bg);

void terminal_initialize();
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void print_hex64(uint64_t tmp);
void print_hex32(uint32_t tmp);
void print_hex16(uint16_t tmp);
void print_dec64(uint64_t tmp);
void print_dec32(uint32_t tmp);
void print_dec16(uint16_t tmp);
void print_bin64(uint64_t tmp);
void print_bin32(uint32_t tmp);
void print_bin16(uint16_t tmp);
void print_bin8(uint8_t tmp);
