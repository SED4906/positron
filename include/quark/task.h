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
    int pid,uid;
    //size_t text_s,text_e;
    size_t data_s,data_e;
    //size_t bss_s,bss_e;
    struct task *next, *prev;
} task;

extern task *tasks;

void init_task();
void exit_task();
void deallocate_data_segment(task* task);
bool within_data(size_t address);