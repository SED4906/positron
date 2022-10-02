#include <quark/idt.h>
#include <gluon/isrs.h>
#include <libc.h>

void gp_fault(size_t code) {
    printf("#GP! <%X>", code);
    die();
}