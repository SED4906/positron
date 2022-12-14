#include <quark/gdt.h>
#include <quark/idt.h>
#include <gluon/isrs.h>
#include <quark/mm.h>
#include <quark/task.h>
#include <libc.h>

void _start() {
    init_mm();
    printf("Memory management.\n");
    init_gdt();
    printf("Global descriptor table.\n");
    init_idt();
    printf("Interrupt descriptor table.\n");

    init_task();
    printf("Process management.\n");
    init_isrs();
    printf("Interrupt service routines.\n");
    enable_interrupts();
    printf("Interrupts enabled.");

    for(;;) idle();
}