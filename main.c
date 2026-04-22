#include "process.h"
#include "utils.h"
#include "fifo.h"
#include "rr.h"
#include "sjf.h"
#include "srtf.h"

int main() {
    Process original[N] = {
        {0, 4, 4, 0},
        {1, 2, 2, 1},
        {2, 6, 6, 2},
        {3, 3, 3, 3},
        {4, 1, 1, 4},
        {5, 5, 5, 5}
    };

    Process p[N];

    resetProcesses(p, original);
    FIFO(p);

    resetProcesses(p, original);
    RoundRobin(p);

    resetProcesses(p, original);
    SJF(p);

    resetProcesses(p, original);
    SRTF(p);

    return 0;
}