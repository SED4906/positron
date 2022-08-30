#include <common/process.h>
#include <common/mm.h>

process* processes;

void init_process() {
    processes = (process*)alloc(sizeof(process));
    processes->next = 0;
    processes->pid = 0;
    processes->t.active = 1;
    processes->t.next = &processes->t;
    init_task();
}