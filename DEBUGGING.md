Debugging with GDB and QEMU
===========================
Start Qemu with -s and -S
Start a gdb session, connect with
	target remote localhost:1234
Then load symbols
	symbol-file kernel.img
Set breakpoint on 64bit entry point
	break code64
Run until the breakpoint is reached
	c
Open a second gdb session, attempt to connect with
	target remote localhost:1234
Close first gdb with ^D
	
