OBJS=src/kernel/boot.o src/kernel/kernel.o
LD_SCRIPT=linker.ld
LDFLAGS=-Wl,--build-id=none
CFLAGS=-ffreestanding -O2 -nostdlib -target i686-elf

.PHONY: clean run

kernel.bin: $(OBJS) $(LD_SCRIPT)
	clang -T $(LD_SCRIPT) -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS)
%.o: %.c
	clang -c $< -o $@ $(CFLAGS)

%.o: %.asm
	yasm -felf32 $< -o $@

clean:
	rm -f kernel.bin $(OBJS)
run:
	qemu-system-x86_64 -kernel kernel.bin
