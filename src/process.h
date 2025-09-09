#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    char* name;
    pid_t pid;
    size_t start_time;
    size_t burst_duration;
    size_t bursts_remaining;
    size_t io_wait_duration;
    size_t deadline_time;
} Process;


void initialize_Process(Process* process, char* name, pid_t pid, size_t start_time, size_t burst_duration, size_t bursts_remaining, size_t io_wait_duration, size_t deadline_time);

void free_Process(Process* process);