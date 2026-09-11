; SerenWine boot entry - Multiboot2 header + 32-bit entry
; Sets up a temporary stack, calls kernel_main, halts on return.

MBALIGN     equ 1<<0
MEMINFO     equ 1<<1
FLAGS       equ MBALIGN | MEMINFO
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB kernel stack
stack_top:

section .text
global _start
extern kernel_main
_start:
    mov esp, stack_top
    push ebx        ; multiboot info pointer
    push eax        ; multiboot magic
    cli
    call kernel_main
.hang:
    hlt
    jmp .hang
