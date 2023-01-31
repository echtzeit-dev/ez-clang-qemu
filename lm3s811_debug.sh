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
make DEBUG=1
#make DEBUG=1 SHELL='sh -x'

echo "ELF output with debug info:"
objdump -h ${TARGET_DEVICE}/debug.axf

echo "Binary output:"
ls -lh ${TARGET_DEVICE}/debug.*

# The Python Device Layer runs QEMU in a subprocess and connects through pipes.
# In order to debug the firmware running in QEMU execute:
#   > ez-clang --connect=lm3s811@qemu --rpc-debug-qemu
#   Welcome to ez-clang, your friendly remote C++ REPL. Type `.q` to exit.
#   QEMU waiting for debugger. Attach to localhost:1234 to proceed.
#   qemu-system-arm -machine lm3s811evb -nographic -m 16K -kernel /usr/lib/ez-clang/0.0.6/firmware/lm3s811/debug.axf -serial stdio -monitor null -s -S
#
# Then attach a debugger like LLDB from the command line:
#   > lldb
#   (lldb) target create /usr/lib/ez-clang/0.0.6/firmware/lm3s811/debug.axf
#   (lldb) gdb-remote localhost:1234
#   Process 1 stopped
#   * thread #1, stop reason = signal SIGTRAP
#       frame #0: 0x0000074e firmware.axf`ResetHandler at lm3s811.c:209:25
#      206  extern unsigned long _bss;
#      207  extern unsigned long _ebss;
#      208
#   -> 209  void ResetHandler(void) {
#      210    // Copy the data segment initializers from flash to SRAM. All addresses
#      211    // provided from linker scripts are 4-byte aligned.
#      212    unsigned long *Src = &_etext;
#
# Or from Visual Studio Code with a config like this:
#   {
#     "name": "Attach qemu",
#     "type": "lldb",
#     "request": "custom",
#     "targetCreateCommands": [
#       "target create /usr/lib/ez-clang/0.0.6/firmware/lm3s811/debug.axf",
#     ],
#     "processCreateCommands": [
#       "gdb-remote localhost:1234",
#     ]
#   }
#
# Or attach GDB like this:
#   > export PATH=$PATH:~/.platformio/packages/toolchain-gccarmnoneeabi/bin
#   > arm-none-eabi-gdb bin/lm3s811.axf
#   (gdb) target remote:1234
#   (gdb) display /3i $pc
#
# Alternatively connect QEMU manually through a virtual serial port:
#   > socat -d -d pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS90 pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS91
#   > qemu-system-arm -machine lm3s811evb -nographic -m 16K -kernel bin/lm3s811.bin -serial /dev/ttyS90 -S -gdb :1234
#   > ez-clang --connect=lm3s811@/dev/ttyS91
