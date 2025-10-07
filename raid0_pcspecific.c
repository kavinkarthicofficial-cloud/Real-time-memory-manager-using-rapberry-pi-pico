#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_SIZE 200
#define DISK1_FILE "disk1.bin"
#define DISK2_FILE "disk2.bin"

void raid0_striping() {
    char data[DISK_SIZE];
    FILE *disk1, *disk2;
    
    printf("Enter data to write (max %d characters):\n", DISK_SIZE);
    fgets(data, DISK_SIZE, stdin);
    data[strcspn(data, "\n")] = '\0';

    disk1 = fopen(DISK1_FILE, "w");
    disk2 = fopen(DISK2_FILE, "w");
    if (!disk1 || !disk2) {
        printf("Error opening disk files for writing!\n");
        return;
    }

    for (int i = 0; i < strlen(data); i++) {
        if (i % 2 == 0)
            fputc(data[i], disk1);
        else
            fputc(data[i], disk2);
    }

    fclose(disk1);
    fclose(disk2);

    printf("Data written using RAID 0 striping.\n");

    disk1 = fopen(DISK1_FILE, "r");
    disk2 = fopen(DISK2_FILE, "r");
    if (!disk1 || !disk2) {
        printf("Error opening disk files for reading!\n");
        return;
    }

    char reconstructed[DISK_SIZE];
    int idx = 0;
    int ch1, ch2;

    while (!feof(disk1) || !feof(disk2)) {
        if (!feof(disk1) && (ch1 = fgetc(disk1)) != EOF)
            reconstructed[idx++] = (char)ch1;
        if (!feof(disk2) && (ch2 = fgetc(disk2)) != EOF)
            reconstructed[idx++] = (char)ch2;
    }

    reconstructed[idx] = '\0';
    fclose(disk1);
    fclose(disk2);

    printf("Reconstructed data: %s\n", reconstructed);

    printf("\nSimulating disk2 failure...\nTrying to reconstruct with only disk1:\n");
    disk1 = fopen(DISK1_FILE, "r");
    if (!disk1) {
        printf("Disk1 failed too. All data lost.\n");
        return;
    }

    printf("Partial data from disk1: ");
    while ((ch1 = fgetc(disk1)) != EOF) {
        printf("%c_", ch1);
    }
    printf("\n");
    fclose(disk1);
}

int main() {
    printf("--- RAID 0: Striping Simulation on PC ---\n");
    raid0_striping();
    return 0;
}
