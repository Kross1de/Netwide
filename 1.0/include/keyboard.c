// keyboard.c
#include "kernel.h"

static int shift_pressed = 0;
static int ctrl_pressed = 0;

const char keymap_lower[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
    '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const char keymap_upper[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0,
    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
    '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int keyboard_ready(void) {
    return inb(0x64) & 0x01;
}

char get_key(void) {
    while (!keyboard_ready());

    unsigned char scancode = inb(0x60);
    
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        if (scancode == 0x1D) {
            ctrl_pressed = 0;
        }
        return 0;
    }
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }
    if (scancode == 0x1D) {
        ctrl_pressed = 1;
        return 0;
    }
    
    if (scancode < 128) {
        if (shift_pressed) {
            return keymap_upper[scancode];
        } else {
            return keymap_lower[scancode];
        }
    }
    return 0;
}