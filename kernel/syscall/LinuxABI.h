#pragma once
#include <stdint.h>

// Wine (compiled for Linux) makes raw int 0x80 / syscall instructions expecting
// the Linux i386 syscall ABI: eax = syscall number, ebx/ecx/edx/esi/edi = args,
// return value in eax. This is DIFFERENT from NovarisOS's Win32 API surface -
// Wine itself is a Linux userspace program that translates Win32 calls down
// into Linux syscalls (mmap, open, read, futex, clone, etc). To run Wine here,
// this kernel needs to answer THOSE syscalls convincingly, not emulate Win32.
//
// This stub currently implements only enough to prove the dispatch path works.
// Real work ahead: mmap/munmap (Wine needs anonymous+file mappings for PE
// loading), open/read/write/close, futex (Wine's threading depends on it),
// clone (thread creation, distinct from fork), and a real ELF loader for
// Wine's own binaries and shared libraries.

namespace Kernel {

struct RegisterState {
    uint32_t edi, esi, ebp, esp_unused;
    uint32_t ebx, edx, ecx, eax;
};

// Linux i386 syscall numbers this stub currently recognizes (unistd_32.h subset)
enum LinuxSyscall : uint32_t {
    SYS_exit = 1,
    SYS_read = 3,
    SYS_write = 4,
    SYS_open = 5,
    SYS_close = 6,
    SYS_mmap = 90,
    SYS_munmap = 91,
};

class LinuxABI {
public:
    static void install_handler();
    static uint32_t dispatch(RegisterState&);
};

}

extern "C" void syscall_isr();
