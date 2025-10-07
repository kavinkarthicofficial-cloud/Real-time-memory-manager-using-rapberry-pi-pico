#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
    int requests[] = {98, 183, 37, 122, 14, 124, 65, 67};
    int n = sizeof(requests) / sizeof(requests[0]);
    int head = 53;
    int completed[8] = {0};
    int total_seek_time = 0;

    printf("SSTF Seek Sequence: %d", head);

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
        printf(" -> %d", head);
    }

    printf("\nTotal Seek Time: %d\n", total_seek_time);
    printf("SSTF calculation complete!\n");

    return 0;
}
