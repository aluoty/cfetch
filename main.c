#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Color tokens for our layout
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_DEFAULT "\033[0m"

void print_memory() {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) return;

    char line[128];
    long total_mem = 0, avail_mem = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &total_mem);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %ld", &avail_mem);
        }
        // Stop reading once we have both to save CPU cycles
        if (total_mem && avail_mem) break;
    }
    fclose(file);

    long used_mem = (total_mem - avail_mem) / 1024;
    printf("Memory:  %ldMB / %ldMB\n", used_mem, total_mem / 1024);
}

void print_uptime() {
    FILE *file = fopen("/proc/uptime", "r");
    if (!file) return;

    double up_seconds;
    if (fscanf(file, "%lf", &up_seconds) == 1) {
        int hours = (int)up_seconds / 3600;
        int minutes = ((int)up_seconds % 3600) / 60;
        printf("Uptime:  %dh %dm\n", hours, minutes);
    }
    fclose(file);
}

void print_cpu() {
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (!file) return;

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "model name", 10) == 0) {
            // Find the colon separating the key and the value
            char *cpu_name = strchr(line, ':');
            if (cpu_name) {
                cpu_name += 2; // Skip the colon and the space next to it
                cpu_name[strcspn(cpu_name, "\n")] = 0; // Strip trailing newline
                printf("CPU:     %s\n", cpu_name);
                break;
            }
        }
    }
    fclose(file);
}

void print_system() {
    // Open the kernel release file provided by the virtual filesystem
    FILE *kernel_file = fopen("/proc/sys/kernel/osrelease", "r");
    char kernel_buffer[64] = "Unknown";

    if (kernel_file != NULL) {
        // Read the single line of text containing the version
        if (fgets(kernel_buffer, sizeof(kernel_buffer), kernel_file) != NULL) {
            // Strip the trailing newline character if it exists
            kernel_buffer[strcspn(kernel_buffer, "\n")] = 0;
        }
        fclose(kernel_file);
    }

    // Render our layout using standard ANSI escapes
    // You can replace the simple OS logo with a custom ASCII layout later!
    printf(COLOR_CYAN "  /\\_/\\   " COLOR_DEFAULT "OS:      MyCustomOS\n");
    printf(COLOR_CYAN " ( o.o )  " COLOR_DEFAULT "Kernel:  %s\n", kernel_buffer);
    printf(COLOR_CYAN "  > ^ <   " COLOR_DEFAULT "Shell:   bash (temporary)\n");
}

int main() {
    print_system();
    print_memory();
    print_uptime();
    print_cpu();
    return 0;
}
