#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed)) idt_gate;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr;

extern void set_idt_entry(uint8_t vector, void* isr, uint8_t flags);
extern void die();
extern void idle();
extern void pic_clear_mask(char irq);
void init_idt();
extern void load_idt(idt_ptr* idtr);
extern void enable_interrupts();