#include <common/isrs.h>
#include <common/idt.h>

void init_isrs() {
    set_idt_entry(14, (void*)isr_exc14_pf, 0x8E);
    set_idt_entry(32, (void*)isr_irq0_pit, 0x8F);
    init_pit();
    pic_clear_mask(0);
}