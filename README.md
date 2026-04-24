# CPU Scheduling Simulation

##  Description
This lab simulates different CPU scheduling algorithms and compares their performance using randomly generated datasets of processes (threads).  

Each process has:
- Burst Time
- Arrival Time

The program executes multiple scheduling strategies and reports:
- Execution logs
- Gantt charts (ASCII)
- Waiting times
- Turnaround times
- Averages for comparison

---

##  Implemented Algorithms

### 1. FIFO (First In First Out)
- Processes are executed in order of arrival.
- Non-preemptive.
- Simple but can lead to long waiting times.

---

### 2. Round Robin (RR)
- Preemptive scheduling.
- Each process gets a fixed time slice (quantum).
- If not finished, it is placed back in the queue.


---

### 3. SJF (Shortest Job First)
- Selects the process with the smallest burst time.
- Non-preemptive.
- Optimal average waiting time (in theory).

---

### 4. SRTF (Shortest Remaining Time First)
- Preemptive version of SJF.
- Always runs the process with the shortest remaining time.

---


##  How to Compile and Run

### Compile
```bash
make all
```

### Run
```bash
./scheduler
```

# Implementation of Labs 1 and 2 on the Beaglebone Black

## Lab 1
This lab's only job is to print "Hello World" on the Beagle. 

### Clean
```bash
make clean
```

### Compile and run
```bash
make beagle LAB=1
```

---

## Lab 2
Lab 2 is a basic calculator that adds whole numbers and floats. The main idea behind it was organizing the files in layers.

### OS Layer
- Acts as the bridge between the hardware and the software. It handles the low-level initialization, timer, and UART configuration.

## Library Layer
- Provides a custom implementation of PRINT and READ that do not rely on C libraries. It handles string manipulation, numeric conversions and gives the functions that will be used 
by the user program.

### Program Layer
- This is the high-level code that implements the calculator. It uses the Library functions so that it doesn't directly interact with the hardware.

### Clean
```bash
make clean
```

### Compile and run
```bash
make beagle LAB=2
```
