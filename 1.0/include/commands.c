// commands.c
#include "kernel.h"

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
}

void command_cls(void) {
    clear_screen();
}

void command_sysinfo(void) {
    print("\nSystem Information:\n");
    print("  OS Name: Netwide\n");
    print("  Kernel: Ncore 0.0.1\n");
    print("  Version: 1.0\n");
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