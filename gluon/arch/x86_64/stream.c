#include <gluon/stream.h>
#include <quark/mm.h>
#include <libc.h>
#include <stddef.h>

int read(file_handle* handle, void* buffer, size_t bytes) {
    size_t data = get_virtual_mapping(handle->space, (size_t)handle->data) + hhdm;
    if(handle->pos+bytes <= handle->len) {
        memcpy(buffer, (void*)data, bytes);
        handle->pos += bytes;
        return bytes;
    } else {
        int bytes_actual = handle->len-handle->pos;
        memcpy(buffer, (void*)data, bytes_actual);
        handle->pos = handle->len;
        return bytes_actual;
    }
}

int write(file_handle* handle, void* buffer, size_t bytes) {
    size_t data = get_virtual_mapping(handle->space, (size_t)handle->data) + hhdm;
    if(handle->len + bytes > 4096) {
        int bytes_actual = 4096-handle->len;
        memcpy((void*)data,buffer,bytes_actual);
        handle->len = 4096;
        return bytes_actual;
    } else {
        memcpy((void*)data,buffer,bytes);
        handle->len += bytes;
        return bytes;
    }
}

void flush(file_handle* handle) {
    size_t data = get_virtual_mapping(handle->space, (size_t)handle->data) + hhdm;
    memmove((void*)data, (void*)(data + handle->pos),handle->len - handle->pos);
    handle->len -= handle->pos;
    handle->pos = 0;
}

file_handle* open(void* data) {
    file_handle* handle = alloc(sizeof(file_handle));
    handle->space = cr3;
    handle->data = data;
    handle->pos = 0;
    handle->len = 0;
    return handle;
}

void* map(void* addr) {
    void* page = alloc_page();
    if(!page) return 0;
    if(!map_page(cr3, addr, page,7)) return 0;
    return addr;
}