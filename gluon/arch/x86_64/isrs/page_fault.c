#include <quark/idt.h>
#include <quark/mm.h>
#include <quark/task.h>
#include <gluon/isrs.h>
#include <libc.h>

void incur_page_fault(size_t code, size_t address) {
    if(!(code & 1) && within_data(address)) {
        map_page(cr3,address &~ 0xFFF, alloc_page(), 7);
        return;
    }
    printf("#PF! <%d:%X>", code, address);
    die();
}