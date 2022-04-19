#ifndef EZ_CLANG_QEMU_DEVICE_H
#define EZ_CLANG_QEMU_DEVICE_H

//
// Interface declarations for device/transport specific code
//

typedef _Bool bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

#define nullptr 0
#define false (bool)0
#define true (bool)1

extern void setupSendReceive();
extern void sendBytes(const char *Buffer, uint32_t Size);
extern bool receiveBytes(char Buffer[], uint32_t NumArgBytes);
extern void hardFault();

#endif // EZ_CLANG_QEMU_DEVICE_H
