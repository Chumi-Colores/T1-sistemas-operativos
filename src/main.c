#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "event.h"
#include "scheduler.h"
#include "event_controller.h"

void write_processes_from_file_to_array(FILE* file, Process* processes, int number_of_processes)
{
    Process* p;
    char name[128];
    pid_t pid;
    size_t start_time;
    size_t burst_duration;
    size_t bursts_remaining;
    size_t io_wait_duration;
    size_t deadline_time;
    for (size_t i = 0; i < number_of_processes; i++)
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


void run_simulation(EventController* event_controller, Scheduler* scheduler)
{
    printf("Running simulation...\n");
    while (scheduler->not_done_processes > 0)
    {
        Event* event = get_event(event_controller, scheduler->current_tick);
        update_waiting_processes(scheduler);
        update_expired_processes(scheduler);
        update_running_process(scheduler, event);
        update_queues(scheduler);
        update_priorities(scheduler);
        insert_new_process(scheduler, event);
        
        printf("Tick terminado %zu\n", scheduler->current_tick);
        update_ticks(scheduler);
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

    int number_of_processes; // Número de procesos
	fscanf(input_file, "%d", &number_of_processes);

	int N; // Número de eventos
	fscanf(input_file, "%d", &N);


    Process processes[number_of_processes];
    write_processes_from_file_to_array(input_file, processes, number_of_processes);

    Scheduler scheduler;
    initialize_Scheduler(&scheduler, processes, number_of_processes, q);
    
    Event events[N];
    write_events_from_file_to_array(input_file, events, N);

    EventController event_controller;
    initialize_EventController(&event_controller, events, N);

    run_simulation(&event_controller, &scheduler);

    free_Scheduler(&scheduler);

	fclose(input_file);
	fclose(output_file);

	return EXIT_SUCCESS;
}

