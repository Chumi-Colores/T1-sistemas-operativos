#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    NOT_INITIATED,
    WAITING,
    READY,
    RUNNING,
    FINISHED,
    DEAD,
    KICKED
} ProcessState;

typedef struct {
    char* name;
    pid_t pid;
    ProcessState state;
    size_t interruptions;
    size_t turnaround_time;
    size_t response_time;
    size_t waiting_time;

    size_t start_time;
    size_t burst_duration;
    size_t bursts_remaining;
    size_t io_wait_duration;
    size_t deadline_time;

    size_t time_spent_on_quantum;
    size_t time_spent_on_burst;
    size_t time_spent_io_waiting;
    size_t last_time_process_left_cpu;
    
} Process;


void initialize_Process(Process* process, char* name, pid_t pid, size_t start_time, size_t burst_duration, size_t bursts_remaining, size_t io_wait_duration, size_t deadline_time);

void free_Process(Process* process);