#include <stdio.h>
#include <limits.h>
#include "srtf.h"
#include "utils.h"

// Shortest Remaining Time First scheduling
void SRTF(Process p[]) {
    printf("\n--- SRTF Scheduling ---\n");

    int time = 0, completed = 0;

    while (completed < N) {
        int idx = -1;
        int minRem = INT_MAX;

        // Pick process with the smallest remaining time
        for (int i = 0; i < N; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0 &&
                p[i].remaining < minRem) {
                minRem = p[i].remaining;
                idx = i;
            }
        }

        // If no process is ready, advance time
        if (idx == -1) {
            time++;
            continue;
        }

        printf("Time %d: Process %d running\n", time, p[idx].id);

        // Run process for 1 time unit
        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            completed++;
            p[idx].completion = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;

            printf("Time %d: Process %d completed\n", time, p[idx].id);
        }
    }

    printStats(p);
}