bits 16

; All bits are shifted 4 bits to the left
; Because reasons

start:
    mov ax, 0x07C0      ; 0x07c00 is where we are
    add ax, 0x20        ; add 0x20 (512 when shifted)
    mov ss, ax          ; set the stack segment
    mov sp, 0x1000      ; set the stack pointer

    mov ax, 0x07C0      ; set the data segment
    mov ds, ax          ; hmmm?

    ; We will use a BIOS prodecure to print characters
    ; by placing to the [?] in the SI (Source Register)

    mov si, msg             ; pointer to the message in SI
    mov ah, 0x0E            ; print char BIOS procedure

; The printing procedure
.next:
    lodsb               ; next byte to from SI -> AL, then increment SI
    cmp al, 0           ; if the byte is zero (null byte for string end)
    je .done            ; jump to done
    int 0x10            ; invoke the BIOS system call by interrupting with arg 0x10 (per definition)
    ; interrupt-routine looks in the AL register and prints the byte there
    jmp .next           ; loop

.done:
    jmp $               ; jump to itself -> loop forever

msg: db 'Hello', 1, 2, 0x0      ; our string

; Lastly the MBR must be 512 bytes, and end with a signature (0xAA55).
; To achieve this, we will fill up content to 510 bytes and add 0xAA55 (2 bytes)

times 510-($-$$) db 0   ; fill up to 510 bytes
dw 0xAA55               ; and add the MBR signature