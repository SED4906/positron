#include <common/mm.h>

heap* heaps;

size_t find_run_of_zeros(char* bitmap, size_t bits, size_t bytes) {
    size_t run = 0;
    for(size_t bit=0;bit<bytes*8;bit++) {
        if(run >= bits) {
            return bit - run;
        }
        size_t byte = bit >> 3;
        size_t mask = 1 << (bit & 7);
        if(bitmap[byte] & mask) run = 0; else run++;
    }
    return 0;
}

void set_some_bits(char* bitmap, size_t bits, size_t begin) {
    for(size_t bit=begin;bit<begin+bits;bit++) {
        size_t byte = bit >> 3;
        size_t mask = 1 << (bit & 7);
        bitmap[byte] |= mask;
    }
}

void clear_run_of_ones(char* bitmap, size_t begin, size_t bytes) {
    for(size_t bit=begin;bit<bytes*8;bit++) {
        size_t byte = bit >> 3;
        size_t mask = 1 << (bit & 7);
        if(bitmap[byte] & mask) bitmap[byte] ^= mask; else break;
    }
}

void* alloc(size_t bytes) {
    if(bytes > 3582) return 0;
    if(!heaps) heaps = (heap*)alloc_page();
    heap* current = heaps;
    while(current) {
        size_t pos = 0;
        if(!(pos = find_run_of_zeros(current->bitmap,bytes+2,448))) {
            if(!current->next) current->next = (heap*)alloc_page();
            current = current->next;
        } else {
            set_some_bits(current->bitmap, bytes, pos+1);
            return &current->bitmap[513+pos];
        }
    }
    return 0;
}

void dealloc(void* ptr) {
    if(!ptr) return;
    size_t pos = (size_t)ptr & 0xFFF;
    if(!pos) return;
    pos -= 512;
    heap* current = (heap*)((size_t)ptr &~ 0xFFF);
    clear_run_of_ones(current->bitmap, pos, 448);
}