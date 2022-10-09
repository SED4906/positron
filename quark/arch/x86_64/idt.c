#include <quark/idt.h>
#include <libc.h>

idt_gate idt[256];
idt_ptr idtr = {.base = 0, .limit = 0};

void set_idt_entry(uint8_t vector, void* isr, uint8_t flags) {
    idt_gate* gate = &idt[vector];

    gate->isr_low = (uint64_t)isr & 0xFFFF;
    gate->kernel_cs = 0x08;
    gate->ist = 0;
    gate->attributes = flags;
    gate->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
    gate->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    gate->reserved = 0;
}

extern void* isr_stub_table[48];
extern void init_pic(char m, char s);
void init_idt() {

    for (uint8_t vector = 0; vector < 48; vector++) {
        set_idt_entry(vector, isr_stub_table[vector], 0x8E);
    }

    idtr.base = (uint64_t)&idt;
    idtr.limit = 0xFFFF;
    
    init_pic(32, 40);
    load_idt(&idtr);
}