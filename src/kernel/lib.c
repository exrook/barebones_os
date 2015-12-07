#include <stddef.h>
#include "lib.h"

uint16_t strlen(const char* str) {
	size_t ret = 0;
	while(str[ret] != 0) {
		ret++;
	}
	return ret;
}
