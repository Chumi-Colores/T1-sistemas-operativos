#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    pid_t pid;
    size_t time;
} Event;


void initialize_Event(Event* event, pid_t pid, size_t time);