#include <stdio.h>
#include "utils.h"

// Copies original data into array and resets fields
void resetProcesses(Process p[], Process original[]) {
    for (int i = 0; i < N; i++) {
        p[i] = original[i];
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        p[i].waiting = 0;
        p[i].turnaround = 0;
    }
}


// Prints results for each alg
void printStats(Process p[]) {
    float totalW = 0, totalT = 0;

    printf("Waiting Times: ");
    for (int i = 0; i < N; i++) {
        printf("%d ", p[i].waiting);
        totalW += p[i].waiting;
    }

    printf("\nTurnaround Times: ");
    for (int i = 0; i < N; i++) {
        printf("%d ", p[i].turnaround);
        totalT += p[i].turnaround;
    }

    printf("\nAvg Waiting Time: %.2f\n", totalW / N);
    printf("Avg Turnaround Time: %.2f\n\n", totalT / N);
}