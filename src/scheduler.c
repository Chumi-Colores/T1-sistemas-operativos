#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scheduler.h"

void initialize_Scheduler(Scheduler* scheduler, Process* processes, int number_of_processes, int q)
{
    scheduler->processes = processes;
    scheduler->process_count = number_of_processes;
    scheduler->active_processes_amount = 0;
    initialize_Queue(&scheduler->high_queue, q+q, number_of_processes);
    initialize_Queue(&scheduler->low_queue, q, number_of_processes);
}

void update_waiting_processes(Scheduler* scheduler, int tick)
{
    // Actualizar los procesos que hayan terminado su tiempo de espera de I/O de WAITING a READY.
}

void update_expired_processes(Scheduler* scheduler, int tick)
{
    // Actualizar los procesos en las colas que hayan cumplido su deadline pero no completaron sus bursts a estado DEAD.
}

void update_running_processes(Scheduler* scheduler, int tick)
{

    // Si hay un proceso en estado RUNNING, actualizar su estado seg´un el siguiente orden:
    //  1) Alcanzó su deadline
    //  2) Termninó su CPU burst
    //  3) Su quantum se acabó
    //  4) Si ocurre un evento que involucra a un proceso distinto al que est´a en la CPU, entonces el proceso en ejecuci´on debe abandonar la CPU
    //  5) Continua ejecutando con normalidad
}

void update_queues(Scheduler* scheduler, int tick)
{
    // Ingresar los procesos a las colas seg´un corresponda:
    //  1) Si un proceso sali´o de la CPU, ingresarlo a la cola que corresponda.
    //  2) Para cada proceso, si el tiempo de inicio se cumple, ingresarlo a la cola High.
    //  3) Para cada proceso de la cola Low, revisar si se cumple la condici´on para subir a la cola High y cambiarlos de cola seg´un corresponda.
}

void update_priorities(Scheduler* scheduler, int tick)
{
    // Actualizar las prioridades de todos los procesos seg´un la f´ormula dada.
}

void update_running_process(Scheduler* scheduler, int tick)
{
    // Ingresar proceso a la CPU siguiendo este orden de prioridad:
    //  1) Si se cumpli´o el tiempo de un evento, ingresar el proceso indicado.
    //  2) Primer proceso en estado READY de la cola High.
    //  3) Primer proceso en estado READY de la cola Low.
}

void update_ticks_in_queues(Scheduler* scheduler, int tick){
    scheduler->high_queue.processes->current_tick = tick;
    scheduler->low_queue.processes->current_tick = tick;
}


void free_Scheduler(Scheduler* scheduler)
{
    for (size_t i = 0; i < scheduler->process_count; i++)
    {
        free_Process(&scheduler->processes[i]);
    }
}