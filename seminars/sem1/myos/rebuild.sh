#!/bin/bash
nasm -f elf32 multiboot_header.asm
nasm -f elf32 start.asm
gcc -m32 -c myos.c
ld -m elf_i386 -o myos.bin -T myos_linker.ld multiboot_header.o start.o myos.o
cp myos.bin cdrom/boot/myos.bin
grub-mkrescue -o cdrom.iso cdrom
qemu-system-x86_64 -cdrom cdrom.iso