#include <quark/idt.h>
#include <quark/mm.h>
#include <quark/task.h>
#include <gluon/isrs.h>
#include <libc.h>

void incur_page_fault(size_t code, size_t address) {
    printf("#PF! <%d:%X>", code, address);
    if(tasks->pid) {
        enable_interrupts();
        exit_task();
    }
    printf(" (fatal error)");
    die();
}