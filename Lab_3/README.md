UID: 805796976
# You Spin Me Round Robin

Program simulating a Round Robin Scheduler, providing the average wait time and
average response time on output.

## Building

Run the following command to build the program executable:
```shell
# make
```

This will create in the current directory an object file named rr.o and the
program executable named rr.

## Running

Run the program with the following command:
```shell
# ./rr [FILENAME] [QUANTUM_LENGTH]
```
where `[FILENAME]` is the path to a file containing a processes file, and
`[QUANTUM_LENGTH]` is a valid 32-bit unsigned integer.

A process file describes the processes to be ran. It begins with an integer
number specifying how many processes to run, and each line thereafter contains
unsigned integers which describe the process id (PID), the process's arrival
time, and the process's burst (or running) time in order.

The syntax for the process file is:
```text
[NUMBER OF PROCESSES TO RUN]
[PROCESS 1 PID], [PROCESS 1 ARRIVAL TIME], [PROCESS 1 BURST TIME]
[PROCESS 2 PID], [PROCESS 2 ARRIVAL TIME], [PROCESS 2 BURST TIME]
```
and so on for as many processes as there are.

Below is an example process file with the following specifications:
- There are 4 processes to be ran
- PID 1 arrives at time 0 and has a burst time of 7
- PID 2 arrives at time 2 and has a burst time of 4
- PID 3 arrives at time 4 and has a burst time of 1
- PID 4 arrives at time 5 and has a burst time of 4

```text
4
1, 0, 7
2, 2, 4
3, 4, 1
4, 5, 4
```

The result of running the program with the example process file above and a
quantum length of 3 is:
```shell
# ./rr processes.txt 3
Average waiting time: 7
Average response time: 2.75
```

## Cleaning up

To clean up the environment, run the following:
```shell
# make clean
```

This will remove the program executable and any associated relevant files.
