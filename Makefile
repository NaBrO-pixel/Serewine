CXX = g++
AS = nasm
LD = ld

CXXFLAGS = -m32 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector \
           -nostdlib -Wall -Wextra -O2 -std=c++17
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T Kernel/linker.ld -nostdlib

CXX_SOURCES = Kernel/kernel.cpp \
              Kernel/Arch/x86/VGA.cpp \
              Kernel/Arch/x86/GDT.cpp \
              Kernel/Syscall/LinuxABI.cpp

ASM_SOURCES = Kernel/Boot/boot.s \
              Kernel/Arch/x86/gdt_flush.s

OBJ = $(CXX_SOURCES:.cpp=.o) $(ASM_SOURCES:.s=.o)

KERNEL_BIN = build/serenwine.bin
ISO = build/serenwine.iso

.PHONY: all clean iso

all: $(KERNEL_BIN)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJ)
	mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

iso: $(KERNEL_BIN)
	mkdir -p build/isodir/boot/grub
	cp $(KERNEL_BIN) build/isodir/boot/serenwine.bin
	cp grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) build/isodir

run: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -f $(OBJ)
	rm -rf build
