#!/bin/bash

export PATH=$PATH:~/.platformio/packages/toolchain-gccarmnoneeabi/bin

export AS=arm-none-eabi-as
export CC=arm-none-eabi-gcc
export LD=arm-none-eabi-ld

export objdump=arm-none-eabi-objdump
export objcopy=arm-none-eabi-objcopy

export TARGET_DEVICE=lm3s811

echo "Building for target device $TARGET_DEVICE:"
make DEBUG=1
#make DEBUG=1 SHELL='sh -x'

echo "ELF output with debug info:"
objdump -h bin/ez-clang-qemu.axf

echo "Binary output:"
ls -lh bin/ez-clang-qemu.bin

# Some useful commands to run and debug with QEMU
#
# socat -d -d pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS90 pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS91
# qemu-system-arm -machine lm3s811evb -nographic -m 16K -kernel bin/ez-clang-qemu.bin -serial /dev/ttyS90 -S -gdb :3333
# ez-clang --connect=/dev/ttyS91 --verbose --debug --debug-only=orc,serial
#
# export PATH=$PATH:~/.platformio/packages/toolchain-gccarmnoneeabi/bin
# arm-none-eabi-gdb bin/ez-clang-qemu.axf
# > target remote:3333
# > display /3i $pc
