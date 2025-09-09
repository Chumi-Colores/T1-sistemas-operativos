#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

void initialize_Queue(Queue* queue, int quantum)
{
    queue->quantum = quantum;
}