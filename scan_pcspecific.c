#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REQUESTS 20

int requests[MAX_REQUESTS];
int request_count = 0;
int head = 0;
int disk_size = 200;
int direction = 1;

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    char input_buffer[256];
    printf("Enter disk requests (comma-separated, e.g., 98,183,37,122,14):\n");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    char *token = strtok(input_buffer, ", \n");
    while (token != NULL && request_count < MAX_REQUESTS) {
        requests[request_count++] = atoi(token);
        token = strtok(NULL, ", \n");
    }

    printf("Enter head position: ");
    scanf("%d", &head);
    printf("Enter direction (1 for right, 0 for left): ");
    scanf("%d", &direction);

    int sorted_requests[MAX_REQUESTS + 2];
    sorted_requests[0] = 0;
    sorted_requests[request_count + 1] = disk_size - 1;
    for (int i = 0; i < request_count; i++)
        sorted_requests[i + 1] = requests[i];
    int total_count = request_count + 2;

    qsort(sorted_requests, total_count, sizeof(int), compare);

    int index = 0;
    while (index < total_count && sorted_requests[index] < head)
        index++;

    int total_seek_time = 0;
    printf("SCAN Seek Sequence: %d", head);

    if (direction == 1) {
        for (int i = index; i < total_count; i++) {
            printf(" -> %d", sorted_requests[i]);
            total_seek_time += abs(head - sorted_requests[i]);
            head = sorted_requests[i];
        }
        for (int i = index - 1; i >= 0; i--) {
            printf(" -> %d", sorted_requests[i]);
            total_seek_time += abs(head - sorted_requests[i]);
            head = sorted_requests[i];
        }
    } else {
        for (int i = index - 1; i >= 0; i--) {
            printf(" -> %d", sorted_requests[i]);
            total_seek_time += abs(head - sorted_requests[i]);
            head = sorted_requests[i];
        }
        for (int i = index; i < total_count; i++) {
            printf(" -> %d", sorted_requests[i]);
            total_seek_time += abs(head - sorted_requests[i]);
            head = sorted_requests[i];
        }
    }

    printf("\nTotal Seek Time: %d\n", total_seek_time);
    printf("SCAN calculation complete!\n");

    return 0;
}
