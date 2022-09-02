#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct { size_t rsp;
                 size_t cr3; }
                 context;

typedef struct task { context c; 
                      size_t r[16];
                      int active;
                      struct task* next; }
                      task;

void init_task(task* first);