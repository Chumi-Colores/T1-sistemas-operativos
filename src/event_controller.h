#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"


typedef struct {
    Event* events;
    int number_of_events;
} EventController;


void initialize_EventController(EventController* event_controller, Event* events, int number_of_events);

int compare_events(const void* a, const void* b);

Event* get_event(EventController* event_controller, size_t time);