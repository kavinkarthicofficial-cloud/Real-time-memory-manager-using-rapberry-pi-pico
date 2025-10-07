#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ff.h"  // FatFs library for SD card

void scan(int requests[], int n, int head, int disk_size, int direction) {
    // ... (same as before)
}

int main() {
    stdio_init_all(); // Initialize USB serial for printf
    
    // Mount SD card
    FATFS fs;
    FIL fil;
    if (f_mount(&fs, "0:", 1) != FR_OK) {
        printf("SD card mount failed!\n");
        return -1;
    }

    // Open a file for writing
    if (f_open(&fil, "scan_output.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        printf("Failed to open file!\n");
        return -1;
    }

    int requests[] = {98, 183, 37, 122, 14, 124, 65, 67};
    int n = sizeof(requests) / sizeof(requests[0]);
    int head = 53;
    int disk_size = 200;
    int direction = 1;

    // Redirect output to SD card file
    char buffer[100];
    sprintf(buffer, "SCAN Seek Sequence: %d", head);
    f_puts(buffer, &fil);

    // ... (rest of the scan logic, writing to file instead of printf)

    f_close(&fil);
    f_unmount("0:");

    return 0;
}