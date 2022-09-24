#include <quark/idt.h>
#include <gluon/isrs.h>
#include <stdbool.h>
#include <stddef.h>

char scancodes[] = "\0\e1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0""789-456+1230.\0\0\0\0\0\0\0\0";
char scancodes_shift[] = "\0\e!@#$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0""789-456+1230.\0\0\0\0\0\0\0\0";
bool mod_lcontrol=false;
bool mod_lshift=false;
bool mod_rshift=false;
bool mod_lalt=false;
bool caps_lock=false;
char decode_scancode(uint8_t scancode) {
    char result=0;
    switch (scancode)
    {
    case 0x1D:
        mod_lcontrol = true;
        break;
    case 0x2A:
        mod_lshift = true;
        break;
    case 0x36:
        mod_rshift = true;
        break;
    case 0x38:
        mod_lalt = true;
        break;
    case 0x3A:
        caps_lock = !caps_lock;
        break;
    default:
        if(scancode < 96) {
            if(caps_lock ^ (mod_lshift | mod_rshift)) result=scancodes_shift[scancode];
            else result=scancodes[scancode];
        }
        if(mod_lcontrol) result &= 0x1F;
    }
    return result;
}

void keyboard_handler(uint8_t scancode) {
    char c;
    if((c=decode_scancode(scancode))) printf("%c", &c);
}