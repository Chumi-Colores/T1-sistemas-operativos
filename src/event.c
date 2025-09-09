#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"

void initialize_Event(Event* event, pid_t pid, size_t time)
{
    event->pid = pid;
    event->time = time;
}