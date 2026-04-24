#include <stdio.h>
#include <time.h>
#include "fifo.h"
#include "utils.h"

void FIFO(Process p[], int n) {
    printf("--- FIFO Scheduling ---\n");

    time_t base = time(NULL);
    int timeNow = 0;
    GanttBlock gantt[MAX_GANTT_BLOCKS];
    int ganttCount = 0;
    int firstStartedPrinted = 0;

    for (int i = 0; i < n; i++) {
        /*
           Verificamos si la CPU debe quedarse IDLE antes de que llegue el siguiente proceso.
           Si timeNow es menor que arrival, no hay proceso listo para ejecutar
        */
        if (timeNow < p[i].arrival) {
            addGanttBlock(gantt, &ganttCount, -1, timeNow, p[i].arrival);
            timeNow = p[i].arrival;
        }

        // Cada proceso empieza cuando le toca y corre completo sin preempts
        p[i].start = timeNow;
        p[i].waiting = p[i].start - p[i].arrival;

        logProcessStart(base, timeNow, &p[i], !firstStartedPrinted);
        firstStartedPrinted = 1;

        // Guardamos el bloque de ejecucion para el Gantt chart
        addGanttBlock(gantt, &ganttCount, p[i].id, timeNow, timeNow + p[i].burst);

        simulateCpuBurst(p[i].burst);
        timeNow += p[i].burst;

        p[i].completion = timeNow;
        p[i].turnaround = p[i].completion - p[i].arrival;

        logProcessEvent(base, timeNow, &p[i], "Completed");
    }

    printGanttChart(gantt, ganttCount);
    printStats(p, n);
}
