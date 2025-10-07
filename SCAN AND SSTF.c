#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Function to calculate total seek time using SCAN
void scan_disk_scheduling(int requests[], int n, int head, int disk_size, int direction) {
    int seek_count = 0, distance, cur_track;
    int left[20], right[20], left_size = 0, right_size = 0;

    // Splitting requests into left and right based on the head position
    for (int i = 0; i < n; i++) {
        if (requests[i] < head)
            left[left_size++] = requests[i];
        else
            right[right_size++] = requests[i];
    }

    // Sorting left and right arrays
    for (int i = 0; i < left_size - 1; i++) {
        for (int j = 0; j < left_size - i - 1; j++) {
            if (left[j] > left[j + 1]) {
                int temp = left[j];
                left[j] = left[j + 1];
                left[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < right_size - 1; i++) {
        for (int j = 0; j < right_size - i - 1; j++) {
            if (right[j] > right[j + 1]) {
                int temp = right[j];
                right[j] = right[j + 1];
                right[j + 1] = temp;
            }
        }
    }

    // Moving in the specified direction
    printf("\nSCAN Disk Scheduling Order: ");
    if (direction == 1) { // Moving towards higher values
        for (int i = 0; i < right_size; i++) {
            cur_track = right[i];
            printf("%d ", cur_track);
            seek_count += abs(cur_track - head);
            head = cur_track;
        }
        // Move to the highest track (disk size - 1)
        seek_count += abs(disk_size - 1 - head);
        head = disk_size - 1;
        for (int i = left_size - 1; i >= 0; i--) {
            cur_track = left[i];
            printf("%d ", cur_track);
            seek_count += abs(cur_track - head);
            head = cur_track;
        }
    } else { // Moving towards lower values
        for (int i = left_size - 1; i >= 0; i--) {
            cur_track = left[i];
            printf("%d ", cur_track);
            seek_count += abs(cur_track - head);
            head = cur_track;
        }
        // Move to 0
        seek_count += abs(head - 0);
        head = 0;
        for (int i = 0; i < right_size; i++) {
            cur_track = right[i];
            printf("%d ", cur_track);
            seek_count += abs(cur_track - head);
            head = cur_track;
        }
    }

    printf("\nTotal Seek Operations: %d\n", seek_count);
}

// Function to calculate total seek time using SSTF
void sstf_disk_scheduling(int requests[], int n, int head) {
    int seek_count = 0;
    int served[n], seek_sequence[n];

    for (int i = 0; i < n; i++)
        served[i] = 0;

    printf("\nSSTF Disk Scheduling Order: ");
    
    for (int i = 0; i < n; i++) {
        int min_distance = INT_MAX, index = -1;
        
        // Find the closest request
        for (int j = 0; j < n; j++) {
            if (!served[j] && abs(requests[j] - head) < min_distance) {
                min_distance = abs(requests[j] - head);
                index = j;
            }
        }
        
        served[index] = 1;
        seek_count += min_distance;
        head = requests[index];
        seek_sequence[i] = head;
    }

    for (int i = 0; i < n; i++)
        printf("%d ", seek_sequence[i]);

    printf("\nTotal Seek Operations: %d\n", seek_count);
}

// Driver Code
int main() {
    int n, head, disk_size, direction;
    
    printf("Enter number of requests: ");
    scanf("%d", &n);
    int requests[n];

    printf("Enter request sequence: ");
    for (int i = 0; i < n; i++)
        scanf("%d", &requests[i]);

    printf("Enter initial head position: ");
    scanf("%d", &head);

    printf("Enter disk size: ");
    scanf("%d", &disk_size);

    printf("Enter SCAN direction (1 for high, 0 for low): ");
    scanf("%d", &direction);

    // Call SCAN algorithm
    scan_disk_scheduling(requests, n, head, disk_size, direction);

    // Call SSTF algorithm
    sstf_disk_scheduling(requests, n, head);

    return 0;
}