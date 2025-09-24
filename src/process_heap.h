#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

typedef struct {
    Process** data;   // arreglo de punteros a Process
    int quantum;
    size_t size;      // elementos actuales
    size_t capacity;  // capacidad del arreglo
    size_t current_tick;
} ProcessHeap;

void initialize_ProcessHeap(ProcessHeap* heap, int quantum, int capacity);

Process* heap_extract_max(ProcessHeap* heap);

void heap_insert(ProcessHeap* heap, Process* proc);

void update_queue_priorities(ProcessHeap* heap, size_t current_tick);


void free_heap(ProcessHeap* heap);