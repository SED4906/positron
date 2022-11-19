#include <gluon/ipc.h>
#include <quark/mm.h>
#include <quark/task.h>
#include <stdint.h>

mbox boxes[256];

char* claim(uint8_t box) {
    if boxes[box].oid < 0 {
        boxes[box].data = (char*)alloc_page() + hhdm;
        boxes[box].oid = tasks->pid;
        boxes[box].flag = false;
    }
    return boxes[box].data;
}

char* find(uint8_t box) {
    return boxes[box].data;
}

void grant(uint8_t box) {
    if boxes[box].oid == tasks->pid {
        dealloc_page(boxes[box].data);
        boxes[box].data = 0;
        boxes[box].oid = -1;
        boxes[box].flag = false;
    }
}

void ready(uint8_t box) {
    if boxes[box].oid == tasks->pid {
        boxes[box].flag = true;
    }
}

void not_ready(uint8_t box) {
    if boxes[box].oid == tasks->pid {
        boxes[box].flag = false;
    }
}

bool check(uint8_t box) {
    return boxes[box].flag;
}

void init_ipc() {
    for(int i=0;i<256;i++) {
        boxes[box].data = 0;
        boxes[box].oid = -1;
        boxes[box].flag = false;
    }
}