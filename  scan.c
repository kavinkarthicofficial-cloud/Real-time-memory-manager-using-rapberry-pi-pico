#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

SemaphoreHandle_t xSerialMutex;
TaskHandle_t xSCANTaskHandle;

#define MAX_REQUESTS 20

int requests[MAX_REQUESTS];
int request_count = 0;
int head = 0;
int disk_size = 200;
int direction = 1;

void scan_task(void *pvParameters) {
    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.print("SCAN Seek Sequence: ");
        Serial.print(head);

        int sorted_requests[MAX_REQUESTS + 2];
        sorted_requests[0] = 0;
        sorted_requests[request_count + 1] = disk_size - 1;

        for (int i = 0; i < request_count; i++)
            sorted_requests[i + 1] = requests[i];

        // Sort the array
        for (int i = 0; i < request_count + 2; i++) {
            for (int j = i + 1; j < request_count + 2; j++) {
                if (sorted_requests[i] > sorted_requests[j]) {
                    int temp = sorted_requests[i];
                    sorted_requests[i] = sorted_requests[j];
                    sorted_requests[j] = temp;
                }
            }
        }

        int index = 0;
        while (index < request_count + 2 && sorted_requests[index] < head)
            index++;

        int total_seek_time = 0;

        if (direction == 1) { // Right
            for (int i = index; i < request_count + 2; i++) {
                Serial.print(" -> ");
                Serial.print(sorted_requests[i]);
                total_seek_time += abs(head - sorted_requests[i]);
                head = sorted_requests[i];
            }
            for (int i = index - 1; i >= 0; i--) {
                Serial.print(" -> ");
                Serial.print(sorted_requests[i]);
                total_seek_time += abs(head - sorted_requests[i]);
                head = sorted_requests[i];
            }
        } else { // Left
            for (int i = index - 1; i >= 0; i--) {
                Serial.print(" -> ");
                Serial.print(sorted_requests[i]);
                total_seek_time += abs(head - sorted_requests[i]);
                head = sorted_requests[i];
            }
            for (int i = index; i < request_count + 2; i++) {
                Serial.print(" -> ");
                Serial.print(sorted_requests[i]);
                total_seek_time += abs(head - sorted_requests[i]);
                head = sorted_requests[i];
            }
        }

        Serial.print("\nTotal Seek Time: ");
        Serial.println(total_seek_time);

        xSemaphoreGive(xSerialMutex);
        Serial.println("SCAN calculation complete!");
    }

    vTaskDelete(NULL);
}

void read_input() {
    Serial.println("\nEnter disk requests (comma-separated, e.g., 98,183,37,122,14): ");
    while (Serial.available() == 0) {
        delay(10);
    }
    String input = Serial.readStringUntil('\n');
    input.trim();

    
    char input_buffer[100];
    input.toCharArray(input_buffer, sizeof(input_buffer));

    request_count = 0;
    char *token = strtok(input_buffer, ",");
    while (token != NULL && request_count < MAX_REQUESTS) {
        requests[request_count++] = atoi(token);
        token = strtok(NULL, ",");
    }

    Serial.println("\nEnter head position: ");
    while (Serial.available() == 0) {
        delay(10);
    }
    head = Serial.parseInt();
    while (Serial.read() != '\n'); 

    Serial.println("\nEnter direction (1 for right, 0 for left): ");
    while (Serial.available() == 0) {
        delay(10);
    }
    direction = Serial.parseInt();
    while (Serial.read() != '\n'); 

    
    Serial.print("Parsed direction: ");
    Serial.println(direction);
    Serial.print("Parsed head: ");
    Serial.println(head);
    Serial.print("Parsed requests: ");
    for (int i = 0; i < request_count; i++) {
        Serial.print(requests[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Initializing SCAN simulation...");

    read_input(); 

    xSerialMutex = xSemaphoreCreateMutex();

    xTaskCreate(scan_task, 
                "SCAN Task", 
                2048,         
                NULL, 
                1, 
                &xSCANTaskHandle);

    vTaskStartScheduler();
}

void loop() {
    // Not used when using FreeRTOS scheduler
}