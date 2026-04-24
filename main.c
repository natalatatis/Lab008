#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "utils.h"
#include "fifo.h"
#include "rr.h"
#include "sjf.h"
#include "srtf.h"

int main(void) {
    int quantum;

    /*
       Esta parte inicializamos la semilla random.
       time(NULL) cambia con cada ejecucion, entonces rand() genera datasets diferentes
    */
    srand(time(NULL));

    printf("CPU Scheduling Simulation\n");
    printf("Enter quantum for Round Robin: ");
    scanf("%d", &quantum);

    // Verificamos que el quantum sea valido para evitar ciclos incorrectos
    if (quantum <= 0) {
        printf("Invalid quantum. Using quantum = 2.\n");
        quantum = 2;
    }

    int n = 0;

    // Generamos una sola lista original para que todos los algoritmos usen el mismo dataset
    Process *original = generateProcesses(&n);
    Process *work = malloc(sizeof(Process) * n);

    if (work == NULL) {
        printf("Memory allocation error.\n");
        free(original);
        return 1;
    }

    printf("\nThread range: %d-%d | Burst range: %d-%d | Arrival range: %d-%d\n",
           MIN_THREADS, MAX_THREADS, MIN_BURST, MAX_BURST, MIN_ARRIVAL, MAX_ARRIVAL);
    printf("Round Robin Quantum: %d\n\n", quantum);

    printDataset(original, n);

    /*
       Antes de cada algoritmo copiamos original a work,
       asi FIFO, RR, SJF y SRTF se comparan justamente con los mismos procesos.
    */
    resetProcesses(work, original, n);
    FIFO(work, n);

    resetProcesses(work, original, n);
    RoundRobin(work, n, quantum);

    resetProcesses(work, original, n);
    SJF(work, n);

    resetProcesses(work, original, n);
    SRTF(work, n);

    free(work);
    free(original);
    return 0;
}
