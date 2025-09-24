#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"

void initialize_Process(Process* process, char* name, pid_t pid, size_t start_time, size_t burst_duration, size_t bursts_remaining, size_t io_wait_duration, size_t deadline_time)
{
    process->name = malloc(strlen(name) + 1);
    strcpy(process->name, name);
    process->pid = pid;
    process->state = READY;
    process->interruptions = 0;
    process->response_time = -1;

    process->start_time = start_time;
    process->burst_duration = burst_duration;
    process->bursts_remaining = bursts_remaining;
    process->io_wait_duration = io_wait_duration;
    process->deadline_time = deadline_time;
    
    process->time_spent_on_quantum = 0;
    process->time_spent_on_burst = 0;
    process->time_spent_io_waiting = 0;
}

void free_Process(Process* process)
{
    free(process->name);
}