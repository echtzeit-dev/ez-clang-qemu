#!/bin/bash

# With PlatformIO you have all these tools in:
# export PATH=$PATH:~/.platformio/packages/toolchain-gccarmnoneeabi/bin

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
objdump -h ${TARGET_DEVICE}/release.axf

echo "Binary output:"
ls -lh ${TARGET_DEVICE}/release.*

# The Python Device Layer runs QEMU in a subprocess and connects through pipes.
# Alternatively connect QEMU manually through a virtual serial port:
#
# socat -d -d pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS90 pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS91
# qemu-system-arm -machine lm3s811evb -nographic -m 16K -kernel bin/lm32811.bin -serial /dev/ttyS90
# ez-clang --connect=/dev/ttyS91
