; This file is needed for GRUB to recognize it as a kernel
; because of the way the MBR is written

; This contains a *Magic number, *Architecture Identifier, and *Header length

section .multiboot_header

magic   equ 0xE85250D6        ; multiboot 2
arch    equ 0                 ; protected mode i386

header_start:
    dd magic                        ; magic number
    dd arch                         ; architecture
    dd header_end - header_start    ; header leght
    dd 0x100000000 - (magic + arch + (header_end-header_start)) ;chsum
    dw 0                            ; type
    dw 0                            ; flags
    dw 8                            ; size
header_end: