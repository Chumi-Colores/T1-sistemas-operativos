#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedualer.h"

void initialize_Schedualer(Schedualer* schedualer, Process* processes, int K, int q)
{
    schedualer->processes = processes;
    schedualer->process_count = K;
    schedualer->active_processes_amount = 0;
    initialize_Queue(&schedualer->high_queue, q+q);
    initialize_Queue(&schedualer->low_queue, q);
}

void update_waiting_processes(Schedualer* schedualer, size_t tick)
{
    // Actualizar los procesos que hayan terminado su tiempo de espera de I/O de WAITING a READY.
}

void update_expired_processes(Schedualer* schedualer, size_t tick)
{
    // Actualizar los procesos en las colas que hayan cumplido su deadline pero no completaron sus bursts a estado DEAD.
}

void update_running_processes(Schedualer* schedualer, size_t tick)
{

    // Si hay un proceso en estado RUNNING, actualizar su estado seg´un el siguiente orden:
    //  1) Alcanz´o su deadline
    //  2) Termnin´o su CPU burst
    //  3) Su quantum se acab´o
    //  4) Si ocurre un evento que involucra a un proceso distinto al que est´a en la CPU, entonces el proceso en ejecuci´on debe abandonar la CPU
    //  5) Continua ejecutando con normalidad
}

void update_queues(Schedualer* schedualer, size_t tick)
{
    // Ingresar los procesos a las colas seg´un corresponda:
    //  1) Si un proceso sali´o de la CPU, ingresarlo a la cola que corresponda.
    //  2) Para cada proceso, si el tiempo de inicio se cumple, ingresarlo a la cola High.
    //  3) Para cada proceso de la cola Low, revisar si se cumple la condici´on para subir a la cola High y cambiarlos de cola seg´un corresponda.
}

void update_priorities(Schedualer* schedualer, size_t tick)
{
    // Actualizar las prioridades de todos los procesos seg´un la f´ormula dada.
}

void update_running_process(Schedualer* schedualer, size_t tick)
{
    // Ingresar proceso a la CPU siguiendo este orden de prioridad:
    //  1) Si se cumpli´o el tiempo de un evento, ingresar el proceso indicado.
    //  2) Primer proceso en estado READY de la cola High.
    //  3) Primer proceso en estado READY de la cola Low.
}


void free_Schedualer(Schedualer* schedualer)
{
    for (size_t i = 0; i < schedualer->process_count; i++)
    {
        free_Process(&schedualer->processes[i]);
    }
}