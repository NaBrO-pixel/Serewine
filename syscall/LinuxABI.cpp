#include "LinuxABI.h"
#include "../Arch/x86/VGA.h"

namespace Kernel {

extern "C" uint32_t linux_syscall_dispatch(RegisterState* regs)
{
    return LinuxABI::dispatch(*regs);
}

uint32_t LinuxABI::dispatch(RegisterState& regs)
{
    switch (regs.eax) {
    case SYS_write:
        // ebx=fd, ecx=buf, edx=count. Only stdout/stderr to VGA for now,
        // and no userspace page validation yet - unsafe, placeholder only.
        if (regs.ebx == 1 || regs.ebx == 2) {
            const char* buf = (const char*)regs.ecx;
            for (uint32_t i = 0; i < regs.edx; i++)
                VGA::putchar(buf[i]);
            return regs.edx;
        }
        return (uint32_t)-1; // -EBADF

    case SYS_exit:
        VGA::write("\n[LinuxABI] process called exit(), halting.\n");
        asm volatile("cli; hlt");
        return 0;

    case SYS_open:
    case SYS_close:
    case SYS_read:
    case SYS_mmap:
    case SYS_munmap:
        // Not implemented yet - these are the real blockers for Wine.
        VGA::write("[LinuxABI] unimplemented syscall #");
        VGA::write_hex(regs.eax);
        VGA::write("\n");
        return (uint32_t)-38; // -ENOSYS

    default:
        VGA::write("[LinuxABI] unknown syscall #");
        VGA::write_hex(regs.eax);
        VGA::write("\n");
        return (uint32_t)-38; // -ENOSYS
    }
}

void LinuxABI::install_handler()
{
    // IDT wiring for int 0x80 goes here once the IDT module exists.
    // Left as a stub: registering syscall_isr at vector 0x80, DPL=3
    // so ring-3 Wine userspace can invoke it.
}

}
