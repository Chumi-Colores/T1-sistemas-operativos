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
    qsort(events, number_of_events, sizeof(Event), compare_events);
    event_controller->number_of_events = number_of_events;
}

int compare_events(const void* a, const void* b)
{
    const Event* event1 = (const Event*)a;
    const Event* event2 = (const Event*)b;

    if (event1->time < event2->time) return -1;
    if (event1->time > event2->time) return 1;
    return 0;
}


Event* get_event(EventController* event_controller, size_t time)
{
    int low = 0;
    int high = event_controller->number_of_events - 1;
    while (low <= high)
    {
        int index = (low + high) / 2;
        Event current_event = event_controller->events[index];

        if (current_event.time == time)
        {
            return &event_controller->events[index];
        }
        else if (current_event.time > time)
        {
            high = index - 1;
        }
        else
        {
            low = index + 1;
        }
    }

    return NULL; // no encontrado
}
