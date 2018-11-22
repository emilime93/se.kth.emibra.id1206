/* Define a terminal window size */
#define COLUMNS 80
#define ROWS 25

// Text to be printed
char *name = "My os is running // Emil .L .B";

typedef struct vga_char {
    char character;
    char color;
} vga_char;

void myos_main(void) {
    // The VGA terminal mapped memory
    vga_char *vga = (vga_char*) 0xb8000;

    for (int i = 0; i < COLUMNS*ROWS; i++) {
        vga[i].character = ' ';
        vga[i].color = 0x0f;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        vga[600+i].character = name[i];
        vga[600+i].color = 0x0f;
    }
}