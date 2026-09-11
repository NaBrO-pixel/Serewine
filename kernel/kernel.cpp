#include <stdint.h>
#include "Arch/x86/VGA.h"
#include "Arch/x86/GDT.h"

using namespace Kernel;

extern "C" void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_addr)
{
    VGA::initialize();
    VGA::set_color(0x0A, 0x00); // bright green on black
    VGA::write("SerenWine kernel booted.\n");
    VGA::set_color(0x0F, 0x00);

    VGA::write("Multiboot magic: ");
    VGA::write_hex(multiboot_magic);
    VGA::write("\n");

    GDT::initialize();
    VGA::write("GDT installed (5 descriptors: null/kcode/kdata/ucode/udata).\n");

    VGA::write("\nStatus: boot + segmentation only.\n");
    VGA::write("Not yet implemented: IDT/interrupts, paging, physical\n");
    VGA::write("memory manager, ELF loader, Linux-ABI syscalls beyond\n");
    VGA::write("write()/exit() stubs. See Kernel/Syscall/LinuxABI.h for\n");
    VGA::write("the real list of what Wine needs from this kernel.\n");

    (void)multiboot_info_addr;

    for (;;)
        asm volatile("hlt");
}

// Minimal freestanding C++ ABI requirements (no exceptions/RTTI used)
extern "C" void __cxa_pure_virtual() { for (;;) asm volatile("hlt"); }
