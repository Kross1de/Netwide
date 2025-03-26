// kernel.c
#include "include/kernel.h"

void kmain(void) {
    clear_screen();
    set_color(COLOR_CYAN);
    print("=====================================\n");
    print("      Welcome to Netwide v1.0       \n");
    print("=====================================\n");
    set_color(COLOR_WHITE);
    print("Type 'help' to see available commands.\n\n");

    for (int i = 0; i < MAX_COMMAND_LENGTH; i++) {
        command_buffer[i] = 0;
    }
    command_index = 0;

    print_prompt();
    while (1) {
        char c = get_key();
        if (c == 0) continue;

        if (c == '\n') {
            print("\n");
            if (command_index > 0) {
                process_command();
            }
            print_prompt();
        } else if (c == '\b') {
            if (command_index > 0) {
                command_index--;
                char str[2] = {c, '\0'};
                print(str);
            }
        } else if (command_index < MAX_COMMAND_LENGTH - 1) {
            command_buffer[command_index++] = c;
            char str[2] = {c, '\0'};
            print(str);
        }
    }
}