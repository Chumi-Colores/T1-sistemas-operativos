#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "process_heap.h"

ProcessHeap* create_heap(size_t capacity) {
    ProcessHeap* heap = malloc(sizeof(ProcessHeap));
    heap->data = malloc(sizeof(Process*) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    heap->current_tick = 0;
    return heap;
}

void swap(Process** process_1_pointer, Process** process_2_pointer) {
    Process* temporal_pointer = *process_1_pointer;
    *process_1_pointer = *process_2_pointer;
    *process_2_pointer = temporal_pointer;
}

void heapify_up(ProcessHeap* heap, size_t idx) {
    if (idx == 0) return;
    size_t parent = (idx - 1) / 2;

    if (compare_process(heap->data[idx], heap->data[parent], heap->current_tick) > 0) {
        swap(&heap->data[idx], &heap->data[parent]);
        heapify_up(heap, parent);
    }
}

double get_process_priority(Process* process, size_t current_tick)
{
    size_t time_until_deadline = process->deadline_time - current_tick;
    double priority = (1.0/time_until_deadline) + process->bursts_remaining;
    return priority;
}

void heap_insert(ProcessHeap* heap, Process* proc) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->data = realloc(heap->data, sizeof(Process*) * heap->capacity);
    }
    heap->data[heap->size] = proc;
    heapify_up(heap, heap->size);
    heap->size++;
}

void heapify_down(ProcessHeap* heap, size_t idx) {
    size_t left = 2 * idx + 1;
    size_t right = 2 * idx + 2;
    size_t largest = idx;

    if (left < heap->size &&
        compare_process(heap->data[left], heap->data[largest], heap->current_tick) > 0) {
        largest = left;
    }
    if (right < heap->size &&
        compare_process(heap->data[right], heap->data[largest], heap->current_tick) > 0) {
        largest = right;
    }

    if (largest != idx) {
        swap(&heap->data[idx], &heap->data[largest]);
        heapify_down(heap, largest);
    }
}

int compare_process(const Process* process_1_pointer, const Process* process_2_pointer, size_t current_tick) {
    double process_1_priority = get_process_priority((Process*)process_1_pointer, current_tick);
    double process_2_priority = get_process_priority((Process*)process_2_pointer, current_tick);

    if (process_1_priority > process_2_priority) return 1;
    if (process_1_priority < process_2_priority) return -1;

    // Empate de prioridad, menor pid primero
    if (process_1_pointer->pid < process_2_pointer->pid) return 1;
    if (process_1_pointer->pid > process_2_pointer->pid) return -1;

    return 0; // son iguales, no debería pasar
}


Process* heap_extract_max(ProcessHeap* heap) {
    if (heap->size == 0) return NULL;

    Process* max = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    heapify_down(heap, 0);

    return max;
}

