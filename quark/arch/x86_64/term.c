#include <common/limine.h>
#include <stddef.h>

struct limine_terminal_request terminal_req = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0, .response = 0
};

size_t terminal_write(const char* str, size_t len) {
    if(!terminal_req.response) return 0;
    terminal_req.response->write(terminal_req.response->terminals[0], str, len);
    return len;
}