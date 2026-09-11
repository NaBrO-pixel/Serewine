# SerenWine

A SerenityOS-style hobby kernel (C++, multiboot2, Unix-like layout) with the
architecture pointed at eventually running Wine.

## What's here right now

- `Kernel/Boot/boot.s` — multiboot header + 32-bit entry, sets up a stack, jumps to `kernel_main`
- `Kernel/kernel.cpp` — entry point, prints boot banner via VGA
- `Kernel/Arch/x86/VGA.{h,cpp}` — text-mode console driver
- `Kernel/Arch/x86/GDT.{h,cpp}` + `gdt_flush.s` — 5-entry GDT (null, kernel code/data, **user code/data at ring 3** — needed later for real userspace)
- `Kernel/Syscall/LinuxABI.{h,cpp}` — syscall dispatch **stub**: implements `write`/`exit` only, everything else returns `-ENOSYS` and logs

Builds to a valid multiboot ELF and a bootable GRUB ISO. Verified with
`grub-file --is-x86-multiboot` and a full `grub-mkrescue` pass in this
environment — I don't have QEMU here to screenshot the boot, but the binary
and ISO are structurally correct and use the same toolchain path SerenityOS
and your NovarisOS builds use.

## Build

```sh
make          # build/serenwine.bin
make iso      # build/serenwine.iso
make run      # boot it in QEMU (qemu-system-i386)
```

## The actual hard problem: why "SerenityOS + Wine" isn't a small feature

SerenityOS's own syscall ABI is custom (not Linux's), which is why upstream
SerenityOS doesn't run Wine — Wine is a Linux userspace program that expects
the **Linux i386 syscall ABI** (`int 0x80`, `eax` = syscall number, Linux
`errno` conventions). Porting Wine here means the kernel has to answer as a
Linux kernel would for the syscalls Wine actually calls, not implement Win32.
This is the opposite problem from NovarisOS, which already speaks a
substantial chunk of Win32 directly — here the Win32 translation happens
*inside* Wine's userspace code, and this kernel only has to satisfy Wine's
Linux-side calls underneath it.

`Kernel/Syscall/LinuxABI.h` lists what's implemented (`write`, `exit`) versus
what's stubbed (`open`, `read`, `close`, `mmap`, `munmap`) and comments on
why each matters for Wine specifically (PE loading needs `mmap`, threading
needs `futex`/`clone`, etc).

## Realistic build order from here

1. IDT + PIC remap + basic exception handlers (currently missing entirely — no interrupts work yet)
2. Physical + virtual memory manager, paging (needed before `mmap` means anything)
3. Wire `int 0x80` to `LinuxABI::dispatch` via the IDT (`syscall_isr` is declared but not implemented)
4. ELF loader for dynamically linked binaries (Wine and its shared libs are ELF, not PE, at the outer layer)
5. VFS + a real filesystem (Wine needs `open`/`read`/`stat` against real files: its own `.so`s, the Windows PE `.exe`, registry-equivalent config)
6. `mmap`/`munmap` with proper page fault handling
7. Threading primitives: `clone`, `futex` — Wine is thread-heavy
8. Only after 1–7 work: attempt building Wine itself against this syscall surface, expect months of ABI mismatches

This is comparable in scope to what NovarisOS took to reach 51K LOC for
Win32 — the Linux-ABI-for-Wine path is a similarly large, multi-month
project, not a bolt-on. Happy to keep building this out piece by piece
(IDT next is the natural step) rather than attempt it all in one pass.
