// screen.c
#include "kernel.h"

unsigned int current_loc = 0;
char *vidptr = (char*)0xb8000;
unsigned int lines = 0;
unsigned char current_color = COLOR_WHITE;

void set_color(unsigned char color) {
    current_color = color;
}

void clear_screen(void) {
    unsigned int i = 0;
    while (i < SCREENSIZE) {
        vidptr[i++] = ' ';
        vidptr[i++] = current_color;
    }
    current_loc = 0;
    lines = 0;
    update_cursor();
}

void update_cursor(void) {
    unsigned short position = current_loc;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

void print(const char *str) {
    unsigned int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            current_loc += 80 - (current_loc % 80);
            lines++;
        } else if (str[i] == '\b') {
            if (current_loc > 0) {
                current_loc--;
                vidptr[current_loc * 2] = ' ';
                vidptr[current_loc * 2 + 1] = current_color;
            }
        } else {
            vidptr[current_loc * 2] = str[i];
            vidptr[current_loc * 2 + 1] = current_color;
            current_loc++;
        }

        if (current_loc >= 80 * 25) {
            current_loc = 0;
        }
        update_cursor();
        i++;
    }

    if (lines >= LINES) {
        clear_screen();
        lines = 0;
    }
}

void print_prompt(void) {
    set_color(COLOR_GREEN);
    print("OS> ");
    set_color(COLOR_WHITE);
}