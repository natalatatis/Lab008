#include <stdio.h>
#include "fifo.h"
#include "utils.h"

// First In First Out scheduling
void FIFO(Process p[]) {
    printf("\n--- FIFO Scheduling ---\n");

    int time = 0;

    for (int i = 0; i < N; i++) {
        // if cpu is idle, jump to arrival time
        if (time < p[i].arrival)
            time = p[i].arrival;

        printf("Time %d: Process %d started\n", time, p[i].id);

        // waiting time is current time - arrival time
        p[i].waiting = time - p[i].arrival;

        // Run process completely
        time += p[i].burst;

        // Completion and turnaround times
        p[i].completion = time;
        p[i].turnaround = time - p[i].arrival;

        printf("Time %d: Process %d completed\n", time, p[i].id);
    }

    printStats(p);
}