#include <stdio.h>
#include "rr.h"
#include "utils.h"

// Round Robin
void RoundRobin(Process p[]) {
    printf("\n--- Round Robin Scheduling ---\n");

    int time = 0, completed = 0;
    int queue[100], front = 0, rear = 0;
    int visited[N] = {0}; // tracks which processes are in queue

    // add processes that arrive at time 0
    for (int i = 0; i < N; i++) {
        if (p[i].arrival == 0) {
            queue[rear++] = i;
            visited[i] = 1;
        }
    }

    while (completed < N) {
        // if queue empty, advance time
        if (front == rear) {
            time++;
            // check for new arrivals
            for (int i = 0; i < N; i++) {
                if (!visited[i] && p[i].arrival <= time) {
                    queue[rear++] = i;
                    visited[i] = 1;
                }
            }
            continue;
        }

        int i = queue[front++]; // get next process

        // Check the remaining time and execute for quantum or remaining time
        int exec = (p[i].remaining > QUANTUM) ? QUANTUM : p[i].remaining;

        printf("Time %d: Process %d running\n", time, p[i].id);

        p[i].remaining -= exec;
        time += exec;

        // Add new arrivals to the queue
        for (int j = 0; j < N; j++) {
            if (!visited[j] && p[j].arrival <= time) {
                queue[rear++] = j;
                visited[j] = 1;
            }
        }

        if (p[i].remaining > 0) {
            // If not finished, add back to queue
            queue[rear++] = i;
        } else {
            completed++;
            p[i].completion = time;
            p[i].turnaround = time - p[i].arrival;
            p[i].waiting = p[i].turnaround - p[i].burst;

            printf("Time %d: Process %d completed\n", time, p[i].id);
        }
    }

    printStats(p);
}