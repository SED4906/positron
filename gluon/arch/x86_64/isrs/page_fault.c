#include <quark/idt.h>
#include <gluon/isrs.h>
#include <libc.h>

void incur_page_fault(size_t code, size_t address) {
    printf("#PF! <%d:%X>", code, address);
    die();
}