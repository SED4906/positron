#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct freelist { struct freelist* next; } freelist;

typedef struct heap {
    char bitmap[448];
    struct heap* next;
} heap;

void* alloc(size_t bytes);

void dealloc(void* ptr);

size_t alloc_page();

void dealloc_page(size_t page);

size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags);

size_t get_virtual_mapping(size_t pmap, size_t vaddr);

void init_mm();

size_t get_pmap();

extern uint64_t hhdm;
extern size_t cr3;