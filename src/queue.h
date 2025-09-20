#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process_heap.h"


typedef struct
{
    int quantum;
    ProcessHeap* processes;
 } Queue;


void initialize_Queue(Queue* queue, int quantum, int heap_capacity);
