#include <stdio.h>
#include <limits.h>
#include <time.h>
#include "sjf.h"
#include "utils.h"

void SJF(Process p[], int n) {
    printf("--- SJF Scheduling ---\n");

    time_t base = time(NULL);
    int timeNow = 0;
    int completed = 0;
    int done[MAX_THREADS] = {0};
    GanttBlock gantt[MAX_GANTT_BLOCKS];
    int ganttCount = 0;
    int firstStartedPrinted = 0;

    while (completed < n) {
        int idx = -1;
        int shortestBurst = INT_MAX;

        /*
           Buscamos entre los procesos que ya llegaron y todavia no terminaron.
           Se elige el burst mas corto pero no se hace preempt una vez que empieza.
        */
        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival <= timeNow && p[i].burst < shortestBurst) {
                shortestBurst = p[i].burst;
                idx = i;
            }
        }

        // Si no hay procesos disponibles, avanzamos hasta la siguiente llegada
        if (idx == -1) {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!done[i] && p[i].arrival < nextArrival) {
                    nextArrival = p[i].arrival;
                }
            }
            addGanttBlock(gantt, &ganttCount, -1, timeNow, nextArrival);
            timeNow = nextArrival;
            continue;
        }

        p[idx].start = timeNow;
        p[idx].waiting = p[idx].start - p[idx].arrival;

        logProcessStart(base, timeNow, &p[idx], !firstStartedPrinted);
        firstStartedPrinted = 1;

        addGanttBlock(gantt, &ganttCount, p[idx].id, timeNow, timeNow + p[idx].burst);

        simulateCpuBurst(p[idx].burst);
        timeNow += p[idx].burst;

        p[idx].completion = timeNow;
        p[idx].turnaround = p[idx].completion - p[idx].arrival;
        done[idx] = 1;
        completed++;

        logProcessEvent(base, timeNow, &p[idx], "Completed");
    }

    printGanttChart(gantt, ganttCount);
    printStats(p, n);
}
