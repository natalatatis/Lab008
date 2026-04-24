#ifndef PROCESS_H
#define PROCESS_H

/*
   Esta estructura representa el proceso/thread de nuestra simulacion.
   Guardamos sus datos iniciales y valores que se
   calculan durante cada algoritmo de scheduling.
*/
typedef struct {
    int id;             // Identificador del proceso, por ejemplo P0, P1, P2
    int burst;          // Tiempo total de CPU que necesita el proceso
    int remaining;      // Tiempo que falta por ejecutar (lo usamos en RR y SRTF)
    int arrival;        // Tiempo en el que el proceso llega a la ready queue
    int start;          // Primera vez que el proceso obtiene CPU
    int completion;     // Tiempo en el que el proceso termina
    int waiting;        // Tiempo total que el proceso paso esperando
    int turnaround;     // completion - arrival

    // Estas banderas evitan imprimir varias veces el mismo evento de llegada/inicio
    int arrived_logged;
    int started_logged;
} Process;

/*
   Cada bloque representa una parte del Gantt chart.
   process_id = -1 significa que la CPU estuvo IDLE entre start y end.
*/
typedef struct {
    int process_id;
    int start;
    int end;
} GanttBlock;

#endif
