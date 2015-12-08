BITS 32
; Multiboot header constants
MBALIGN		equ 1<<0
MEMINFO		equ 1<<1
FLAGS		equ MBALIGN | MEMINFO
MAGIC		equ 0x1BADB002
CHECKSUM	equ -(MAGIC+FLAGS)
; Multiboot header
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
section .bss, nobits
align 4
stack_bottom:
resb 16384
stack_top:


;Actual code
section .text
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
global _start
_start:
	;jmp .halt
	;in kernel mode
	mov esp, stack_top
	
	;check for CPUID
	pushfd 
	pop eax
	mov ecx, eax
	xor eax, 1 << 21
	push eax
	popfd
	pushfd
	pop eax
	push ecx
	popfd
	xor eax, ecx
	jz .NoCPUID

	;Check for extended cpuid
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .NoLongMode

	;Check for longmode
	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz .NoLongMode

	;Clear memory for paging tables
	mov edi, 0x1000
	mov cr3, edi
	xor eax, eax
	mov ecx, 4096
	rep stosd
	mov edi, cr3

	;Create Paging Tables in memory
	mov DWORD [edi], 0x2003
	add edi, 0x1000
	mov DWORD [edi], 0x3003
	add edi, 0x1000
	mov DWORD [edi], 0x4003
	add edi, 0x1000

	mov ebx, 0x00000003
	mov ecx, 512

.SetEntry:
	mov DWORD [edi], ebx
	add ebx, 0x1000
	add edi, 8
	loop .SetEntry

	;Set Long Mode Bit
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr
	
	;Enable Paging
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	; Load GDT
	lgdt [GDT64.Pointer]
	jmp GDT64.Code:code64
.halt:
	cli
.hang:
	hlt
	jmp .hang
.NoCPUID:
	jmp .halt
.NoLongMode:
	jmp .halt
GDT64:
	.Null: equ $ - GDT64
	dw 0
	dw 0
	db 0
	db 0
	db 0
	db 0
	.Code: equ $ - GDT64
	dw 0
	dw 0
	db 0
	db 10011010b
	db 00100000b
	db 0
	.Data: equ $ - GDT64
	dw 0
	dw 0
	db 0
	db 10010010b
	db 00000000b
	db 0
	.Pointer:
	dw $ - GDT64 - 1
	dq GDT64

BITS 64
global code64
code64:
	; Map kernel memory in page table, then jump to kernel
	extern page_init
	call page_init
	extern kernel_main
	jmp kernel_main
