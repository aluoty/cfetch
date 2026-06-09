#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_CYAN    "\033[1;36m"
#define COLOR_DEFAULT "\033[0m"

// Functions to parse hardware and system metrics into strings
void get_kernel(char *buffer, size_t size) {
    FILE *file = fopen("/proc/sys/kernel/osrelease", "r");
    if (file != NULL) {
        if (fgets(buffer, size, file) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0; // Strip newline
        }
        fclose(file);
    } else {
        strncpy(buffer, "Unknown", size);
    }
}

void get_uptime(char *buffer, size_t size) {
    FILE *file = fopen("/proc/uptime", "r");
    if (!file) {
        strncpy(buffer, "Unknown", size);
        return;
    }
    double up_seconds;
    if (fscanf(file, "%lf", &up_seconds) == 1) {
        int hours = (int)up_seconds / 3600;
        int minutes = ((int)up_seconds % 3600) / 60;
        snprintf(buffer, size, "%dh %dm", hours, minutes);
    }
    fclose(file);
}

void get_cpu(char *buffer, size_t size) {
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        strncpy(buffer, "Unknown", size);
        return;
    }
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *cpu_name = strchr(line, ':');
            if (cpu_name) {
                cpu_name += 2; // Skip colon and space
                cpu_name[strcspn(cpu_name, "\n")] = 0;
                strncpy(buffer, cpu_name, size);
                break;
            }
        }
    }
    fclose(file);
}

void get_memory(char *buffer, size_t size) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        strncpy(buffer, "Unknown", size);
        return;
    }
    char line[128];
    long total_mem = 0, avail_mem = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &total_mem);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %ld", &avail_mem);
        }
        if (total_mem && avail_mem) break;
    }
    fclose(file);

    long used_mem = (total_mem - avail_mem) / 1024;
    snprintf(buffer, size, "%ldMB / %ldMB", used_mem, total_mem / 1024);
}

void get_shell(char *buffer, size_t size) {
    // getenv pulls the environmental variable pointer from the user session
    char *shell_path = getenv("SHELL");

    if (shell_path != NULL) {
        // Find the last forward slash in "/bin/bash" -> "bash"
        char *shell_name = strrchr(shell_path, '/');
        
        if (shell_name != NULL) {
            // shell_name points to "/bash", so we add 1 to get just "bash"
            strncpy(buffer, shell_name + 1, size - 1);
        } else {
            // Fallback if no slash was found in the environment string
            strncpy(buffer, shell_path, size - 1);
        }
    } else {
        // If getenv returned NULL, the variable doesn't exist
        strncpy(buffer, "Unknown", size - 1);
    }
    
    // Explicit null-termination guarantee for absolute C string safety
    buffer[size - 1] = '\0';
}

int main() {
    // Buffers to store parsed strings
    char kernel[64] = {0};
    char uptime[32] = {0};
    char cpu[128]  = {0};
    char memory[64] = {0};
    char shell[64] = {0};

    // Run the parsers
    get_kernel(kernel, sizeof(kernel));
    get_uptime(uptime, sizeof(uptime));
    get_cpu(cpu, sizeof(cpu));
    get_memory(memory, sizeof(memory));
    get_shell(shell, sizeof(shell));

    // Print the unified side-by-side layout
    printf(COLOR_CYAN " _     _   _______   _______  " COLOR_DEFAULT "OS:      uos\n");
    printf(COLOR_CYAN "| |   | | |  ___  | |  _____| " COLOR_DEFAULT "Kernel:  %s\n", kernel);
    printf(COLOR_CYAN "| |   | | | |   | | | |_____  " COLOR_DEFAULT "Uptime:  %s\n", uptime);
    printf(COLOR_CYAN "| |   | | | |   | | |_____  | " COLOR_DEFAULT "Memory:  %s\n", memory);
    printf(COLOR_CYAN "| |___| | | |___| |  _____| | " COLOR_DEFAULT "CPU:     %s\n", cpu);
    printf(COLOR_CYAN " \\_____/  |_______| |_______| " COLOR_DEFAULT "Shell:  %s\n", shell);
    printf("\n");

    return 0;
}
