# This is the name that our final kernel executable will have.
# Change as needed.
KERNEL := positron.elf

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
#CC := x86_64-elf-gcc
CC := clang
CPP := clang++
# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".
#LD := x86_64-elf-ld
LD := ld.lld

AS := nasm

# User controllable CFLAGS.
CFLAGS ?= -Wall -Wextra -O0 -pipe -Iinclude
CPPFLAGS ?= -Wall -Wextra -O0 -pipe -Iinclude

# User controllable linker flags. We set none by default.
LDFLAGS ?= 	-Tplasma/$(ARCH)/linker.ld \
	-nostdlib              \
	-static                \
	-pie                   \
	--no-dynamic-linker 

CFLAGS +=            \
	-I.                  \
	-std=c17           \
	-ffreestanding       \
	-fno-stack-protector \
	-fpie                \
	-fPIC \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-target x86_64-elf \
	-g

CPPFLAGS +=            \
	-I.                  \
	-std=c++20           \
	-ffreestanding       \
	-fno-stack-protector \
	-fpie                \
	-fPIC \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-target x86_64-elf \
	-g

ARCH := arch/x86_64

OBJ := plasma/$(ARCH)/main.o quark/libc.o
OBJ += quark/$(ARCH)/gdt.o quark/$(ARCH)/idt.o quark/$(ARCH)/mm.o quark/$(ARCH)/quarkS.o quark/$(ARCH)/task.o
OBJ += gluon/$(ARCH)/fb.o gluon/$(ARCH)/mm.o gluon/$(ARCH)/printf.o gluon/$(ARCH)/process.o
OBJ += gluon/$(ARCH)/isrs/isrs.o gluon/$(ARCH)/isrs/isrsS.o gluon/$(ARCH)/isrs/page_fault.o gluon/$(ARCH)/isrs/keyb.o gluon/$(ARCH)/isrs/gp_fault.o

# Default target.
.PHONY: all
all: $(KERNEL)

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) -felf64 -g $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ)