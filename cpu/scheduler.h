#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef struct task {
    uint32_t esp;       // Stack Pointer
    uint32_t id;        // Process ID
    struct task *next;  // For the list
    
    // MLFQ Fields
    int priority;       // 0 (High), 1 (Med), 2 (Low)
    int ticks_left;     // How many ticks allowed before demotion
} task_t;

void init_scheduler();
void schedule();
void create_task(void (*entry_point)(), int id);
void exit_task();
void start_multitasking_test();

#endif
