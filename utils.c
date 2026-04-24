#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "utils.h"

// Esta funcion devuelve un numero random dentro del rango [min, max]
int randomInRange(int min, int max) {
    return min + rand() % (max - min + 1);
}

/*
   Ordenamos por tiempo de llegada para que todos los algoritmos reciban
   los procesos en un orden base consistente. Si dos procesos llegan al
   mismo tiempo, usamos el id para desempatar.
*/
void sortByArrival(Process p[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrival > p[j + 1].arrival ||
                (p[j].arrival == p[j + 1].arrival && p[j].id > p[j + 1].id)) {
                Process tmp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = tmp;
            }
        }
    }
}

/*
   Esta parte genera el dataset random pedido por el lab
   - numero random de procesos (de 5-15)
   - burst random para cada proceso (de 1-10 s)
   - arrival random para cada proceso (de 0-100 s)
*/
Process *generateProcesses(int *n) {
    *n = randomInRange(MIN_THREADS, MAX_THREADS);
    Process *p = malloc(sizeof(Process) * (*n));

    if (!p) {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; i < *n; i++) {
        p[i].id = i;
        p[i].burst = randomInRange(MIN_BURST, MAX_BURST);
        p[i].remaining = p[i].burst;
        p[i].arrival = randomInRange(MIN_ARRIVAL, MAX_ARRIVAL);
        p[i].start = -1;
        p[i].completion = 0;
        p[i].waiting = 0;
        p[i].turnaround = 0;
        p[i].arrived_logged = 0;
        p[i].started_logged = 0;
    }

    sortByArrival(p, *n);
    return p;
}

/*
   Reiniciamos los procesos antes de ejecutar cada algoritmo.
   Esta parte es importante porque RR y SRTF modifican remaining, waiting, etc.
   Sin este reset, un algoritmo afectaria los resultados del siguiente.
*/
void resetProcesses(Process dest[], Process src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
        dest[i].remaining = dest[i].burst;
        dest[i].start = -1;
        dest[i].completion = 0;
        dest[i].waiting = 0;
        dest[i].turnaround = 0;
        dest[i].arrived_logged = 0;
        dest[i].started_logged = 0;
    }
}

// Imprime el dataset inicial para verificar que los 4 algoritmos usen los mismos datos
void printDataset(Process p[], int n) {
    printf("Dataset: %d threads\n", n);
    printf("Burst Times: [");
    for (int i = 0; i < n; i++) {
        printf("%d%s", p[i].burst, i == n - 1 ? "" : ", ");
    }

    printf("]\nArrival Times: [");
    for (int i = 0; i < n; i++) {
        printf("%d%s", p[i].arrival, i == n - 1 ? "" : ", ");
    }
    printf("]\n\n");
}

/*
   Convierte el tiempo virtual de la simulacion a una hora tipo timestamp.
   Por ejemplo, si base es la hora actual y virtual_time es 4, imprime base + 4 segundos.
*/
static void printTime(time_t base, int virtual_time) {
    time_t simulated = base + virtual_time;
    char buffer[64];
    struct tm *info = localtime(&simulated);

    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", info);
    printf("[%s] ", buffer);
}

// Funcion general para eventos simples como Completed
void logProcessEvent(time_t base, int virtual_time, Process *p, const char *message) {
    printTime(base, virtual_time);
    printf("Process %d (Burst %d): %s\n", p->id, p->burst, message);
}

/*
   Imprime el inicio del proceso con un formato similar al del PDF.
   - El primer proceso siempre imprime Arrived y Started en lineas separadas.
   - Los demas procesos imprimen Arrived at X, Started (waited Y seconds)
*/
void logProcessStart(time_t base, int virtual_time, Process *p, int is_first_process) {
    printTime(base, virtual_time);

    if (!p->arrived_logged && is_first_process) {
        printf("Process %d (Burst %d): Arrived\n", p->id, p->burst);
        p->arrived_logged = 1;

        printTime(base, virtual_time);
        printf("Process %d (Burst %d): Started\n", p->id, p->burst);
    } else if (!p->arrived_logged) {
        float waited = (float)(virtual_time - p->arrival);
        printf("Process %d (Burst %d): Arrived at %d, Started (waited %.2f seconds)\n",
               p->id, p->burst, p->arrival, waited);
        p->arrived_logged = 1;
    } else {
        printf("Process %d (Burst %d): Started\n", p->id, p->burst);
    }

    p->started_logged = 1;
}

// Imprime cuando un proceso es interrumpido, mostrando cuanto burst le queda
void logProcessPreempted(time_t base, int virtual_time, Process *p) {
    printTime(base, virtual_time);
    printf("Process %d (Burst %d remaining): Preempted\n", p->id, p->remaining);
}

/*
   Funcion especial para SRTF
   Cuando llega un proceso con menor remaining time, se imprime que ese proceso
   acaba de llegar y que interrumpio al proceso anterior.
*/
void logArrivedAndPreempted(time_t base, int virtual_time, Process *new_process, Process *old_process) {
    printTime(base, virtual_time);
    printf("Process %d (Burst %d): Arrived at %d, Preempted %d (remaining %d)\n",
           new_process->id,
           new_process->burst,
           new_process->arrival,
           old_process->id,
           old_process->remaining);
    new_process->arrived_logged = 1;
}

typedef struct {
    int duration;
} BurstArgs;

/*
   Este thread solo duerme por un tiempo corto para simular que la CPU esta trabajando.
   TIME_SCALE_MS evita que el programa tarde demasiado aunque el burst sea grande.
*/
static void *cpuThread(void *arg) {
    BurstArgs *data = (BurstArgs *)arg;
    struct timespec req;

    req.tv_sec = 0;
    req.tv_nsec = data->duration * TIME_SCALE_MS * 1000000L;
    nanosleep(&req, NULL);
    return NULL;
}


// Crea un thread para simular el CPU burst y espera a que termine con pthread_join
void simulateCpuBurst(int virtual_duration) {
    pthread_t thread;
    BurstArgs args;
    args.duration = virtual_duration;

    if (pthread_create(&thread, NULL, cpuThread, &args) != 0) {
        perror("pthread_create");
        exit(1);
    }

    pthread_join(thread, NULL);
}

/*
   Imprime los waiting times y turnaround times en el orden en que los procesos
   empezaron por primera vez. Asi el array coincide con el orden que el usuario
   ve en los logs :D
*/
void printStats(Process p[], int n) {
    float totalWaiting = 0.0f;
    float totalTurnaround = 0.0f;
    int order[MAX_THREADS];

    for (int i = 0; i < n; i++) {
        order[i] = i;
    }

    // Ordenamos indices por start time sin mover los procesos originales
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int a = order[j];
            int b = order[j + 1];

            if (p[a].start > p[b].start ||
                (p[a].start == p[b].start && p[a].arrival > p[b].arrival) ||
                (p[a].start == p[b].start && p[a].arrival == p[b].arrival && p[a].id > p[b].id)) {
                int tmp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }

    printf("Waiting Times: [");
    for (int i = 0; i < n; i++) {
        int idx = order[i];
        printf("%.2f%s", (float)p[idx].waiting, i == n - 1 ? "" : ", ");
        totalWaiting += p[idx].waiting;
    }
    printf("]\n");

    printf("Turnaround Times: [");
    for (int i = 0; i < n; i++) {
        int idx = order[i];
        printf("%.2f%s", (float)p[idx].turnaround, i == n - 1 ? "" : ", ");
        totalTurnaround += p[idx].turnaround;
    }
    printf("]\n");

    printf("Avg Waiting Time: %.2f seconds\n", totalWaiting / n);
    printf("Avg Turnaround Time: %.2f seconds\n\n", totalTurnaround / n);
}

/*
   Agrega un bloque al Gantt chart.
   Si el bloque nuevo es del mismo proceso que el anterior y empieza justo donde
   termino el anterior, los unimos para que el Gantt no se vuelva innecesariamente largo.
*/
void addGanttBlock(GanttBlock gantt[], int *count, int process_id, int start, int end) {
    if (start >= end) {
        return;
    }

    if (*count > 0 && gantt[*count - 1].process_id == process_id && gantt[*count - 1].end == start) {
        gantt[*count - 1].end = end;
        return;
    }

    if (*count >= MAX_GANTT_BLOCKS) {
        printf("Gantt chart is full. Some blocks were not saved.\n");
        return;
    }

    gantt[*count].process_id = process_id;
    gantt[*count].start = start;
    gantt[*count].end = end;
    (*count)++;
}

/*
   Imprime el Gantt chart usando ASCII: +, -, |, letras y numeros.
   No es proporcional a la duracion para que cada bloque tenga el mismo ancho y
   no se vuelva gigante cuando hay tiempos IDLE y demas muy largos.
*/
void printGanttChart(GanttBlock gantt[], int count) {
    const int width = 7;
    const int blocksPerLine = 8;

    if (count <= 0) {
        printf("\nGantt Chart: no CPU activity.\n\n");
        return;
    }

    printf("\nGantt Chart in ASCII:\n\n");

    // Dividimos el Gantt en varias filas para que especialmente RR y SRTF no se impriman en una linea enorme
    for (int startBlock = 0; startBlock < count; startBlock += blocksPerLine) {
        int endBlock = startBlock + blocksPerLine;
        if (endBlock > count) {
            endBlock = count;
        }

        for (int i = startBlock; i < endBlock; i++) {
            printf("+");
            for (int j = 0; j < width; j++) {
                printf("-");
            }
        }
        printf("+\n");

        for (int i = startBlock; i < endBlock; i++) {
            char label[20];
            if (gantt[i].process_id == -1) {
                snprintf(label, sizeof(label), "IDLE");
            } else {
                snprintf(label, sizeof(label), "P%d", gantt[i].process_id);
            }

            // Centramos el texto dentro del bloque
            int len = 0;
            while (label[len] != '\0') {
                len++;
            }

            int left = (width - len) / 2;
            int right = width - len - left;

            printf("|");
            for (int j = 0; j < left; j++) {
                printf(" ");
            }
            printf("%s", label);
            for (int j = 0; j < right; j++) {
                printf(" ");
            }
        }
        printf("|\n");

        for (int i = startBlock; i < endBlock; i++) {
            printf("+");
            for (int j = 0; j < width; j++) {
                printf("-");
            }
        }
        printf("+\n");

        printf("%-8d", gantt[startBlock].start);
        for (int i = startBlock; i < endBlock; i++) {
            printf("%-8d", gantt[i].end);
        }
        printf("\n\n");
    }
}
