#include "GDT.h"

namespace Kernel {

GDTEntry GDT::s_entries[5];
GDTPointer GDT::s_pointer;

void GDT::set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    s_entries[index].base_low = base & 0xFFFF;
    s_entries[index].base_middle = (base >> 16) & 0xFF;
    s_entries[index].base_high = (base >> 24) & 0xFF;
    s_entries[index].limit_low = limit & 0xFFFF;
    s_entries[index].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    s_entries[index].access = access;
}

void GDT::initialize()
{
    s_pointer.limit = sizeof(GDTEntry) * 5 - 1;
    s_pointer.base = (uint32_t)&s_entries;

    set_entry(0, 0, 0, 0, 0);                // null
    set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // kernel code, ring 0
    set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // kernel data, ring 0
    set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);  // user code, ring 3 (needed for Linux-ABI userspace later)
    set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // user data, ring 3

    gdt_flush((uint32_t)&s_pointer);
}

}
