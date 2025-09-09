#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"

typedef struct
{
    Event* events;
    int events_remaining;
} EventController;


void initialize_EventController(EventController* event_controller, Event* events, int K);

void execute_events(EventController* event_controller, Schedualer* Schedualer, size_t tick);