#include <quark/task.h>
#include <libc.h>
#include <stdbool.h>
task* tasks;

context context_switch(size_t rsp, size_t cr3) {
    if(tasks->last_active) {
        tasks->c.rsp = rsp;
        tasks->c.cr3 = cr3;
        tasks=tasks->next;
    } else tasks->last_active = true;
    return tasks->c;
}

void save_register(size_t d, int w) {
    if(tasks->last_active) tasks->r[w] = d;
}

size_t load_register(int w) {
    return tasks->r[w];
}