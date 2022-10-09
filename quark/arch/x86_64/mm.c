#include <quark/mm.h>
#include <quark/idt.h>
#include <common/limine.h>
#include <libc.h>

freelist* free_pages;
uint64_t hhdm;
heap* heaps;
size_t cr3;

/// @brief Physical memory page allocation
/// @return Address of next free page, 0 if fail.
size_t alloc_page() {
    size_t ret = (size_t)free_pages;
    free_pages = ((freelist*)(ret + hhdm))->next;
    memset((void*)(ret+hhdm),0,4096);
    return ret;
}

/// @brief Physical memory page deallocation
/// @param page Page to deallocate.
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

    // Goes through memory map and adds usable pages one by one.
    for(size_t e=0;e<memmap_request.response->entry_count;e++) {
        struct limine_memmap_entry* en=memmap_request.response->entries[e];
        if(en->type != LIMINE_MEMMAP_USABLE) continue;
        for(size_t o=0;o<en->length;o+=4096) dealloc_page(en->base + o);
    }

    // Unmaps identity mapping, should reveal bugs.
    size_t* pmap = (size_t*)get_pmap();
    pmap[0] = 0;
    cr3 = (size_t)pmap;
}

/// @brief Step of a virtual memory mapping
/// @param pmap Page map to add entry to.
/// @param entry Entry number, 0-511.
/// @return Address of new sub-page map.
size_t map_page_step(size_t pmap, size_t entry) {
    // If an entry is present, return its page-aligned address.
    if(((size_t*)(pmap + hhdm))[entry] & 1) return ((size_t*)(pmap + hhdm))[entry] &~ 0xFFF;
    // Otherwise create a new one.
    size_t ret = alloc_page();
    memset((void*)(ret+hhdm),0,4096);
    ((size_t*)(pmap + hhdm))[entry] = ret | 7;
    return ret;
}

/// @brief Maps virtual address in page map to physical address.
/// @param pmap Page map to map in.
/// @param vaddr Virtual address to map.
/// @param paddr Physical address to map.
/// @param flags Flags for the mapping.
/// @return The same virtual address.
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

/// @brief Step of getting a virtual memory mapping
/// @param pmap Page map to check entry of.
/// @param entry Entry number, 0-511.
/// @return Address of sub-page mapping if it exists, 0 otherwise.
size_t get_virtual_mapping_step(size_t pmap, size_t entry) {
    // If an entry is present, return its page-aligned address.
    if(((size_t*)(pmap + hhdm))[entry] & 1) return ((size_t*)(pmap + hhdm))[entry] &~ 0xFFF;
    return 0;
}

size_t get_virtual_mapping(size_t pmap, size_t vaddr) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    pml3 = get_virtual_mapping_step(pmap, pml4_entry);
    if(!pml3) return 0;
    pml2 = get_virtual_mapping_step(pml3, pml3_entry);
    if(!pml2) return 0;
    pml1 = get_virtual_mapping_step(pml2, pml2_entry);
    if(!pml1) return 0;
    return ((size_t*)(pml1 + hhdm))[pml1_entry];
}

/// @brief Finds a run of successive zeros in a bitmap of a specified length
/// @param bitmap The bitmap to scan.
/// @param bits The desired run length.
/// @param bytes The length of the bitmap.
/// @return Bit position of start of run, -1 if fail.
int find_run_of_zeros(char* bitmap, size_t bits, size_t bytes) {
    size_t run = 0;
    for(size_t bit=0;bit<bytes*8;bit++) {
        if(run >= bits) {
            return bit - run;
        }
        size_t byte = bit >> 3;
        size_t mask = 1 << (bit & 7);
        if(bitmap[byte] & mask) run = 0; else run++;
    }
    return -1;
}

/// @brief Sets some bits.
/// @param bitmap The bitmap to modify.
/// @param bits The length in bits to set.
/// @param begin The first bit position to set.
void set_some_bits(char* bitmap, size_t bits, size_t begin) {
    for(size_t bit=begin;bit<begin+bits;bit++) {
        size_t byte = bit >> 3;
        size_t mask = 1 << (bit & 7);
        bitmap[byte] |= mask;
    }
}

/// @brief Clears a run of consecutive ones.
/// @param bitmap The bitmap to modify.
/// @param begin The first bit position to clear.
/// @param bytes The length of the bitmap.
void clear_run_of_ones(char* bitmap, size_t begin, size_t bytes) {
    for(size_t bit=begin;bit<bytes*8;bit++) {
        size_t byte = bit >> 3;
        size_t mask = 1 << (bit & 7);
        if(bitmap[byte] & mask) bitmap[byte] ^= mask; else break;
    }
}

/// @brief Small memory allocation.
/// @param bytes Length of allocation (<=3582)
/// @return Pointer to allocated bytes.
void* alloc(size_t bytes) {
    if(bytes > 3582) return 0;
    if(!heaps) heaps = (heap*)(alloc_page()+hhdm);
    heap* current = heaps;
    while(current) {
        size_t pos = 0;
        if((pos = find_run_of_zeros(current->bitmap,bytes+2,448)) < 0) {
            if(!current->next) current->next = (heap*)(alloc_page()+hhdm);
            current = current->next;
        } else {
            set_some_bits(current->bitmap, bytes, pos+1);
            return &current->bitmap[513+pos];
        }
    }
    return 0;
}

/// @brief Small memory deallocation.
/// @param ptr Pointer to the allocated memory.
void dealloc(void* ptr) {
    if(!ptr) return;
    size_t pos = (size_t)ptr & 0xFFF;
    if(!pos) return;
    pos -= 512;
    heap* current = (heap*)((size_t)ptr &~ 0xFFF);
    clear_run_of_ones(current->bitmap, pos, 448);
}