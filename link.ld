ENTRY(_start)
KERNEL_LMA = 1M;
KERNEL_VMA = 0xffffffff80000000;
SECTIONS
{
	. = 1K;
	.multiboot :
	{
		src/kernel/bootstrap.o (.multiboot)
	}
	. = KERNEL_LMA;

	.bootstrap :
	{
		src/kernel/bootstrap.o (.text)
		src/kernel/pageinit.o (.text)
		src/kernel/pageinit.o (.rodata)
	}
	.bootstrap_stack :
	{
		src/kernel/bootstrap.o (.bss)
		src/kernel/pageinit.o (.bss)
		src/kernel/pageinit.o (.data)
	}

	. += KERNEL_VMA;

	.text : AT(ADDR(.text) - KERNEL_VMA)
	{
		_code = .;
		*(EXCLUDE_FILE(*src/kernel/bootstrap.o) .text)
		. = ALIGN(4096);
	}

	.rodata : AT(ADDR(.rodata) - KERNEL_VMA)
	{
		_rodata = .;
		*(.rodata*)
		. = ALIGN(4096);
	}

	.data : AT(ADDR(.data) - KERNEL_VMA)
	{
		_data = .;
		*(.data)
		. = ALIGN(4096);
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		_bss = .;
		*(.bss)
		*(COMMON)
	}

	_end = .;
	_first_free_real_page = ALIGN(4K) - KERNEL_VMA;

	/DISCARD/ :
	{
		*(.comment)
	}
}
