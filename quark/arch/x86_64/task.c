#include <common/task.h>

task* tasks;

context context_switch(size_t rsp, size_t cr3) {
    tasks->c.rsp = rsp;
    tasks->c.cr3 = cr3;
    tasks=tasks->next;
    return tasks->c;
}

void save_register(size_t d, int w) {
    if(tasks->active) tasks->r[w] = d;
}

size_t load_register(int w) {
    return tasks->r[w];
}

void init_task(task* first) {
    tasks = first;
}