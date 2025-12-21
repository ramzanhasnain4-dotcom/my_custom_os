#include "scheduler.h"
#include "idt.h"

#define STACK_SIZE 1024
#define MAX_TASKS 10

// --- TUNING SECTION ---
// I increased these numbers so the tasks stay Green/Yellow longer.
// 50Hz timer = 50 ticks per second.
#define TICKS_PRIO_0 100   // High Priority: 2 Seconds (was 5)
#define TICKS_PRIO_1 300   // Medium Priority: 6 Seconds (was 15)
#define TICKS_PRIO_2 600   // Low Priority: 12 Seconds (was 30)

uint8_t stack_pool[MAX_TASKS][STACK_SIZE];
task_t task_pool[MAX_TASKS];
int task_used[MAX_TASKS];
task_t *current_task = 0;

extern void switch_task(uint32_t *old_esp, uint32_t new_esp);

// ----------------------------------------------------------------------------
// VISUALIZATION HELPER
// ----------------------------------------------------------------------------
void print_status(int id, int priority, char status) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    int offset = 160 + (id * 6);
    
    uint16_t color = 0x0F00; 
    if (priority == 0) color = 0x2F00; // Green
    if (priority == 1) color = 0xEF00; // Yellow
    if (priority == 2) color = 0x4F00; // Red
    if (status == 'X') color = 0x7F00; // Grey (Dead)

    vga[offset]   = (uint16_t)('T') | color;
    vga[offset+1] = (uint16_t)('0' + id) | color;
    vga[offset+2] = (uint16_t)status | color;
}

// ----------------------------------------------------------------------------
// OPERATION 1: TASK CREATION
// ----------------------------------------------------------------------------
void init_task_stack(task_t *task, uint8_t *stack_mem, void (*entry_point)()) {
    uint32_t *stack_ptr = (uint32_t*)(stack_mem + STACK_SIZE);
    stack_ptr--; *stack_ptr = (uint32_t)entry_point;
    stack_ptr--; *stack_ptr = 0; // EBP
    stack_ptr--; *stack_ptr = 0; // EBX
    stack_ptr--; *stack_ptr = 0; // ESI
    stack_ptr--; *stack_ptr = 0; // EDI
    stack_ptr--; *stack_ptr = 0x202; // EFLAGS
    task->esp = (uint32_t)stack_ptr;
}

void create_task(void (*entry_point)(), int id) {
    int slot = -1;
    for(int i=0; i<MAX_TASKS; i++) {
        if(task_used[i] == 0) { slot = i; break; }
    }
    if(slot == -1) return;

    task_used[slot] = 1;
    task_pool[slot].id = id;
    task_pool[slot].priority = 0; 
    task_pool[slot].ticks_left = TICKS_PRIO_0;
    
    init_task_stack(&task_pool[slot], stack_pool[slot], entry_point);

    if(current_task == 0) {
        current_task = &task_pool[slot];
        current_task->next = current_task;
    } else {
        task_t *next = current_task->next;
        current_task->next = &task_pool[slot];
        task_pool[slot].next = next;
    }
}

// ----------------------------------------------------------------------------
// OPERATION 2: TASK TERMINATION
// ----------------------------------------------------------------------------
void exit_task() {
    if(current_task->next == current_task) {
        print_status(current_task->id, 0, 'X');
        while(1); 
    }

    task_t *prev = current_task;
    while(prev->next != current_task) {
        prev = prev->next;
    }

    print_status(current_task->id, 0, 'X');

    prev->next = current_task->next;

    for(int i=0; i<MAX_TASKS; i++) {
        if(&task_pool[i] == current_task) {
            task_used[i] = 0;
            break;
        }
    }

    task_t *dead_task = current_task;
    current_task = prev->next;
    
    switch_task(&dead_task->esp, current_task->esp);
}

// ----------------------------------------------------------------------------
// OPERATION 3: CONTEXT SWITCHING (MLFQ SCHEDULER)
// ----------------------------------------------------------------------------
void schedule() {
    if(!current_task) return;

    current_task->ticks_left--;
    if(current_task->ticks_left <= 0) {
        if(current_task->priority < 2) current_task->priority++; 
        
        if(current_task->priority == 0) current_task->ticks_left = TICKS_PRIO_0;
        else if(current_task->priority == 1) current_task->ticks_left = TICKS_PRIO_1;
        else current_task->ticks_left = TICKS_PRIO_2;
    }

    task_t *best_task = 0;
    task_t *scanner = current_task->next;
    int best_prio = 3;

    for(int i=0; i<MAX_TASKS; i++) {
        if(scanner->priority < best_prio) {
            best_prio = scanner->priority;
            best_task = scanner;
        }
        scanner = scanner->next;
        if(scanner == current_task->next) break;
    }

    task_t *old_task = current_task;
    current_task = best_task;

    print_status(current_task->id, current_task->priority, 'R');

    switch_task(&old_task->esp, current_task->esp);
}

void init_scheduler() {
    for(int i=0; i<MAX_TASKS; i++) task_used[i] = 0;
    current_task = 0;
}

// ----------------------------------------------------------------------------
// TEST CASES
// ----------------------------------------------------------------------------

void task_suicide_bomb() {
    // INCREASED LOOP: 
    // This will now take much longer to finish, so you can see T2 alive longer.
    // We loop 20 times instead of 5.
    for(int i=0; i<20; i++) {
        // Spin for a while 
        for(volatile int k=0; k<10000000; k++); 
    }
    
    exit_task();
}

void task_eternal() {
    while(1) {
        for(volatile int k=0; k<100000; k++); 
    }
}

void start_multitasking_test() {
    create_task(task_eternal, 1);
    create_task(task_suicide_bomb, 2);
    create_task(task_eternal, 3);
    
    uint32_t dummy;
    switch_task(&dummy, current_task->esp);
    while(1);
}
