#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Color tokens for our layout
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_DEFAULT "\033[0m"

void print_logo_and_specs() {
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
    print_logo_and_specs();
    return 0;
}
