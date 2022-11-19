#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t rsp;
    size_t cr3;
} context;

typedef struct task {
    context c; 
    size_t r[16];
    bool last_active;
    int pid;
    struct task *next, *prev;
} task;

extern task *tasks;

void init_task();
void exit_task();