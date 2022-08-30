#pragma once
#include <stddef.h>
#include <stdint.h>
#include <common/task.h>

typedef struct process { struct task t;
                         int pid;
                         struct process* next; } process;

void init_process();