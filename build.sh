#!/bin/bash

export AS=arm-none-eabi-as
export CC=arm-none-eabi-gcc
export LD=arm-none-eabi-ld

export objdump=arm-none-eabi-objdump
export objcopy=arm-none-eabi-objcopy

export TARGET_DEVICE=lm3s811

echo "Building for target device $TARGET_DEVICE:"
make
#make SHELL='sh -x'

echo "ELF output:"
objdump -h bin/ez-clang-qemu.axf

echo "Binary output:"
ls -lh bin/ez-clang-qemu.*

# Some useful commands to run with QEMU
#
# socat -d -d pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS90 pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS91
# qemu-system-arm -machine lm3s811evb -nographic -m 16K -kernel bin/ez-clang-qemu.bin -serial /dev/ttyS90
# ez-clang --connect=/dev/ttyS91
