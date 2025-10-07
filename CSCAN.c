#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define MAX_REQUESTS 100
#define CYLINDER_LIMIT 199  // Max track is from 0 to 199

// Global variables
SemaphoreHandle_t xSerialMutex;

// Simulated disk requests (could be replaced with real-time input)
const int test_requests[] = {176, 79, 34, 60, 92, 11, 41, 114};
const int test_request_count = 8;
const int test_head = 50;
const int test_direction = 1; // 1 for right, 0 for left

void cscan_task(void *pvParameters) {
    int requests[MAX_REQUESTS];
    int request_count = test_request_count;
    int head = test_head;
    int direction = test_direction;
    
    // Copy test requests (in a real application, these would come from input)
    for (int i = 0; i < request_count; i++) {
        requests[i] = test_requests[i];
    }

    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY) == pdTRUE) {
        printf("--- C-SCAN Disk Scheduling ---\n");
        printf("Requests: ");
        for (int i = 0; i < request_count; i++) {
            printf("%d ", requests[i]);
        }
        printf("\nHead: %d, Direction: %s\n", head, direction ? "Right" : "Left");
        xSemaphoreGive(xSerialMutex);
    }

    int left[MAX_REQUESTS + 1], right[MAX_REQUESTS + 1];
    int left_size = 0, right_size = 0;
    int seek_sequence[MAX_REQUESTS + 2];
    int seek_sequence_size = 0;
    int seek_count = 0;

    right[right_size++] = CYLINDER_LIMIT;
    left[left_size++] = 0;

    for (int i = 0; i < request_count; i++) {
        if (requests[i] < head) {
            left[left_size++] = requests[i];
        } else {
            right[right_size++] = requests[i];
        }
    }

    // Sort left array (bubble sort)
    for (int i = 0; i < left_size - 1; i++) {
        for (int j = 0; j < left_size - i - 1; j++) {
            if (left[j] > left[j + 1]) {
                int temp = left[j];
                left[j] = left[j + 1];
                left[j + 1] = temp;
            }
        }
    }

    // Sort right array (bubble sort)
    for (int i = 0; i < right_size - 1; i++) {
        for (int j = 0; j < right_size - i - 1; j++) {
            if (right[j] > right[j + 1]) {
                int temp = right[j];
                right[j] = right[j + 1];
                right[j + 1] = temp;
            }
        }
    }

    if (direction == 1) {  // Right direction
        for (int i = 0; i < right_size; i++) {
            seek_sequence[seek_sequence_size++] = right[i];
            seek_count += abs(right[i] - head);
            head = right[i];
        }

        seek_count += abs(CYLINDER_LIMIT - 0);  // Jump from end to start
        head = 0;

        for (int i = 0; i < left_size; i++) {
            seek_sequence[seek_sequence_size++] = left[i];
            seek_count += abs(left[i] - head);
            head = left[i];
        }
    } else {  // Left direction
        for (int i = left_size - 1; i >= 0; i--) {
            seek_sequence[seek_sequence_size++] = left[i];
            seek_count += abs(left[i] - head);
            head = left[i];
        }

        seek_count += abs(0 - CYLINDER_LIMIT);  // Jump from start to end
        head = CYLINDER_LIMIT;

        for (int i = right_size - 1; i >= 0; i--) {
            seek_sequence[seek_sequence_size++] = right[i];
            seek_count += abs(right[i] - head);
            head = right[i];
        }
    }

    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY) == pdTRUE) {
        printf("\nTotal number of seek operations = %d\n", seek_count);
        printf("Seek Sequence: ");
        for (int i = 0; i < seek_sequence_size; i++) {
            printf("%d", seek_sequence[i]);
            if (i < seek_sequence_size - 1) printf(" -> ");
        }
        printf("\n");
        xSemaphoreGive(xSerialMutex);
    }

    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();
    
    // Wait for serial connection
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }

    printf("Initializing C-SCAN Disk Scheduling...\n");

    // Create mutex for serial output
    xSerialMutex = xSemaphoreCreateMutex();

    // Create C-SCAN task
    xTaskCreate(cscan_task,
                "C-SCAN Task",
                1024,  // Stack size
                NULL,
                1,     // Priority
                NULL);

    // Start scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (1);
}