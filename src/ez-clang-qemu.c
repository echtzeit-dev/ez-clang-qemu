#include "device.h"

#define c_array_size(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

//
// Static buffer for RPC requests and responses
//
char MessageBuffer[0x0400] __attribute__((aligned(0x0100)));

//
// Boundaries of the code buffer are provided from linker script
//
extern char _scode_buffer;
extern char _ecode_buffer;

//
// Compact implementations for stdlib functions (with non-standard signatures!)
//
uint32_t strlen(const char *src) {
  uint32_t length = 0;
  while (*(src + length) != '\0')
    length += 1;
  return length;
}

void memset(char *Dst, int Value, uint32_t Size) {
  for (uint32_t i = 0; i < Size; i++)
    Dst[i] = Value;
}

void memcpy(char *Dst, const char *Src, uint32_t Size) {
  for (uint32_t i = 0; i < Size; ++i)
    Dst[i] = Src[i];
}

int memcmp(const char *A, const char *B, uint32_t Size) {
  for (uint32_t i = 0; i < Size; ++i)
    if (A[i] != B[i])
      return A[i] > B[i] ? 1 : -1;
  return 0; // Equal
}

//
// I/O functions for RPC stream access
//
uint32_t writeBool(char Buffer[], bool Value) {
  Buffer[0] = Value ? 0x01 : 0x00;
  return 1;
}

uint32_t writeUInt64(char *Buffer, uint32_t Value) {
  // Write 32-bit unsigned integer as 64-bit little endian
  #define Mask 0xFFul
  Buffer[0] = (uint8_t)((Value & (Mask << 0)) >> 0);
  Buffer[1] = (uint8_t)((Value & (Mask << 8)) >> 8);
  Buffer[2] = (uint8_t)((Value & (Mask << 16)) >> 16);
  Buffer[3] = (uint8_t)((Value & (Mask << 24)) >> 24);
  Buffer[4] = 0;
  Buffer[5] = 0;
  Buffer[6] = 0;
  Buffer[7] = 0;
  return 8;
  #undef Mask
}

uint32_t writeString(char Buffer[], const char *Value) {
  uint32_t Chars = strlen(Value);
  uint32_t LengthOffset = writeUInt64(Buffer, Chars);
  memcpy(Buffer + LengthOffset, Value, Chars);
  return LengthOffset + Chars;
}

uint32_t readUInt64as32(const char Buffer[], uint32_t *Value) {
  *Value = Buffer[0];
  *Value |= (uint32_t)(Buffer[1]) << 8;
  *Value |= (uint32_t)(Buffer[2]) << 16;
  *Value |= (uint32_t)(Buffer[3]) << 24;
  return 8;
}

uint32_t readAddr(const char Buffer[], uint32_t *Value) {
  return readUInt64as32(Buffer, Value);
}

uint32_t readSize(const char Buffer[], uint32_t *Value) {
  return readUInt64as32(Buffer, Value);
}

//
// Simple pointer and memory address helpers
//
#ifndef _UINTPTR_T
typedef __UINTPTR_TYPE__ uintptr_t;
#define _UINTPTR_T
#endif

char *align16(const char *Ptr) {
  return (char *)((uintptr_t)(Ptr + 0xf) & ~0xf);
}

uint32_t ptr2addr(void *Ptr) {
  return (uint32_t)((uintptr_t)Ptr);
}

char *addr2ptr(uint32_t Addr) {
  return (char *)((uintptr_t)Addr);
}

//
// RPC response and error handling
//
char *ResponseBufferBegin;
const char *ResponseBufferEnd;
uint32_t ResponseBufferCap;
const char *ErrorResponsePtr;

bool error(const char *Message) {
  ErrorResponsePtr = Message;
  return false;
}

void responseClearBuffer() {
  ResponseBufferBegin = nullptr;
  ResponseBufferEnd = nullptr;
  ResponseBufferCap = 0;
  ErrorResponsePtr = nullptr;
}

const char *responseSetBuffer(char *InputEnd, uint32_t CapacityRemaining) {
  ResponseBufferBegin = align16(InputEnd);
  uint32_t Padding = ResponseBufferBegin - InputEnd;
  memset(InputEnd, 0, Padding);
  ResponseBufferCap = CapacityRemaining - Padding;
  return ResponseBufferBegin;
}

char *responseAcquire(uint32_t Size) {
  if (Size > ResponseBufferCap)
    error("Response size exceeds message buffer capacity");
  ResponseBufferEnd = ResponseBufferBegin + Size;
  return ResponseBufferBegin;
}

const char *responseFinalize(char *ResultEnd) {
  if (ResultEnd != ResponseBufferEnd)
    error("Response capacity tracking failure");
  return ResultEnd;
}

//
// Symbol handling and lookup
//
struct Symbol {
  const char *Name;
  uint32_t Addr;
};

#define STRINGIFY(NAME) #NAME
#define X(NAME) { \
  .Name = STRINGIFY(NAME), \
  .Addr = ptr2addr((void *)&NAME) \
}

#define SymbolNotFound 0

uint32_t lookupInArray(const char *Name, uint32_t Length,
                       const struct Symbol *Syms, uint32_t NumSyms) {
  // We compare with the serialized strings from the RPC payload directly.
  // They are not zero-terminated, thus we use memcmp!
  for (uint32_t i = 0; i < NumSyms; i++)
    if (Length == strlen(Syms[i].Name))
      if (memcmp(Syms[i].Name, Name, Length) == 0)
        return Syms[i].Addr;
  return SymbolNotFound;
}

const char *__ez_clang_rpc_lookup(const char *Data, uint32_t Size);
const char *__ez_clang_rpc_commit(const char *Data, uint32_t Size);
const char *__ez_clang_rpc_execute(const char *Data, uint32_t Size);
const char *__ez_clang_rpc_mem_read_cstring(const char *Data, uint32_t Size);

uint32_t lookupRPCEndpoint(const char *Name, uint32_t Length) {
  // Is there a maintainable way to make this static in C?
  const struct Symbol Syms[] = {
    X(__ez_clang_rpc_lookup),
    X(__ez_clang_rpc_commit),
    X(__ez_clang_rpc_execute),
    X(__ez_clang_rpc_mem_read_cstring),
  };
  return lookupInArray(Name, Length, Syms, c_array_size(Syms));
}

void __ez_clang_report_value(uint32_t SeqID, const char *Blob, uint32_t Size);
void __ez_clang_report_string(const char *Data, uint32_t Size);
char *__ez_clang_inline_heap_acquire(uint32_t Bytes);

uint32_t lookupRuntimeFunction(const char *Name, uint32_t Length) {
  // Is there a maintainable way to make this static in C?
  const struct Symbol Syms[] = {
    X(__ez_clang_report_value),
    X(__ez_clang_report_string),
    X(__ez_clang_inline_heap_acquire)
  };
  return lookupInArray(Name, Length, Syms, c_array_size(Syms));
}

uint32_t lookupSymbolAddr(const char *Name, uint32_t Length) {
  const char *PrefixRPC = "__ez_clang_rpc_";
  if (memcmp(Name, PrefixRPC, strlen(PrefixRPC)) == 0)
    return lookupRPCEndpoint(Name, Length);

  const char *PrefixRuntime = "__ez_clang_";
  if (memcmp(Name, PrefixRuntime, strlen(PrefixRuntime)) == 0)
    return lookupRuntimeFunction(Name, Length);

  return SymbolNotFound;
}

//
// RPC interface implementation
//
const char *__ez_clang_rpc_lookup(const char *Data, uint32_t Size) {
  uint32_t SymbolsRemaining;
  Data += readSize(Data, &SymbolsRemaining);

  char *Resp = responseAcquire(1 + 8 + SymbolsRemaining * 8);
  Resp += writeBool(Resp, false); // HasError
  Resp += writeUInt64(Resp, SymbolsRemaining);

  while (SymbolsRemaining > 0) {
    uint32_t Length;
    Data += readSize(Data, &Length);
    uint32_t Addr = lookupSymbolAddr(Data, Length);
    Resp += writeUInt64(Resp, Addr);
    Data += Length; // No terminating zero
    SymbolsRemaining -= 1;
  }

  return responseFinalize(Resp);
}

const char *__ez_clang_rpc_commit(const char *Data, uint32_t Size) {
  uint32_t SegmentsRemaining;
  Data += readSize(Data, &SegmentsRemaining);

  while (SegmentsRemaining > 0) {
    uint32_t TargetAddr;
    Data += readAddr(Data, &TargetAddr);
    uint32_t SegmentSize;
    Data += readSize(Data, &SegmentSize);
    uint32_t ContentSize;
    Data += readSize(Data, &ContentSize);
    memcpy(addr2ptr(TargetAddr), Data, ContentSize);
    memset(addr2ptr(TargetAddr + ContentSize), 0,
           SegmentSize - ContentSize);
    Data += ContentSize;
    SegmentsRemaining -= 1;
  }

  char *Resp = responseAcquire(1);
  Resp += writeBool(Resp, false); // HasError
  return responseFinalize(Resp);
}

const char *InlineHeapEnd = nullptr;
const char *InlineHeapPtr = nullptr;

const char *__ez_clang_rpc_execute(const char *Data, uint32_t Size) {
  uint32_t FnAddr;
  Data += readAddr(Data, &FnAddr);

  typedef void Fn_t(void *);
  Fn_t *Fn = (Fn_t *)addr2ptr(FnAddr);

  // Acquire response memory so we can provide the remaining MessageBuffer space
  // as an inline-heap for the function; it's accessible from JITed code via
  // __ez_clang_inline_heap_aquire()
  char *Resp = responseAcquire(1);
  InlineHeapPtr = align16(ResponseBufferEnd);
  InlineHeapEnd = ResponseBufferBegin + ResponseBufferCap;

  uint64_t Unused = 0;
  Fn((void *)&Unused);

  InlineHeapPtr = nullptr;
  InlineHeapEnd = nullptr;
  Resp += writeBool(Resp, false); // HasError
  return responseFinalize(Resp);
}

const char *__ez_clang_rpc_mem_read_cstring(const char *Data, uint32_t Size) {
  uint32_t Addr;
  readAddr(Data, &Addr);
  const char *Str = addr2ptr(Addr);
  char *Resp = responseAcquire(8 + strlen(Str));
  Resp += writeString(Resp, Str);
  return responseFinalize(Resp);
}

//
// RPC Opcodes
//
enum { Setup = 0, Hangup, Result, CallWrapper, ReportValue, ReportString };
void sendMessage(uint8_t OpCode, uint32_t SeqID, const char ArgBytes[], uint32_t NumArgBytes);

//
// Runtime interface implementation
//
void __ez_clang_report_value(uint32_t SeqID, const char *Blob, uint32_t Size) {
  // The host uses this function to print expression values. It knows the type
  // of the data in this blob.
  sendMessage(ReportValue, SeqID, Blob, Size);
}

void __ez_clang_report_string(const char *Data, uint32_t Size) {
  sendMessage(ReportString, 0, Data, Size);
}

char *__ez_clang_inline_heap_acquire(uint32_t Bytes) {
  const char *NextBlock = align16(InlineHeapPtr + Bytes);
  if (NextBlock > InlineHeapEnd)
    return nullptr;
  char *Block = (char *)InlineHeapPtr;
  InlineHeapPtr = NextBlock;
  return Block;
}

//
// High-level transport protocol
//
#define MessageHeaderSize 32

void sendMessage(uint8_t OpCode, uint32_t SeqID, const char ArgBytes[],
                 uint32_t NumArgBytes) {
  char HeaderBuffer[MessageHeaderSize];
  char *Data = HeaderBuffer;
  Data += writeUInt64(Data, NumArgBytes + MessageHeaderSize);
  Data += writeUInt64(Data, OpCode);
  Data += writeUInt64(Data, SeqID);
  Data += writeUInt64(Data, 0);
  sendBytes(HeaderBuffer, Data - HeaderBuffer);
  sendBytes(ArgBytes, NumArgBytes);
}

void sendSetupMessage(char Buffer[]) {
  // Magic sequence is symmetric, so we can ignore endianness:
  // 00000001 00100011 01010111 10111101 10111101 01010111 00100011 00000001
  const uint64_t SetupMagic = 0x012357BDBD572301ull;
  sendBytes((const char*)&SetupMagic, sizeof(uint64_t));

  char *Data = Buffer;
  Data += writeString(Data, "0.0.5");
  Data += writeUInt64(Data, ptr2addr(&_scode_buffer));
  Data += writeUInt64(Data, &_ecode_buffer - &_scode_buffer);

  struct Symbol BootstrapSym = X(__ez_clang_rpc_lookup);
  Data += writeUInt64(Data, 1);
  Data += writeString(Data, BootstrapSym.Name);
  Data += writeUInt64(Data, BootstrapSym.Addr);

  sendMessage(Setup, 0, Buffer, Data - Buffer);
}

void sendHangupMessage(bool Success) {
  uint32_t NumArgBytes = 1;
  char SuccessState = Success ? 0 : 1;
  if (!Success && ErrorResponsePtr != nullptr)
    NumArgBytes += strlen(ErrorResponsePtr);

  char Buffer[MessageHeaderSize];
  char *Data = Buffer;
  Data += writeUInt64(Data, NumArgBytes + MessageHeaderSize);
  Data += writeUInt64(Data, Hangup);
  Data += writeUInt64(Data, 0);
  Data += writeUInt64(Data, 0);
  sendBytes(Buffer, Data - Buffer);
  sendBytes(&SuccessState, 1);

  if (!Success && ErrorResponsePtr != nullptr) {
    uint64_t ErrorLen = strlen(ErrorResponsePtr);
    uint32_t LenBytes = writeUInt64(Buffer, ErrorLen);
    sendBytes(Buffer, LenBytes);
    sendBytes(ErrorResponsePtr, ErrorLen);
  }
}

//
// Top-level message loop
//
typedef const char *(RPCEndpoint)(const char *, uint32_t);

bool parseHeader(char Buffer[], uint32_t *OpCode, uint32_t *SeqID,
                 uint32_t *NumArgBytes, RPCEndpoint **Func) {
  const char *Data = Buffer;
  Data += readSize(Data, NumArgBytes);

  if (*NumArgBytes <= MessageHeaderSize)
    return error("Received truncated message");

  *NumArgBytes -= MessageHeaderSize;
  Data += readUInt64as32(Data, OpCode);

  switch (*OpCode) {
    case CallWrapper: break;
    case Hangup: return false; // Explicit restart
    default:
      return error("Received unexpected message op-code");
  }

  // ID to use for responses
  Data += readUInt64as32(Data, SeqID);

  uint32_t TagAddr;
  Data += readAddr(Data, &TagAddr);
  *Func = (RPCEndpoint *)(uintptr_t)TagAddr;
  return true;
}

bool runMessageLoop() {
  while (true) {
    responseClearBuffer();
    if (!receiveBytes(MessageBuffer, MessageHeaderSize))
      return false;

    RPCEndpoint *Handler;
    uint32_t OpCode, SeqID, NumArgBytes;
    if (!parseHeader(MessageBuffer, &OpCode, &SeqID, &NumArgBytes, &Handler))
      return OpCode == Hangup;

    if (NumArgBytes > c_array_size(MessageBuffer))
      return error("Message payload exceeds buffer size");

    if (!receiveBytes(MessageBuffer, NumArgBytes))
      return false;

    // All input was collected. Define the ResponseBuffer in direct succession.
    const char *RespBegin =
        responseSetBuffer(MessageBuffer + NumArgBytes,
                          c_array_size(MessageBuffer) - NumArgBytes);

    // Invoke RPC endpoint and sent back the response in a Result message
    const char *RespEnd = Handler(MessageBuffer, NumArgBytes);
    sendMessage(Result, SeqID, RespBegin, RespEnd - RespBegin);
  }
}

int main() {
  setupSendReceive();
  sendSetupMessage(MessageBuffer);
  bool Success = runMessageLoop();
  sendHangupMessage(Success);
  return 0;
}

void hardFault() {
  sendHangupMessage(error("Hard fault. Halting."));
}
