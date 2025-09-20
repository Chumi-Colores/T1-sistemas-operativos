#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "event.h"
#include "queue.h"
typedef struct
{
    Process* processes;
    int process_count;
    Queue high_queue;
    Queue low_queue;
    int active_processes_amount;
} Scheduler;


void initialize_Scheduler(Scheduler* scheduler, Process* processes, int number_of_processes, int q);

void update_waiting_processes(Scheduler* scheduler, size_t tick);

void update_expired_processes(Scheduler* scheduler, size_t tick);

void update_running_processes(Scheduler* scheduler, size_t tick);

void update_queues(Scheduler* scheduler, size_t tick);

void update_priorities(Scheduler* scheduler, size_t tick);

void update_running_process(Scheduler* scheduler, size_t tick);

void update_ticks(Scheduler* scheduler, size_t tick);

void free_Scheduler(Scheduler* scheduler);