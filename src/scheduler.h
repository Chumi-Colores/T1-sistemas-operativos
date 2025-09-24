#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "event.h"
#include "process_heap.h"
#include "event_controller.h"

typedef struct {
    Process* processes;
    int process_count;
    int not_done_processes;
    ProcessHeap high_queue;
    ProcessHeap low_queue;
    size_t current_tick;
    Process* running_process;
    ProcessHeap* heap_that_running_process_came_from;
} Scheduler;

int compare_processes(const void* a, const void* b);

void initialize_Scheduler(Scheduler* scheduler, Process* processes, int number_of_processes, int q);

void update_waiting_processes(Scheduler* scheduler);

void update_expired_processes(Scheduler* scheduler);

void update_running_process(Scheduler* scheduler, Event* event);

void update_queues(Scheduler* scheduler);

void update_priorities(Scheduler* scheduler);

void insert_new_process(Scheduler* scheduler, Event* event);

void update_ticks(Scheduler* scheduler);

void free_Scheduler(Scheduler* scheduler);