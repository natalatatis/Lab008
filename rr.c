#include <stdio.h>
#include <time.h>
#include "rr.h"
#include "utils.h"

void RoundRobin(Process p[], int n, int quantum) {
    printf("--- Round Robin Scheduling (Quantum %d) ---\n", quantum);

    time_t base = time(NULL);
    int timeNow = 0;
    int completed = 0;

    /*
       Queue circular/simple para guardar los indices de procesos listos.
       Usamos indices del arreglo p[], no ids, porque p[] ya contiene toda la informacion.
    */
    int queue[1000];
    int front = 0;
    int rear = 0;

    // added evita meter el mismo proceso dos veces a la cola cuando ya llego
    int added[MAX_THREADS] = {0};

    GanttBlock gantt[MAX_GANTT_BLOCKS];
    int ganttCount = 0;
    int firstStartedPrinted = 0;

    while (completed < n) {
        // Agregamos a la cola todos los procesos que ya llegaron al tiempo actual
        for (int i = 0; i < n; i++) {
            if (!added[i] && p[i].arrival <= timeNow) {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        /*
           Si la cola esta vacia, la CPU queda IDLE hasta la proxima llegada.
           Asi evitamos avanzar tiempo de uno en uno innecesariamente.
        */
        if (front == rear) {
            int nextArrival = 1000000;
            for (int i = 0; i < n; i++) {
                if (!added[i] && p[i].arrival < nextArrival) {
                    nextArrival = p[i].arrival;
                }
            }
            addGanttBlock(gantt, &ganttCount, -1, timeNow, nextArrival);
            timeNow = nextArrival;
            continue;
        }

        // Sacamos el siguiente proceso de la cola FIFO de Round Robin
        int idx = queue[front++];

        if (p[idx].start == -1) {
            p[idx].start = timeNow;
        }

        logProcessStart(base, timeNow, &p[idx], !firstStartedPrinted);
        firstStartedPrinted = 1;

        /*
           Ya que el proceso ejecuta como maximo quantum,
           si le queda menos que quantum, solo ejecuta lo que le falta.
        */
        int execTime;
        if (p[idx].remaining > quantum) {
            execTime = quantum;
        } else {
            execTime = p[idx].remaining;
        }

        addGanttBlock(gantt, &ganttCount, p[idx].id, timeNow, timeNow + execTime);

        simulateCpuBurst(execTime);
        p[idx].remaining -= execTime;
        timeNow += execTime;

        /*
           Despues de ejecutar, revisamos si llegaron procesos durante ese quantum,
           esos procesos se agregan antes de reinsertar el proceso actual.
        */
        for (int i = 0; i < n; i++) {
            if (!added[i] && p[i].arrival <= timeNow) {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        if (p[idx].remaining > 0) {
            // Si todavia le falta CPU, se preempta y vuelve al final de la cola
            logProcessPreempted(base, timeNow, &p[idx]);
            queue[rear++] = idx;
        } else {
            // Si termino, calculamos las metricas finales
            p[idx].completion = timeNow;
            p[idx].turnaround = p[idx].completion - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            completed++;

            logProcessEvent(base, timeNow, &p[idx], "Completed");
        }
    }

    printGanttChart(gantt, ganttCount);
    printStats(p, n);
}
