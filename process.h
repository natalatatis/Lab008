// Struct that all will share
#ifndef PROCESS_H
#define PROCESS_H

// Info about each process
typedef struct{
    int id;
    int burst; // time needed to complete
    int remaining; // remaining time for RR and srtf
    int arrival; // time when process arrives
    int completion; // time when process completes
    int waiting; // time spent waiting
    int turnaround; // time from arrival to completion
} Process;

#endif