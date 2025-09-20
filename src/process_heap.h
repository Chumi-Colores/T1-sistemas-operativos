#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

typedef struct {
    Process** data;   // arreglo de punteros a Process
    size_t size;      // elementos actuales
    size_t capacity;  // capacidad del arreglo
    int current_tick;
} ProcessHeap;
