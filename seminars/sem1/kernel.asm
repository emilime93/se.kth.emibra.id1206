; This is the kernel
; Will run in 32 bit protected mode (whatever that means)

; Terminal bytes starts at 0xB8000 and spans 80*25=2000 characters
; Each character is 2 bytes
; Each byte looks like this:
; {1 bit blinking}{3 bits background}{4 bits color} | {character code}

global start

section .text
bits 32
start:
    mov dword [0xB8000], 0x2f6D2f45         ; M E
    mov dword [0xB8004], 0x2f6C2f69         ; L I
    mov dword [0xB8008], 0x2f4F2f20         ; O _
    mov dword [0xB800C], 0x2f002f53         ; _ S
    mov dword [0xB8010], 0x2f432f28         ; C (
    mov dword [0xB8014], 0x00002f29         ;   )
    hlt

; EMIL OS (C)
; Emil = 45 6D 69 6C
; 20 = space
; OS = 4f 53
; (C) = 28 43 29