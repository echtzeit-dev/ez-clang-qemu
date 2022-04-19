# ez-clang-qemu

This repository provides a minimal reference implementation of the [ez-clang RPC and runtime interface](https://github.com/echtzeit-dev/ez-clang#documentation).

The build output is a freestanding firmware binary with no external dependencies. Its size is intended to be below 4kb. It targets the Stellaris LM3S811 evaluation board, which is available as a QEMU device. This firmware implements the minimal interface and exposes no addition symbols.

## Supported host machines

Right now, instructions are tested only on Intel x86_64 hosts running Ubuntu 20.04. If you get it to work on other systems, please document your steps and create a pull request.

## Build

In order to build the firmware you need a cross-toolchain with arm-none-eabi support. Either [download an ARM GCC distribution](https://launchpad.net/gcc-arm-embedded/) or install [PlatformIO](https://docs.platformio.org/en/latest//core/installation.html#super-quick-mac-linux) and get the `toolchain-gccarmnoneeabi` package.

```
➜ python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"
➜ export PATH=$PATH:$HOME/.platformio/penv/bin
➜ platformio platform install atmelsam
```

Next, clone this repository, add your GCC toolchain to `PATH` and run the build script:
```
➜ git clone https://github.com/echtzeit-dev/ez-clang-qemu
➜ cd ez-clang-qemu
➜ export PATH=$PATH:$HOME/.platformio/packages/toolchain-gccarmnoneeabi/bin
➜ ./build.sh
...
Binary output:
-rwxrwxr-x 1 ez ez  70K Apr 19 16:08 bin/ez-clang-qemu.axf
-rwxrwxr-x 1 ez ez 2,4K Apr 19 16:08 bin/ez-clang-qemu.bin
```

## Run

Install [socat](http://www.dest-unreach.org/socat) for virtual serial ports and [QEMU](https://www.qemu.org) for device emulation:
```
➜ sudo apt install qemu-system-arm socat
```

In one terminal open a virtual serial port with socat:
```
➜ socat -d -d pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS90 pty,raw,b9600,echo=0,mode=777,link=/dev/ttyS91
2022/04/05 11:37:16 socat[1064290] N PTY is /dev/pts/3
2022/04/05 11:37:16 socat[1064290] N PTY is /dev/pts/4
2022/04/05 11:37:16 socat[1064290] N starting data transfer loop with FDs [5,5] and [7,7]
```

In a second terminal use QEMU to run the device firmware we built above and let it connect to `ttyS90` (one side of the virtual serial port):
```
➜ cd path/to/ez-clang-qemu
➜ qemu-system-arm -machine lm3s811evb -nographic -m 16K -kernel bin/ez-clang-qemu.bin -serial /dev/ttyS90
QEMU 4.2.1 monitor - type 'help' for more information
(qemu)
```

Now run ez-clang and let it connect to `ttyS91` (the other side):
```
➜ ez-clang --connect=/dev/ttyS91
Welcome to ez-clang, your friendly remote C++ REPL. Type `.q` to exit.
Serial connection: /dev/ttyS91 -> qemu:0.0.5 (arm-none-eabi @ cortex-m3)
qemu> int a = 1;
qemu> a
(int) 1
qemu> a - 2
(int) -1
qemu> .q
```
