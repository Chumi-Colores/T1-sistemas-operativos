#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "event.h"
#include "process_heap.h"

typedef struct
{
    Process* processes;
    int process_count;
    ProcessHeap high_queue;
    ProcessHeap low_queue;
    int active_processes_amount;
    size_t current_tick;
} Scheduler;


void initialize_Scheduler(Scheduler* scheduler, Process* processes, int number_of_processes, int q);

void update_waiting_processes(Scheduler* scheduler);

void update_expired_processes(Scheduler* scheduler);

void update_running_processes(Scheduler* scheduler);

void update_queues(Scheduler* scheduler);

void update_priorities(Scheduler* scheduler);

void update_running_process(Scheduler* scheduler);

void update_ticks(Scheduler* scheduler);

void free_Scheduler(Scheduler* scheduler);