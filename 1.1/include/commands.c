// commands.c
#include "kernel.h"
#include "vfs.h"

char command_buffer[MAX_COMMAND_LENGTH];
int command_index = 0;

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return *(unsigned char*)(s1 + i) - *(unsigned char*)(s2 + i);
        }
    }
    return 0;
}

void command_help(void) {
    print("\nAvailable commands:\n");
    print("  help       - Show this help message\n");
    print("  cls        - Clear the screen\n");
    print("  sysinfo    - Display system information\n");
    print("  calculator - Calculator (add two numbers)\n");
    print("  echo       - Echo the input text\n");
    print("  time       - Display system uptime\n");
    print("  factorial  - Calculate factorial of a number\n");
    print("  reboot     - Reboot the system\n");
    print("  shutdown   - Halt the system\n");
    print("   FS CMD'S\n");
    print("  list       - List directory contents\n");
    print("  cd         - Change directory\n");
    print("  md         - Create a new directory\n");
    print("  rm         - Remove a file or directory\n");
    print("  touch <filename> - Create a new file\n");
    print("  cat <filename> - Display the contents of a file\n");
}

void getfullpath(vfs_node_t* node, char* path, size_t size) {
    if (node == NULL || size == 0) {
        print("get_full_path: Invalid node or size\n");
        return;
    }

    char temp_stack[MAX_PATH][MAX_FILENAME];
    int depth = 0;

    // Traverse up to the root, storing names in temp_stack
    vfs_node_t* current = node;
    while (current != NULL && depth < MAX_PATH) {
        if (current->name == NULL) {
            print("getfullpath: Node name is NULL\n");
            return;
        }
        kstrncpy(temp_stack[depth], current->name, MAX_FILENAME);
        depth++;
        current = current->parent;

        // Check for cycles
        vfs_node_t* check = node;
        int steps = 0;
        while (check != NULL && steps < depth) {
            if (check == current) {
                print("getfullpath: Cycle detected in parent pointers\n");
                return;
            }
            check = check->parent;
            steps++;
        }
    }

    // Build the path from the root down
    path[0] = '\0';
    kstrncat(path, "/", size);
    for (int i = depth - 2; i >= 0; i--) { // Skip the root's name ("/")
        if (path[kstrlen(path) - 1] != '/') {
            kstrncat(path, "/", size);
        }
        kstrncat(path, temp_stack[i], size);
    }
}

void command_list(void) {
    char* arg = command_buffer + 4;
    while(*arg == ' ') arg++;

    if(*arg == '\0') {
        vfs_list_directory(".");
    } else {
        vfs_list_directory(arg);
    }
}

void command_cd(void) {
    char* arg = command_buffer + 2;
    while(*arg == ' ') arg++;

    if(*arg == '\0') {
        vfs_change_directory("/");
    } else {
        if(vfs_change_directory(arg) < 0) {
            print("cd: No such directory: ");
            print(arg);
            print("\n");
        }
    }
}

void command_md(void) {
    char* arg = command_buffer + 2;
    while(*arg == ' ') arg++;

    if(*arg == '\0') {
        print("md: missing argument\n");
        return;
    }

    if(vfs_create_directory(arg) < 0) {
        print("md: failed to create directory\n");
        print(arg);
        print("'\n");
    }
}

void command_touch(void) {
    char* arg = command_buffer + 5;
    while(*arg == ' ') arg++;

    if(*arg == '\0') {
        print("touch: missing argument\n");
        return;
    }

    if(vfs_create_file(arg) < 0) {
        print("touch: failed to create file\n");
        print(arg);
        print("'\n");
    }
}

void command_rm(void) {
    char* arg = command_buffer + 2;
    while (*arg == ' ') arg++;

    if (*arg == '\0') {
        print("rm: missing argument\n");
        return;
    }

    int recursive = 0;
    char* path = arg;

    if (strncmp(arg, "-r ", 3) == 0) {
        recursive = 1;
        path = arg + 3;
        while (*path == ' ') path++;
    }

    if (*path == '\0') {
        print("rm: missing path\n");
        return;
    }

    print("rm: attempting to delete '");
    print(path);
    print("'\n");

    vfs_node_t* node = vfs_find_node(path);
    if (!node) {
        print("rm: no such file or directory: ");
        print(path);
        print("\n");
        return;
    }

    print("rm: node found, type: ");
    print(node->type == VFS_TYPE_FILE ? "file" : "directory");
    print("\n");

    if (node->type == VFS_TYPE_DIRECTORY && node->children && !recursive) {
        print("rm: cannot remove '");
        print(path);
        print("': is a non-empty directory (use -r)\n");
        return;
    }

    int result = vfs_delete(path, recursive);
    if (result < 0) {
        print("rm: failed to delete '");
        print(path);
        print("'\n");
    } else {
        print("rm: successfully deleted '");
        print(path);
        print("'\n");
    }
}

void command_cat(void) {
    char* arg = command_buffer + 3;
    while(*arg == ' ') arg++;

    if(*arg == '\0') {
        print("cat: missing argument\n");
        return;
    }

    vfs_node_t* node = vfs_find_node(arg);
    if(!node || node->type != VFS_TYPE_FILE) {
        print("cat: file not found\n");
        print(arg);
        print("'\n");
        return;
    }

    if(node->data) {
        print(node->data);
    }
    print("\n");
}

void command_cls(void) {
    clear_screen();
}

void command_sysinfo(void) {
    print("\nSystem Information:\n");
    print("  OS Name: Netwide\n");
    print("  Kernel: Ncore 0.0.2\n");
    print("  Version: 1.1\n");
    print("  Screen: 80x25 text mode\n");
    print("  Keyboard: PS/2 compatible\n");
}

void command_calculator(void) {
    print("\nEnter two numbers to add (e.g., 5 3):\n");
    print_prompt();

    char num1_str[10] = {0};
    char num2_str[10] = {0};
    int num1_index = 0, num2_index = 0;
    int reading_first = 1;

    while (1) {
        char c = get_key();
        if (c == 0) continue;

        if (c == '\n') {
            if (num1_index > 0 && num2_index > 0) {
                int num1 = 0, num2 = 0;
                for (int i = 0; i < num1_index; i++) {
                    num1 = num1 * 10 + (num1_str[i] - '0');
                }
                for (int i = 0; i < num2_index; i++) {
                    num2 = num2 * 10 + (num2_str[i] - '0');
                }

                print("\nResult: ");
                char result_str[10];
                int result = num1 + num2;
                int i = 0;
                do {
                    result_str[i++] = (result % 10) + '0';
                    result /= 10;
                } while (result > 0);
                result_str[i] = '\0';

                for (int j = i - 1; j >= 0; j--) {
                    char temp[2] = {result_str[j], '\0'};
                    print(temp);
                }
                print("\n");
                break;
            }
        } else if (c == ' ' && reading_first) {
            reading_first = 0;
            char temp[2] = {c, '\0'};
            print(temp);
        } else if (c >= '0' && c <= '9') {
            if (reading_first) {
                if (num1_index < 9) {
                    num1_str[num1_index++] = c;
                    char temp[2] = {c, '\0'};
                    print(temp);
                }
            } else {
                if (num2_index < 9) {
                    num2_str[num2_index++] = c;
                    char temp[2] = {c, '\0'};
                    print(temp);
                }
            }
        }
    }
}

void command_echo(void) {
    int i = 0;
    while (command_buffer[i] != ' ' && command_buffer[i] != '\0') {
        i++;
    }
    if (command_buffer[i] == ' ') {
        i++;
        print("\n");
        while (command_buffer[i] != '\0') {
            char temp[2] = {command_buffer[i], '\0'};
            print(temp);
            i++;
        }
        print("\n");
    }
}

void command_time(void) {
    static unsigned long counter = 0;
    counter += 1000;

    print("\nUptime (approximate cycles): ");
    char time_str[10];
    int i = 0;
    unsigned long temp = counter;
    do {
        time_str[i++] = (temp % 10) + '0';
        temp /= 10;
    } while (temp > 0);
    time_str[i] = '\0';

    for (int j = i - 1; j >= 0; j--) {
        char temp_str[2] = {time_str[j], '\0'};
        print(temp_str);
    }
    print("\n");
}

void command_factorial(void) {
    print("\nEnter a number to calculate its factorial (0-12):\n");
    print_prompt();

    char num_str[10] = {0};
    int num_index = 0;

    while (1) {
        char c = get_key();
        if (c == 0) continue;

        if (c == '\n') {
            if (num_index > 0) {
                int num = 0;
                for (int i = 0; i < num_index; i++) {
                    num = num * 10 + (num_str[i] - '0');
                }

                if (num > 12) {
                    print("\nError: Number too large (max 12)\n");
                    break;
                }

                unsigned long fact = 1;
                for (int i = 1; i <= num; i++) {
                    fact *= i;
                }

                print("\nFactorial: ");
                char fact_str[10];
                int i = 0;
                unsigned long temp = fact;
                do {
                    fact_str[i++] = (temp % 10) + '0';
                    temp /= 10;
                } while (temp > 0);
                fact_str[i] = '\0';

                for (int j = i - 1; j >= 0; j--) {
                    char temp_str[2] = {fact_str[j], '\0'};
                    print(temp_str);
                }
                print("\n");
                break;
            }
        } else if (c >= '0' && c <= '9') {
            if (num_index < 9) {
                num_str[num_index++] = c;
                char temp[2] = {c, '\0'};
                print(temp);
            }
        }
    }
}

void command_reboot(void) {
    print("\nRebooting...\n");
    reboot();
}

void command_shutdown(void) {
    print("\nShutting down...\n");
    shutdown();
}

void process_command(void) {
    command_buffer[command_index] = '\0';

    if (strcmp(command_buffer, "help") == 0) {
        command_help();
    } else if (strcmp(command_buffer, "cls") == 0) {
        command_cls();
    } else if (strcmp(command_buffer, "sysinfo") == 0) {
        command_sysinfo();
    } else if (strcmp(command_buffer, "calculator") == 0) {
        command_calculator();
    } else if (strncmp(command_buffer, "echo ", 5) == 0) {
        command_echo();
    } else if (strcmp(command_buffer, "time") == 0) {
        command_time();
    } else if (strcmp(command_buffer, "factorial") == 0) {
        command_factorial();
    } else if (strcmp(command_buffer, "reboot") == 0) {
        command_reboot();
    } else if (strcmp(command_buffer, "shutdown") == 0) {
        command_shutdown();
    } else if (strncmp(command_buffer, "list", 4) == 0) { 
        command_list();
    } else if (strncmp(command_buffer, "cd ", 3) == 0 || strcmp(command_buffer, "cd") == 0) { 
        command_cd();
    } else if (strncmp(command_buffer, "md ", 3) == 0 || strcmp(command_buffer, "md") == 0) { 
        command_md();
    } else if (strncmp(command_buffer, "touch ", 6) == 0) { 
        command_touch();
    } else if (strncmp(command_buffer, "rm ", 3) == 0) { 
        command_rm();
    } else if (strncmp(command_buffer, "cat ", 4) == 0) { 
        command_cat();
    } else if (command_index > 0) {
        print("\nUnknown command: ");
        print(command_buffer);
        print("\nType 'help' for a list of commands.\n");
    }

    for (int i = 0; i < MAX_COMMAND_LENGTH; i++) {
        command_buffer[i] = 0;
    }
    command_index = 0;
}