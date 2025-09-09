#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "event.h"
#include "schedualer.h"
#include "event_controller.h"

void write_processes_from_file_to_array(FILE* file, Process* processes, int K)
{
    Process* p;
    char name[128];
    pid_t pid;
    size_t start_time;
    size_t burst_duration;
    size_t bursts_remaining;
    size_t io_wait_duration;
    size_t deadline_time;
    for (size_t i = 0; i < K; i++)
    {
        p = &processes[i];
        fscanf(file, "%127s %d %zu %zu %zu %zu %zu", name, &pid, &start_time, &burst_duration, &bursts_remaining, &io_wait_duration, &deadline_time);
        initialize_Process(p, name, pid, start_time, burst_duration, bursts_remaining, io_wait_duration, deadline_time);
    }
}


void write_events_from_file_to_array(FILE* file, Event* events, int N)
{
    Event* event;
    pid_t pid;
    size_t time;
    for (size_t i = 0; i < N; i++)
    {
        event = &events[i];
        fscanf(file, "%d %zu", &pid, &time);
        initialize_Event(event, pid, time);
    }
}


void run_simulation(EventController* event_controller, Schedualer* schedualer)
{
    size_t tick = 0;
    while ((event_controller->events_remaining > 0 || schedualer->active_processes_amount > 0))
    {
        execute_events(event_controller, schedualer, tick);
        update_waiting_processes(schedualer, tick);
        update_expired_processes(schedualer, tick);
        update_running_processes(schedualer, tick);
        update_queues(schedualer, tick);
        update_priorities(schedualer, tick);
        update_running_process(schedualer, tick);
        tick += 1;
    }
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Modo de uso: %s <input_file> <output_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *input_file = fopen(argv[1], "r");
	FILE *output_file = fopen(argv[2], "w");

	int q; // Número para calcular quantum
	fscanf(input_file, "%d", &q);

    int K; // Número de procesos
	fscanf(input_file, "%d", &K);

	int N; // Número de eventos
	fscanf(input_file, "%d", &N);


    Process processes[K];
    write_processes_from_file_to_array(input_file, processes, K);

    Schedualer schedualer;
    initialize_Schedualer(&schedualer, processes, K, q);
    
    Event events[N];
    write_events_from_file_to_array(input_file, events, N);

    EventController event_controller;
    initialize_EventController(&event_controller, events, N);

    run_simulation(&event_controller, &schedualer);

    free_Schedualer(&schedualer);

	fclose(input_file);
	fclose(output_file);

	return EXIT_SUCCESS;
}
