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
	
	; Enable PAE
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

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
	push code64
extern page_init
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
GDT64:				;GDT (64 Bit)
	.Null: equ $ - GDT64	;Null Descriptor
	dw 0			;
	dw 0
	db 0
	db 0
	db 0
	db 0
	.Code: equ $ - GDT64	;Code Segment Descriptor
	dw 0			;Segment Descriptor
	dw 0			;Base Address [0:15]
	db 0			;Base Address [16:23]
	db 10011010b
	;  7  6  5  4  3  2  1  0
	; |P| DPL | S|    Type   |
	;  1  0  0  1  1  0  1  0 
	;  P - Present Bit
	;  DPL - Descriptor Privilege Level
	;  Type - Type Field
	db 00100000b
	;  7  6  5  4  3  2  1  0
	; |G| D| L|AVL|   Limit  | 
	;  0  0  1  0  0  0  0  0
	; G - 
	; D - 
	; L - Long Mode
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

[BITS 64]
global code64
code64:
	cli
	extern kernel_main
	call page_init
	; Map kernel memory in page table, then jump to kernel
	jmp kernel_main