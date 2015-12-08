OBJS=src/kernel/bootstrap.o src/kernel/pageinit.o src/kernel/vga.o src/kernel/lib.o src/kernel/kernel.o
LD_SCRIPT=link.ld
LDFLAGS=-Wl,--build-id=none
CFLAGS=-ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O2 -nostdlib -target x86_64--elf -g
ASMFLAGS=-felf64

.PHONY: clean run debug

kernel.bin: $(OBJS) $(LD_SCRIPT)
	clang -T $(LD_SCRIPT) -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS)
%.o: %.c
	clang -c $< -o $@ $(CFLAGS)

%.o: %.asm
	yasm $(ASMFLAGS) $< -o $@

clean:
	rm -f kernel.bin $(OBJS)
run: kernel.bin
	qemu-system-x86_64 -s -kernel kernel.bin
debug: kernel.bin
	qemu-system-x86_64 -S -s -kernel kernel.bin
kernel.iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/
	cp kernel.bin isodir/boot/
	grub-mkrescue -d /usr/lib/grub/i386-pc/ -o kernel.iso isodir
iso: kernel.iso
runiso: iso
	qemu-system-x86_64 -s -cdrom kernel.iso
debugiso: iso
	qemu-system-x86_64 -s -S -cdrom kernel.iso
