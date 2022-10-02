#pragma once
#include <stdint.h>

typedef uint64_t gdt_segment;
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr;

typedef struct {
    uint32_t u0; 
    uint64_t rsp[3];
    uint64_t u1;
    uint64_t ist[7];
    uint64_t u2;
    uint16_t u3;
    uint16_t iopb;
} __attribute__((packed)) tss_seg;

void init_gdt();
extern void load_gdt(gdt_ptr* gdtr);
extern void load_tss();