# Scheduler
This program implements a process sheduler, making the transitions between the states of a process (waiting, running, ready and terminated) using to batch and one interactive methods: First Come First Served, Shortest Job First and Round Robin.

- First Come First Served (FCFS)
Processes get in the RUNNING state in the same order they got in the READY state. They remain RUNNING until their execution finishes and they are marked as TERMINATED.

- Shortest Job First (SJF)
When switching from READY to RUNNING, the proceess with the shortest time of execution will have priority

- Round Robin (RR)
Processes act like in the FCFS implementation, but if a RUNNING process has an execution time that is too long and prevents the other processes running on the CPU, it is interrupted and marked as READY.

