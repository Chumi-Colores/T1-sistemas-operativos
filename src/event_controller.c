#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scheduler.h"
#include "event.h"
#include "event_controller.h"

void initialize_EventController(EventController* event_controller, Event* events, int number_of_events)
{
    event_controller->events = events;
    event_controller->events_remaining = number_of_events;
}

void execute_events(EventController* event_controller, Scheduler* scheduler, size_t tick)
{
    if (event_controller->events_remaining == 0) return;
}