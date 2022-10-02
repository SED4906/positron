#include <gluon/isrs.h>
#include <quark/idt.h>

void init_isrs() {
    set_idt_entry(13, (void*)isr_exc13_gp, 0x8E);
    set_idt_entry(14, (void*)isr_exc14_pf, 0x8E);
    set_idt_entry(32, (void*)isr_irq0_pit, 0x8F);
    set_idt_entry(33, (void*)isr_irq1_keyb, 0x8E);
    init_pit();
    pic_clear_mask(0);
    pic_clear_mask(1);
}