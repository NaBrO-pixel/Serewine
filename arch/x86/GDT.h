#pragma once
#include <stdint.h>

namespace Kernel {

struct __attribute__((packed)) GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
};

struct __attribute__((packed)) GDTPointer {
    uint16_t limit;
    uint32_t base;
};

class GDT {
public:
    static void initialize();

private:
    static void set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
    static GDTEntry s_entries[5];
    static GDTPointer s_pointer;
};

}

extern "C" void gdt_flush(uint32_t);
