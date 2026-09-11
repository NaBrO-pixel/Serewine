#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Kernel {

class VGA {
public:
    static void initialize();
    static void clear();
    static void putchar(char c);
    static void write(const char* str);
    static void write_hex(uint32_t value);
    static void set_color(uint8_t fg, uint8_t bg);

private:
    static uint16_t* s_buffer;
    static size_t s_row;
    static size_t s_col;
    static uint8_t s_color;

    static uint16_t entry(char c, uint8_t color);
    static void scroll_if_needed();
};

}
