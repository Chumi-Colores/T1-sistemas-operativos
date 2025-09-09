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
} Schedualer;


void initialize_Schedualer(Schedualer* schedualer, Process* processes, int K, int q);

void update_waiting_processes(Schedualer* schedualer, size_t tick);

void update_expired_processes(Schedualer* schedualer, size_t tick);

void update_running_processes(Schedualer* schedualer, size_t tick);

void update_queues(Schedualer* schedualer, size_t tick);

void update_priorities(Schedualer* schedualer, size_t tick);

void update_running_process(Schedualer* schedualer, size_t tick);

void free_Schedualer(Schedualer* schedualer);