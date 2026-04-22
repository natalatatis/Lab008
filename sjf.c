#include <stdio.h>
#include <limits.h>
#include "sjf.h"
#include "utils.h"

// Shortest Job First scheduling
void SJF(Process p[]) {
    printf("\n--- SJF Scheduling ---\n");

    int time = 0, completed = 0;
    int done[N] = {0};

    while (completed < N) {
        int idx = -1;
        int minBurst = INT_MAX;

        // Find shortest available process
        for (int i = 0; i < N; i++) {
            if (!done[i] && p[i].arrival <= time && p[i].burst < minBurst) {
                minBurst = p[i].burst;
                idx = i;
            }
        }

        // If no process is ready, advance time
        if (idx == -1) {
            time++;
            continue;
        }

        printf("Time %d: Process %d started\n", time, p[idx].id);

        p[idx].waiting = time - p[idx].arrival;

        time += p[idx].burst;

        p[idx].completion = time;
        p[idx].turnaround = time - p[idx].arrival;

        printf("Time %d: Process %d completed\n", time, p[idx].id);

        done[idx] = 1;
        completed++;
    }

    printStats(p);
}