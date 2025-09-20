#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "process_heap.h"

void initialize_Queue(Queue* queue, int quantum, int heap_capacity)
{
    queue->quantum = quantum;
    queue->processes = create_heap(heap_capacity);
}

