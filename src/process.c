#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"

void initialize_Process(Process* process, char* name, pid_t pid, size_t start_time, size_t burst_duration, size_t bursts_remaining, size_t io_wait_duration, size_t deadline_time)
{
    process->name = malloc(strlen(name) + 1);
    strcpy(process->name, name);
    process->pid = pid;
    process->state = NOT_INITIATED;
    process->interruptions = 0;
    process->response_time = -1;
    process->waiting_time = 0;

    process->start_time = start_time;
    process->burst_duration = burst_duration;
    process->bursts_remaining = bursts_remaining;
    process->io_wait_duration = io_wait_duration;
    process->deadline_time = deadline_time;
    
    process->time_spent_on_quantum = 0;
    process->time_spent_on_burst = 0;
    process->time_spent_io_waiting = 0;
}


int compare_end_times(const void* a, const void* b)
{
    const Process* process1 = (const Process*)a;
    const Process* process2 = (const Process*)b;

    if (process1->turnaround_time < process2->turnaround_time) return -1;
    if (process1->turnaround_time > process2->turnaround_time) return 1;
    return 0;
}


void print_process_status(Process* process, FILE* output_file, const char* state_enum_strings[])
{
    // nombre_proceso_a,pid_a,estado_a,interrupciones_a,turnaround_a,response_a,waiting_a
    fprintf(output_file, "%s ", process->name);
    fprintf(output_file, "%d ", process->pid);
    fprintf(output_file, "%s ", state_enum_strings[process->state]);
    fprintf(output_file, "%zu ", process->interruptions);
    fprintf(output_file, "%zu ", process->turnaround_time);
    fprintf(output_file, "%zu ", process->response_time);
    fprintf(output_file, "%zu\n", process->waiting_time);

    printf("%s ", process->name);
    printf("%d ", process->pid);
    printf("%s ", state_enum_strings[process->state]);
    printf("%zu ", process->interruptions);
    printf("%zu ", process->turnaround_time);
    printf("%zu ", process->response_time + 1);
    printf("%zu\n", process->waiting_time);

}


void free_Process(Process* process)
{
    free(process->name);
}