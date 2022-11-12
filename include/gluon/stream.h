#pragma once
#include <stddef.h>

typedef struct {
    size_t space;
    void* data;
    size_t pos;
    size_t len;
} file_handle;
