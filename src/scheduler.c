#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process_heap.h"
#include "scheduler.h"
#include "event_controller.h"


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
    scheduler->process_count = number_of_processes;
    scheduler->not_done_processes = number_of_processes;
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
        if (high_queue->data[i]->deadline_time <= tick && high_queue->data[i]->bursts_remaining > 0)
        {
            high_queue->data[i]->state = DEAD;
        }
    }

    for (size_t i = 0; i < low_queue->size; i++)
    {
        if (low_queue->data[i]->deadline_time <= tick && low_queue->data[i]->bursts_remaining > 0)
        {
            low_queue->data[i]->state = DEAD;
        }
    }
}

void update_running_process(Scheduler* scheduler, Event* event)
{
    if (!scheduler->running_process)
    {
        return;
    }

    scheduler->running_process->time_spent_on_burst += 1;
    scheduler->running_process->time_spent_on_quantum += 1;

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
        }
        else if (scheduler->running_process->bursts_remaining == 0)
        {
            scheduler->running_process->state = FINISHED;
        }
        scheduler->not_done_processes -= 1;
        scheduler->running_process = NULL;
    }
    
    // 2) Terminó su CPU burst
    else if (finished_cpu_burst)
    {
        scheduler->running_process->state = WAITING;
        scheduler->running_process->last_time_process_left_cpu = scheduler->current_tick;
        scheduler->running_process->time_spent_on_burst = 0;
        heap_insert(scheduler->heap_that_running_process_came_from, scheduler->running_process); // Devolver a cola que vino
        scheduler->running_process = NULL;
    }

    // 3) Su quantum se acabó
    else if (scheduler->running_process->time_spent_on_quantum >= scheduler->heap_that_running_process_came_from->quantum)
    {
        scheduler->running_process->state = READY;
        scheduler->running_process->last_time_process_left_cpu = scheduler->current_tick;
        scheduler->running_process->time_spent_on_quantum = 0;
        scheduler->running_process->time_spent_on_burst = 0; // NO ESTOY SEGURO SI SE RESETEA
        heap_insert(&scheduler->low_queue, scheduler->running_process); // Devolver a cola baja
        scheduler->running_process = NULL;
    }

    // 4) Sacar proceso por evento
    else if (event && event->pid != scheduler->running_process->pid)
    {
        scheduler->running_process->state = KICKED;
        scheduler->running_process->last_time_process_left_cpu = scheduler->current_tick;
        scheduler->running_process->time_spent_on_burst = 0; // NO ESTOY SEGURO SI SE RESETEA
        scheduler->running_process->time_spent_on_quantum = 0; // NO ESTOY SEGURO SI SE RESETEA
        heap_insert(&scheduler->high_queue, scheduler->running_process); // Devolver a cola alta
        scheduler->running_process = NULL;
    }

    // 5) Continua ejecutando con normalidad
}

void update_queues(Scheduler* scheduler)
{
    // Ingresar los procesos a las colas seg´un corresponda:
    //  1) Si un proceso sali´o de la CPU, ingresarlo a la cola que corresponda.
    // se hace al final del paso anterior por conveniencia.


    //  2) Para cada proceso, si el tiempo de inicio se cumple, ingresarlo a la cola High.
    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].start_time == scheduler->current_tick)
        {
            heap_insert(&scheduler->high_queue, &scheduler->processes[i]);
        }
    }
    
    //  3) Para cada proceso de la cola Low, revisar si se cumple la condici´on para subir a la cola High y cambiarlos de cola seg´un corresponda.
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

void insert_new_process(Scheduler* scheduler, Event* event)
{
    // 1) Si se cumpli´o el tiempo de un evento, ingresar el proceso indicado.
    if (event && event->pid != scheduler->running_process->pid)
    {
        Process* process = get_process(scheduler, event->pid);
        if (!process)
        {
            printf("Process of pid: %d not found\n", event->pid);
            return;
        }
        
        if (process->state == NOT_INITIATED)
        {
            process->state = RUNNING;
            process->response_time = scheduler->current_tick;
            scheduler->running_process = process;
            scheduler->heap_that_running_process_came_from = &scheduler->high_queue;
        }
    }
    // 2) Primer proceso en estado READY de la cola High.
    else if (scheduler->high_queue.size > 0 && scheduler->high_queue.data[0]->state == READY)
    {
        Process* process = heap_extract_max(&scheduler->high_queue);
        if (process->response_time == -1)
        {
            process->response_time = scheduler->current_tick;
        }
        scheduler->running_process = process;
        scheduler->heap_that_running_process_came_from = &scheduler->high_queue;
    }
    
    // 3) Primer proceso en estado READY de la cola Low
    else if (scheduler->low_queue.size > 0 && scheduler->low_queue.data[0]->state == READY)
    {
        Process* process = heap_extract_max(&scheduler->low_queue);
        if (process->response_time == -1)
        {
            process->response_time = scheduler->current_tick;
        }
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

    update_waiting_times(scheduler);
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