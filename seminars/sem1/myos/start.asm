global start
extern myos_main

bits 32

section .text

start:
    cli             ; disable interrupts
    mov esp, stack  ; flytta in "stack" i stack-pekar-registret
    call myos_main  ; annrop till myos_main (c-kod?)
    hlt

section .bss

resb    8192        ; paddar ut 8192 bytes, för stacken unden
stack:              ; märker ut addressen