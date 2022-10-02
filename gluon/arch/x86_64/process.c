#include <gluon/process.h>
#include <quark/idt.h>
#include <common/mm.h>
#include <libc.h>
int next_pid=0;

void init_process() {
    tasks = (task*)alloc(sizeof(task));
    tasks->next = tasks;
    tasks->prev = tasks;
    tasks->pid = next_pid++;
    tasks->uid = 0;
    tasks->last_active = true;
}

void exit_task() {
    task* task_exiting = tasks;
    tasks->next->prev = tasks->prev;
    tasks->prev->next = tasks->next;
    tasks = tasks->next;
    tasks->last_active = false;
    dealloc(task_exiting);
    idle();
    printf("What do I do?\n");
    for(;;) die();
}