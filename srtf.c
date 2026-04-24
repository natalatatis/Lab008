#include <stdio.h>
#include <limits.h>
#include <time.h>
#include "srtf.h"
#include "utils.h"

void SRTF(Process p[], int n) {
    printf("--- SRTF Scheduling ---\n");

    time_t base = time(NULL);
    int timeNow = 0;
    int completed = 0;
    int previous = -1;
    GanttBlock gantt[MAX_GANTT_BLOCKS];
    int ganttCount = 0;
    int firstStartedPrinted = 0;

    while (completed < n) {
        int idx = -1;
        int shortestRemaining = INT_MAX;

        /*
           Revisa en cada unidad de tiempo cual proceso disponible tiene
           el menor tiempo restante. Por eso aca si es preemptive
        */
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= timeNow && p[i].remaining > 0 && p[i].remaining < shortestRemaining) {
                shortestRemaining = p[i].remaining;
                idx = i;
            }
        }

        // Si no hay ningun proceso listo, la CPU esta IDLE por una unidad
        if (idx == -1) {
            addGanttBlock(gantt, &ganttCount, -1, timeNow, timeNow + 1);
            timeNow++;
            continue;
        }

        /*
           Si idx cambio, significa que empieza otro proceso.
           Esto solo puede pasar si el anterior termino o porque fue preempted.
        */
        if (previous != idx) {
            if (previous != -1 && p[previous].remaining > 0) {
                /*
                   Si el nuevo proceso aun no habia sido anunciado, imprimimos el formato:
                   Arrived at X, Preempted Y (remaining Z)
                */
                if (!p[idx].arrived_logged) {
                    logArrivedAndPreempted(base, timeNow, &p[idx], &p[previous]);
                } else {
                    logProcessPreempted(base, timeNow, &p[previous]);
                }
            }

            if (p[idx].start == -1) {
                p[idx].start = timeNow;
            }

            logProcessStart(base, timeNow, &p[idx], !firstStartedPrinted);
            firstStartedPrinted = 1;
            previous = idx;
        }

        // Ejecutamos una unidad para poder revisar si llega algo mas corto
        addGanttBlock(gantt, &ganttCount, p[idx].id, timeNow, timeNow + 1);
        simulateCpuBurst(1);
        p[idx].remaining--;
        timeNow++;

        if (p[idx].remaining == 0) {
            p[idx].completion = timeNow;
            p[idx].turnaround = p[idx].completion - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            completed++;

            logProcessEvent(base, timeNow, &p[idx], "Completed");

            // Reiniciamos previous para que el siguiente proceso imprima Started
            previous = -1;
        }
    }

    printGanttChart(gantt, ganttCount);
    printStats(p, n);
}
