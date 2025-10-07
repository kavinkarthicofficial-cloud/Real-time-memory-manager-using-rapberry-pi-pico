#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>


SemaphoreHandle_t xSerialMutex;
TaskHandle_t xSSTFTaskHandle;

void sstf_task(void *pvParameters) {
    int requests[] = {98, 183, 37, 122, 14, 124, 65, 67};
    const int n = sizeof(requests) / sizeof(requests[0]);
    int head = 53;
    
    
    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.print("SSTF Seek Sequence: ");
        Serial.print(head);
        
        int total_seek_time = 0;
        int completed[n] = {0};
        
        for (int i = 0; i < n; i++) {
            int min_seek = INT_MAX, min_index = -1;
            
            for (int j = 0; j < n; j++) {
                if (!completed[j]) {
                    int seek_time = abs(head - requests[j]);
                    if (seek_time < min_seek) {
                        min_seek = seek_time;
                        min_index = j;
                    }
                }
            }
            
            completed[min_index] = 1;
            head = requests[min_index];
            total_seek_time += min_seek;
            
            Serial.print(" -> ");
            Serial.print(head);
        }
        
        Serial.print("\nTotal Seek Time: ");
        Serial.println(total_seek_time);
        
        
        xSemaphoreGive(xSerialMutex);
        
        Serial.println("SSTF calculation complete!");
    }
    
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    while (!Serial); 
    
    Serial.println("Initializing SSTF simulation...");

    
    xSerialMutex = xSemaphoreCreateMutex();
    
    
    xTaskCreate(sstf_task, 
               "SSTF Task", 
               1024, 
               NULL, 
               1, 
               &xSSTFTaskHandle);
    
    
    vTaskStartScheduler();
}

void loop() {
    
}