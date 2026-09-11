#include "VGA.h"

namespace Kernel {

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

uint16_t* VGA::s_buffer = (uint16_t*)0xB8000;
size_t VGA::s_row = 0;
size_t VGA::s_col = 0;
uint8_t VGA::s_color = 0x0F; // white on black

uint16_t VGA::entry(char c, uint8_t color)
{
    return (uint16_t)c | ((uint16_t)color << 8);
}

void VGA::set_color(uint8_t fg, uint8_t bg)
{
    s_color = fg | (bg << 4);
}

void VGA::initialize()
{
    s_row = 0;
    s_col = 0;
    s_color = 0x0F;
    clear();
}

void VGA::clear()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            s_buffer[y * VGA_WIDTH + x] = entry(' ', s_color);
        }
    }
    s_row = 0;
    s_col = 0;
}

void VGA::scroll_if_needed()
{
    if (s_row < VGA_HEIGHT)
        return;

    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++)
            s_buffer[(y - 1) * VGA_WIDTH + x] = s_buffer[y * VGA_WIDTH + x];
    }
    for (size_t x = 0; x < VGA_WIDTH; x++)
        s_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = entry(' ', s_color);

    s_row = VGA_HEIGHT - 1;
}

void VGA::putchar(char c)
{
    if (c == '\n') {
        s_col = 0;
        s_row++;
        scroll_if_needed();
        return;
    }

    s_buffer[s_row * VGA_WIDTH + s_col] = entry(c, s_color);
    if (++s_col == VGA_WIDTH) {
        s_col = 0;
        s_row++;
        scroll_if_needed();
    }
}

void VGA::write(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i++)
        putchar(str[i]);
}

void VGA::write_hex(uint32_t value)
{
    write("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (value >> i) & 0xF;
        char c = nibble < 10 ? ('0' + nibble) : ('A' + nibble - 10);
        putchar(c);
    }
}

}
