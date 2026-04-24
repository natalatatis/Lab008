#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include "process.h"

#define MIN_THREADS 5
#define MAX_THREADS 15
#define MIN_BURST 1
#define MAX_BURST 10
#define MIN_ARRIVAL 0
#define MAX_ARRIVAL 100
#define TIME_SCALE_MS 1
#define MAX_GANTT_BLOCKS 1000

int randomInRange(int min, int max);
Process *generateProcesses(int *n);
void sortByArrival(Process p[], int n);
void resetProcesses(Process dest[], Process src[], int n);
void printDataset(Process p[], int n);
void printStats(Process p[], int n);
void logProcessEvent(time_t base, int virtual_time, Process *p, const char *message);
void logProcessStart(time_t base, int virtual_time, Process *p, int is_first_process);
void logProcessPreempted(time_t base, int virtual_time, Process *p);
void logArrivedAndPreempted(time_t base, int virtual_time, Process *new_process, Process *old_process);
void simulateCpuBurst(int virtual_duration);
void addGanttBlock(GanttBlock gantt[], int *count, int process_id, int start, int end);
void printGanttChart(GanttBlock gantt[], int count);

#endif
