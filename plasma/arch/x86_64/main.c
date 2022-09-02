#include <common/gdt.h>
#include <common/idt.h>
#include <common/isrs.h>
#include <common/mm.h>
#include <common/process.h>
#include <common/task.h>
#include <libc.h>

void _start() {
    init_mm();
    printf("Memory management.\n");
    init_gdt();
    printf("Global descriptor table.\n");
    init_idt();
    printf("Interrupt descriptor table.\n");

    init_process();
    printf("Process management.\n");
    init_isrs();
    printf("Interrupt service routines.\n");
    enable_interrupts();
    printf("Interrupts enabled.");

    for(;;) idle();
}