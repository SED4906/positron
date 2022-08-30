#include <common/gdt.h>
#include <libc.h>   

gdt_segment gdt[7];
gdt_ptr gdtr = { .limit = sizeof(gdt) - 1,
                           .base = (uint64_t)gdt };
tss_seg tss;

void set_gdt_entry(gdt_segment* seg, uint64_t base, uint16_t limit, uint8_t access, uint8_t flags) {
    seg[0] = limit & 0xFFFF;
    seg[0] |= (base & 0xFFFFFF) << 16;
    seg[0] |= (uint64_t)access << 40;
    seg[0] |= (uint64_t)(limit & 0xF0000) << 48;
    seg[0] |= (uint64_t)(flags & 0x0F) << 52;
    seg[0] |= (uint64_t)(base & 0xFF000000) << 56;
    if(!(access & 16)) {
        seg[1] = base >> 32;
    }
}

void init_gdt() {
    set_gdt_entry(&gdt[0], 0, 0, 0, 0);
    set_gdt_entry(&gdt[1], 0, 0, 0x9A, 0xA);
    set_gdt_entry(&gdt[2], 0, 0, 0x92, 0xC);
    set_gdt_entry(&gdt[3], 0, 0, 0xFA, 0xA);
    set_gdt_entry(&gdt[4], 0, 0, 0xF2, 0xC);
    set_gdt_entry(&gdt[5], (uint64_t)&tss, sizeof(tss_seg), 0x89, 0x0);
    memset(&tss, 0, sizeof(tss_seg));

    load_gdt(&gdtr);
    load_tss();
}