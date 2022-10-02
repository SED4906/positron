#pragma once

void init_isrs();
extern void init_pit();
extern void isr_exc13_gp();
extern void isr_exc14_pf();
extern void isr_irq0_pit();
extern void isr_irq1_keyb();