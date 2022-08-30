#pragma once
#include <stddef.h>
#include <stdint.h>

void fb_plot(uint16_t x, uint16_t y, uint32_t rgb);
uint16_t fb_width();
uint16_t fb_height();
void fb_clear_screen();
size_t fb_print_string(const char * str, size_t len);