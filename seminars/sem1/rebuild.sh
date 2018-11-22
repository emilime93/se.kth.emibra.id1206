nasm -f elf64 multiboot_header.asm
nasm -f elf64 kernel.asm
ld -n -o kernel.bin -T linker.ld multiboot_header.o kernel.o
cp kernel.bin cdrom/boot/kernel.bin
grub-mkrescue -o cdrom.iso cdrom
qemu-system-x86_64 -cdrom cdrom.iso
