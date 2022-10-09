#include <quark/idt.h>
#include <quark/mm.h>
#include <quark/task.h>
#include <libc.h>
#include <stdbool.h>
task* tasks;
int next_pid=0;

void init_task() {
    tasks = (task*)alloc(sizeof(task));
    tasks->next = tasks;
    tasks->prev = tasks;
    tasks->pid = next_pid++;
    tasks->uid = 0;
    tasks->last_active = true;
    tasks->data_s = 0;
    tasks->data_e = 0;
}

void exit_task() {
    __asm__ volatile("cli");
    task* task_exiting = tasks;
    deallocate_data_segment(task_exiting);
    // Remove task from list.
    tasks->next->prev = tasks->prev;
    tasks->prev->next = tasks->next;
    tasks = tasks->next;
    // Prepare for context switch.
    tasks->last_active = false;
    dealloc(task_exiting);
    __asm__ volatile("sti");
    idle();
    // This shouldn't be reached.
    printf("What do I do?\n");
    for(;;) die();
}

context context_switch(size_t rsp, size_t cr3) {
    // If the task before us did not just exit
    if(tasks->last_active) {
        tasks->c.rsp = rsp;
        tasks->c.cr3 = cr3;
        tasks=tasks->next;
        cr3 = tasks->c.cr3;
    } else tasks->last_active = true; // If it did
    return tasks->c;
}

void save_register(size_t d, int w) {
    if(tasks->last_active) tasks->r[w] = d;
}

size_t load_register(int w) {
    return tasks->r[w];
}

bool within_data(size_t address) {
    return address >= tasks->data_s && address < tasks->data_e;
}

void deallocate_data_segment(task* task) {
    for(size_t address = task->data_s; address < task->data_e; address += 4096) {
        size_t paddr = get_virtual_mapping(task->c.cr3, address);
        if(paddr & 1) dealloc_page(paddr &~ 0xFFF);
    }
}