#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process_heap.h"
#include "scheduler.h"
#include "event_controller.h"

void set_turnaround(Process* process, size_t current_tick)
{
    process->turnaround_time = current_tick - process->start_time;
}

int compare_processes(const void* a, const void* b)
{
    const Process* process1 = (const Process*)a;
    const Process* process2 = (const Process*)b;

    if (process1->pid < process2->pid) return -1;
    if (process1->pid > process2->pid) return 1;
    return 0;
}


void initialize_Scheduler(Scheduler* scheduler, Process* processes, int number_of_processes, int q)
{
    scheduler->processes = processes;
    qsort(processes, number_of_processes, sizeof(Process), compare_processes);
    scheduler->time_since_being_empty = 0;
    scheduler->process_count = number_of_processes;
    scheduler->pending_processes = number_of_processes;
    initialize_ProcessHeap(&scheduler->high_queue, 2*q, number_of_processes);
    initialize_ProcessHeap(&scheduler->low_queue, q, number_of_processes);
    scheduler->current_tick = 0;
    scheduler->running_process = NULL;
    scheduler->heap_that_running_process_came_from = NULL;
}

void update_waiting_processes(Scheduler* scheduler)
{
    // Actualizar los procesos que hayan terminado su tiempo de espera de I/O de WAITING a READY.
    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].state == WAITING && (scheduler->processes[i].time_spent_io_waiting >= scheduler->processes[i].io_wait_duration))
        {
            scheduler->processes[i].state = READY;
        }
    }
}

void update_expired_processes(Scheduler* scheduler)
{
    // Actualizar los procesos en las colas que hayan expirado su deadline y les queden bursts a DEAD.
    size_t tick = scheduler->current_tick;
    ProcessHeap* high_queue = &scheduler->high_queue;
    ProcessHeap* low_queue = &scheduler->low_queue;
    for (size_t i = 0; i < high_queue->size; i++)
    {
        // TODO: revisar esto
        if (high_queue->data[i]->deadline_time <= tick && high_queue->data[i]->bursts_remaining > 0)
        {
            printf("PROCESO MURIÓOOOOOOOOOOOO\n");
            high_queue->data[i]->state = DEAD;
            set_turnaround(high_queue->data[i], scheduler->current_tick); // TODO: EL ENUNCIADO NO HABLA DE ESTO
        }
    }

    for (size_t i = 0; i < low_queue->size; i++)
    {
        if (low_queue->data[i]->deadline_time <= tick && low_queue->data[i]->bursts_remaining > 0)
        {
            low_queue->data[i]->state = DEAD;
            printf("PROCESO MURIÓOOOOOOOOOOOO\n");
            set_turnaround(low_queue->data[i], scheduler->current_tick); // TODO: EL ENUNCIADO NO HABLA DE ESTO
        }
    }
}

void update_running_process(Scheduler* scheduler, Event* event)
{
    if (!scheduler->running_process)
    {
        return;
    }

    scheduler->running_process->time_spent_on_quantum ++;

    bool finished_cpu_burst = false;
    if (scheduler->running_process->time_spent_on_burst >= scheduler->running_process->burst_duration)
    {
        finished_cpu_burst = true;
        scheduler->running_process->bursts_remaining -= 1;
    }
    

    // 1) Alcanzó su deadline
    if (scheduler->current_tick == scheduler->running_process->deadline_time)
    {
        if (scheduler->running_process->bursts_remaining > 0)
        {
            scheduler->running_process->state = DEAD;
            set_turnaround(scheduler->running_process, scheduler->current_tick); // TODO: EL ENUNCIADO NO HABLA DE ESTO
        }
        else if (scheduler->running_process->bursts_remaining == 0)
        {
            printf("PROCESO TERMINÓOOOOOOOOOOOOO\n");
            scheduler->running_process->state = FINISHED;
            set_turnaround(scheduler->running_process, scheduler->current_tick);
        }
        scheduler->pending_processes -= 1;
        scheduler->running_process = NULL;
    }
    
    // 2) Terminó su CPU burst
    else if (finished_cpu_burst)
    {
        if (scheduler->running_process->bursts_remaining == 0)
        {
            printf("PROCESO TERMINÓOOOOOOOOOOOOO\n");
            scheduler->running_process->state = FINISHED;
            set_turnaround(scheduler->running_process, scheduler->current_tick);
            scheduler->pending_processes -= 1;
            scheduler->running_process = NULL;
            return;
        }
        scheduler->running_process->state = WAITING;
        scheduler->running_process->last_time_process_left_cpu = scheduler->current_tick;
        scheduler->running_process->time_spent_on_burst = 0;
        heap_insert(&scheduler->high_queue, scheduler->running_process); // Devolver a cola alta
        scheduler->running_process = NULL;
    }

    // 3) Su quantum se acabó
    else if (scheduler->running_process->time_spent_on_quantum >= scheduler->heap_that_running_process_came_from->quantum)
    {
        if (scheduler->running_process->bursts_remaining == 0)
        {
            printf("PROCESO TERMINÓOOOOOOOOOOOOO\n");
            scheduler->running_process->state = FINISHED;
            set_turnaround(scheduler->running_process, scheduler->current_tick);
            scheduler->pending_processes -= 1;
            scheduler->running_process = NULL;
            return;
        }
        scheduler->running_process->state = READY;
        scheduler->running_process->last_time_process_left_cpu = scheduler->current_tick;
        scheduler->running_process->time_spent_on_quantum = 0;
        heap_insert(&scheduler->low_queue, scheduler->running_process); // Devolver a cola baja
        scheduler->running_process = NULL;
    }

    // 4) Sacar proceso por evento
    else if (event && event->pid != scheduler->running_process->pid)
    {
        scheduler->running_process->state = KICKED;
        scheduler->running_process->last_time_process_left_cpu = scheduler->current_tick;
        scheduler->running_process->time_spent_on_quantum = 0; // NO ESTOY SEGURO SI SE RESETEA
        heap_insert(&scheduler->high_queue, scheduler->running_process); // Devolver a cola alta
        scheduler->running_process = NULL;
    }

    // 5) Continua ejecutando con normalidad
}

void update_queues(Scheduler* scheduler)
{
    // Ingresar los procesos a las colas según corresponda:
    // 1) Si un proceso salió de la CPU, ingresarlo a la cola que corresponda.
    // se hace al final del paso anterior por conveniencia.


    //  2) Para cada proceso, si el tiempo de inicio se cumple, ingresarlo a la cola High.
    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].start_time == scheduler->current_tick)
        {
            scheduler->processes[i].state = READY;
            heap_insert(&scheduler->high_queue, &scheduler->processes[i]);
        }
    }

    //  3) Para cada proceso de la cola Low, revisar si se cumple la condición para subir a la cola High y cambiarlos de cola según corresponda.
    for (int i = scheduler->low_queue.size - 1; i >= 0; i--)
    {
        if (2 * scheduler->low_queue.data[i]->deadline_time < scheduler->current_tick - scheduler->low_queue.data[i]->last_time_process_left_cpu)
        {
            heap_insert(&scheduler->high_queue, scheduler->low_queue.data[i]);
            Process* temp = scheduler->low_queue.data[i];
            scheduler->low_queue.data[i] = scheduler->low_queue.data[scheduler->low_queue.size];
            scheduler->low_queue.data[scheduler->low_queue.size] = temp;
            scheduler->low_queue.size -= 1;
        }
    }
}

void update_priorities(Scheduler* scheduler)
{
    update_queue_priorities(&scheduler->high_queue, scheduler->current_tick);
    update_queue_priorities(&scheduler->low_queue, scheduler->current_tick);

    for (int i = 0; i<scheduler->high_queue.size; i++)
    {
        printf("%s con prioridad %f\n", scheduler->high_queue.data[i]->name, get_process_priority(scheduler->high_queue.data[i], scheduler->current_tick));
    }
}


Process* get_process(Scheduler* scheduler, pid_t pid)
{
    int low = 0;
    int high = scheduler->process_count - 1;
    while (low <= high)
    {
        int index = (low + high) / 2;
        Process current_process = scheduler->processes[index];

        if (current_process.pid == pid)
        {
            return &scheduler->processes[index];
        }
        else if (current_process.pid > pid)
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


bool event_can_enter_cpu(Event* event, Scheduler* scheduler)
{
    if (!event)
    {
        return false;
    }
    if (!scheduler->running_process)
    {
        return true;
    }
    return event->pid != scheduler->running_process->pid;
}

void insert_new_process(Scheduler* scheduler, Event* event)
{
    // 1) Si se cumplió el tiempo de un evento, ingresar el proceso indicado.
    if (event_can_enter_cpu(event, scheduler))
    {
        Process* process = get_process(scheduler, event->pid);
        if (!process)
        {
            printf("ERROR ESTO NO DEBERÍA PASAR\n");
            return;
        }
        
        if (process->response_time == -1)
        {
            process->response_time = scheduler->current_tick;
        }
        process->state = RUNNING;
        scheduler->running_process = process;
        int queue = find_queue_from_process(&scheduler->high_queue, &scheduler->low_queue, process);
        if (queue == -1)
        {
            scheduler->heap_that_running_process_came_from = &scheduler->high_queue;
        }
        if (queue == 0){
            scheduler->heap_that_running_process_came_from = &scheduler->low_queue;
        }
        else {
            scheduler->heap_that_running_process_came_from = &scheduler->high_queue;
        }
    }
    // 2) Primer proceso en estado READY de la cola High.
    else if (!scheduler->running_process && scheduler->high_queue.size > 0 && scheduler->high_queue.data[0]->state == READY)
    {
        Process* process = heap_extract_max(&scheduler->high_queue);

        if (process->response_time == -1)
        {
            process->response_time = scheduler->current_tick;
        }
        process->state = RUNNING;
        scheduler->running_process = process;
        scheduler->heap_that_running_process_came_from = &scheduler->high_queue;
    }
    
    // 3) Primer proceso en estado READY de la cola Low
    else if (!scheduler->running_process && scheduler->low_queue.size > 0 && scheduler->low_queue.data[0]->state == READY)
    {
        Process* process = heap_extract_max(&scheduler->low_queue);

        if (process->response_time == -1)
        {
            process->response_time = scheduler->current_tick;
        }
        process->state = RUNNING;
        scheduler->running_process = process;
        scheduler->heap_that_running_process_came_from = &scheduler->low_queue;
    }
}

void update_waiting_times(Scheduler* scheduler){
    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].state == WAITING)
        {
            scheduler->processes[i].time_spent_io_waiting += 1;
        }
        else
        {
            scheduler->processes[i].time_spent_io_waiting = 0;
        }
    }
}

void update_ticks(Scheduler* scheduler){
    scheduler->current_tick += 1;
    size_t tick = scheduler->current_tick;

    scheduler->high_queue.current_tick = tick;
    scheduler->low_queue.current_tick = tick;

    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].state == READY || scheduler->processes[i].state == WAITING) 
        {
            scheduler->processes[i].waiting_time += 1;
        }
    }


    update_waiting_times(scheduler);
}


void print_results(Scheduler* scheduler, FILE* output_file)
{
    const char* enum_strings[] = {
        "NOT_INITIATED",
        "WAITING",
        "READY",
        "RUNNING",
        "FINISHED",
        "DEAD",
        "KICKED"
    };

    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        print_process_status(&scheduler->processes[i], output_file, enum_strings);
    }
    
}


void free_Scheduler(Scheduler* scheduler)
{
    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        free_Process(&scheduler->processes[i]);
    }

    free_heap(&scheduler->high_queue);
    free_heap(&scheduler->low_queue);
}