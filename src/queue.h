#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    int quantum;
 } Queue;


void initialize_Queue(Queue* queue, int quantum);