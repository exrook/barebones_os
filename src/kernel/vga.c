#include "lib.h"
#include "vga.h"

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c) {
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = 0;
		}
	}
}

void terminal_writestring(const char* data) {
	size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; i++) {
		if (data[i] == '\n') {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT) {
				terminal_row = 0;
			};
		} else {
			terminal_putchar(data[i]);
		}
	}
}

void terminal_reverse_writestring(const char* data) {
	size_t datalen = strlen(data);
	for (size_t i = datalen; i > 0; i--) {
		if (data[i-1] == '\n') {
			terminal_column = 0;
			terminal_row++;
		} else {
			terminal_putchar(data[i-1]);
		}
	}
}

void print_hex64(uint64_t tmp) {
	char out[17];
	out[16] = 0;
	for (int i = 0; i < 16; i++) {
		char c = tmp % 16;
		tmp = tmp >> 4;
		if (c < 10) {
			out[i] = c + 0x30;
			c = c + 0x30;
		} else {
			out[i] = c + 0x37;
			c = c + 0x37;
		}
	}
	terminal_writestring("0x");
	terminal_reverse_writestring(out);
}

void print_hex32(uint32_t tmp) {
	char out[9];
	out[8] = 0;
	for (int i = 0; i < 8; i++) {
		char c = tmp % 16;
		tmp = tmp >> 4;
		if (c < 10) {
			out[i] = c + 0x30;
			c = c + 0x30;
		} else {
			out[i] = c + 0x37;
			c = c + 0x37;
		}
	}
	terminal_writestring("0x");
	terminal_reverse_writestring(out);
}

void print_hex16(uint16_t tmp) {
	char out[5];
	out[4] = 0;
	for (int i = 0; i < 4; i++) {
		char c = tmp % 16;
		tmp = tmp >> 4;
		if (c < 10) {
			out[i] = c + 0x30;
			c = c + 0x30;
		} else {
			out[i] = c + 0x37;
			c = c + 0x37;
		}
	}
	terminal_writestring("0x");
	terminal_reverse_writestring(out);
}

void print_bin64(uint64_t tmp) {
	char out[65];
	out[64] = 0;
	for (int i = 0; i < 64; i++) {
		char c = tmp % 2;
		tmp = tmp >> 1;
		out[i] = c + 0x30;
	}
	terminal_writestring("0b");
	terminal_reverse_writestring(out);
}

void print_bin32(uint32_t tmp) {
	char out[33];
	out[32] = 0;
	for (int i = 0; i < 32; i++) {
		char c = tmp % 2;
		tmp = tmp >> 1;
		out[i] = c + 0x30;
	}
	terminal_writestring("0b");
	terminal_reverse_writestring(out);
}

void print_bin16(uint16_t tmp) {
	char out[17];
	out[16] = 0;
	for (int i = 0; i < 16; i++) {
		char c = tmp % 2;
		tmp = tmp >> 1;
		out[i] = c + 0x30;
	}
	// terminal_writestring("0b");
	terminal_reverse_writestring(out);
}

void print_bin8(uint8_t tmp) {
	char out[9];
	out[8] = 0;
	for (int i = 0; i < 8; i++) {
		char c = tmp % 2;
		tmp = tmp >> 1;
		out[i] = c + 0x30;
	}
	terminal_reverse_writestring(out);
}

void print_dec64(uint64_t tmp) {
	char out[21];
	out[20] = 0;
	for (int i = 0; i < 20; i++) {
		char c = tmp % 10;
		tmp = tmp / 10;
		out[i] = c + 0x30;
		c = c + 0x30;
	}
	terminal_reverse_writestring(out);
}

void print_dec32(uint32_t tmp) {
	char out[11];
	out[10] = 0;
	for (int i = 0; i < 10; i++) {
		char c = tmp % 10;
		tmp = tmp / 10;
		out[i] = c + 0x30;
		c = c + 0x30;
	}
	terminal_reverse_writestring(out);
}

void print_dec16(uint16_t tmp) {
	char out[6];
	out[5] = 0;
	for (int i = 0; i < 5; i++) {
		char c = tmp % 10;
		tmp = tmp / 10;
		out[i] = c + 0x30;
		c = c + 0x30;
	}
	terminal_reverse_writestring(out);
}
