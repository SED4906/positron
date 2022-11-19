#include <quark/idt.h>
#include <quark/task.h>
#include <gluon/isrs.h>
#include <libc.h>

void gp_fault(size_t code) {
    printf("#GP! <%X>", code);
    if(tasks->pid) {
        enable_interrupts();
        exit_task();
    }
    printf(" (fatal error)");
    die();
}