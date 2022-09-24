#include <common/mm.h>
#include <quark/idt.h>
#include <common/limine.h>
#include <libc.h>

freelist* free_pages;
uint64_t hhdm;

size_t alloc_page() {
    size_t ret = (size_t)free_pages;
    free_pages = ((freelist*)(ret + hhdm))->next;
    memset((void*)(ret+hhdm),0,4096);
    return ret;
}

void dealloc_page(size_t page) {
    freelist* entry = (freelist*)page;
    ((freelist*)(page + hhdm))->next = free_pages;
    free_pages = entry;
}

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0, .response = 0
};

struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0, .response = 0
};

void init_mm() {
    if(!memmap_request.response) die();
    if(!hhdm_request.response) die();
    hhdm = hhdm_request.response->offset;

    for(size_t e=0;e<memmap_request.response->entry_count;e++) {
        struct limine_memmap_entry* en=memmap_request.response->entries[e];
        if(en->type != LIMINE_MEMMAP_USABLE) continue;
        for(size_t o=0;o<en->length;o+=4096) dealloc_page(en->base + o);
    }

    size_t* pmap = (size_t*)get_pmap();
    pmap[0] = 0;
}

size_t map_page_step(size_t pmap, size_t entry) {
    if(((size_t*)(pmap + hhdm))[entry] & 1) return ((size_t*)(pmap + hhdm))[entry] &~ 0xFFF;
    size_t ret = alloc_page();
    memset((void*)(ret+hhdm),0,4096);
    ((size_t*)(pmap + hhdm))[entry] = ret | 7;
    return ret;
}

size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    pml3 = map_page_step(pmap, pml4_entry);
    pml2 = map_page_step(pml3, pml3_entry);
    pml1 = map_page_step(pml2, pml2_entry);
    ((size_t*)(pml1 + hhdm))[pml1_entry] = paddr | flags;
    return vaddr;
}