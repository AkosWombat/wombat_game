// TODO: Remove these TODOs

/* TODO:

  - Assert with message, that can be valid to show for end user

  - Zero initialize by function if needed in create call, not by user

  - Possibly call the application entry point from wasp, non platform specific?

  - Clean up module headers, with clear API declarations with implementations below

  - Possibly ban recursion from the codebase?

  - Read only section macro, so we can have read only variables

  - Discord bot

  - Timing helpers

  - Error message / is complete / debug log / is valid / wait for result, functions

  - More SIMD, as well as base SIMD requirements

  - Something like Debugf() in the string library?

  - Start using umm where it makes sense, so in buffers, etc...

  - Possibly use compiler intrinsics for all atomic stuff, and therefore eliminating the need for the volatile keyword?

  - Possibly move out common memory functions from the memory module, it's so commonly used (like MemoryCopy)
    - Especially if we compile without crt where we need them...

  - Pass an arena everywhere that needs to allocate, and error if not enough memory, the using code can reserve as much as max allowed for the application, and any call that cant allocate enough just errors, good way to do this!

*/

#ifndef WASP_H
#define WASP_H

/*
  CONTEXT CRACKING
*/

#if defined(__clang__)
#  define Compiler_CLANG 1

#  if defined(_WIN32)
#    define Platform_WIN32 1
#  elif defined(__linux__)
#    define Platform_LINUX 1
#  elif defined(__APPLE__)
#    define Platform_DARWIN 1
#  elif defined(__wasm__)
#    define Platform_WASM 1
#  else
#    error Unsupported platform
#  endif

#  if defined(__amd64__)
#    define Architecture_X86_64 1
#  elif defined(__i386__)
#    define Architecture_X86_32 1
#  elif defined(__arm__)
#    define Architecture_ARM_32 1
#  elif defined(__aarch64__)
#    define Architecture_ARM_64 1
#  elif defined(__wasm32__)
#    define Architecture_WASM_32 1
#  elif defined(__wasm64__)
#    define Architecture_WASM_64 1
#  else
#    error Unsupported architecture
#  endif
#elif defined(_MSC_VER)
#  define Compiler_MSVC 1

#  if defined(_WIN32)
#    define Platform_WIN32 1
#  else
#    error Unsupported platform
#  endif

#  if defined(_M_X64)
#    define Architecture_X86_64 1
#  elif defined(_M_IX86)
#    define Architecture_X86_32 1
#  elif defined(_M_ARM)
#    define Architecture_ARM_32 1
#  elif defined(_M_ARM64)
#    define Architecture_ARM_64 1
#  else
#    error Unsupported architecture
#  endif

#else
#  error Unsupported compiler
#endif

#if Architecture_X86_64 || Architecture_ARM_64 || Architecture_WASM_64
#  define BitSize_64 1
#elif Architecture_X86_32 || Architecture_ARM_32 || Architecture_WASM_32
#  define BitSize_32 1
#else
#  error Unknown bit size
#endif

#if defined(__cplusplus)
#  define Language_CPP 1

#  if Compiler_MSVC
#    define LanguageVersion _MSVC_LANG
#  else
#    define LanguageVersion __cplusplus
#  endif
#else
#  error Unsupported language
#endif

#if Language_CPP
#  if LanguageVersion < 201103L
#    error We currently require C++11 or above, for the 'alignas' keyword
#  endif
#endif

/*
  CONSTANTS
*/

// TODO: Define once we have context cracking in place
// NOTE: Since we build under rosetta on darwin, cache line size will be 64 instead of 128
#define CacheLineSize 64

#define MaxS32 ((s32)0x7fffffff)
#define MaxS8  ((s8)0x7f)

#define MaxU64 ((u64)~0)
#define MaxU32 ((u32)~0)
#define MaxU16 ((u16)~0)
#define MaxU8   ((u8)~0)

/*
  TYPES
*/

// TODO: Lower-case this? Like other attibutes???

// TODO: Also have a extern-c thing

#if Platform_WASM
#  define ExportSymbol extern "C" __attribute__((visibility("default")))
#else
#  define ExportSymbol
#endif

#define internal static
#define global static
#define tracked

#define flag8(type) u8
#define flag16(type) u16
#define flag32(type) u32
#define flag64(type) u64

#define enum8(type) u8
#define enum16(type) u16
#define enum32(type) u32
#define enum64(type) u64

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uintptr_t umm;
typedef intptr_t smm;

typedef int b32;
typedef char b8;

typedef double f64;
typedef float f32;

struct u128
{
    u64 Low;
    u64 High;
};

struct u256
{
    u128 Low;
    u128 High;
};

struct u512
{
    u256 Low;
    u256 High;
};

struct buffer
{
    u8 *Data;
    umm Size;
};

enum log_severity
{
    LogSeverity_None,
    LogSeverity_Debug,
    LogSeverity_Info,
    LogSeverity_Warning,
    LogSeverity_Error,
};

struct log_entry
{
    log_entry *Next;
    log_entry *Prev;
    
    enum32(log_severity) Severity;
    buffer Message;
};

/*
  GLOBALS
*/

/*
  FUNCTIONS
*/

#define StaticAssert(Expression) static_assert(Expression, "Assertion Failed")
#define Assert(Expression) do {if(!(Expression)) {AssertHandler((char *)(#Expression), (char *)(__FILE__), __LINE__, (char *)(__FUNCTION__));}} while(0)

#define InvalidCodePath Assert(!"Invalid code path")
#define InvalidCase Assert(!"Invalid case")
#define NotImplemented Assert(!"Not implemented")

// TODO: Use some byte swap intrinsic

#define EndianSwap16(Value)  \
((((Value) & 0xff)   << 8) | \
(((Value) & 0xff00) >> 8))

#define EndianSwap32(Value)       \
((((Value) & 0xff)       << 24) | \
(((Value) & 0xff00)     << 8)  | \
(((Value) & 0xff0000)   >> 8)  | \
(((Value) & 0xff000000) >> 24))

#define EndianSwap64(Value)               \
((((Value) & 0xff)               << 56) | \
(((Value) & 0xff00)             << 40) | \
(((Value) & 0xff0000)           << 24) | \
(((Value) & 0xff000000)         << 8)  | \
(((Value) & 0xff00000000)       >> 8)  | \
(((Value) & 0xff0000000000)     >> 24) | \
(((Value) & 0xff000000000000)   >> 40) | \
(((Value) & 0xff00000000000000) >> 56))

#define EndianSwap128(Value) {EndianSwap64((Value).High), EndianSwap64((Value).Low)}
#define EndianSwap256(Value) {EndianSwap128((Value).High), EndianSwap128((Value).Low)}

#define OffsetOf(Type, Member) ((umm)(&((Type *)0)->Member))
#define SizeOf(Value) sizeof(Value)
#define SizeOfMember(Type, Member) SizeOf(&((Type *)0)->Member)
#define SizeOfMemberBuffer(Type, Member) SizeOf(((Type *)0)->Member)
#define AlignAs(Value) alignas(Value)
#define AlignOf(Value) alignof(Value)

#define ArrayCount(Array) (SizeOf(Array) / SizeOf((Array)[0]))

#define DoublyLinkedListInsertAfter(Sentinel, Element) \
(Element)->Next = (Sentinel)->Next;                    \
(Element)->Prev = (Sentinel);                          \
(Element)->Prev->Next = (Element);                     \
(Element)->Next->Prev = (Element)

#define DoublyLinkedListInsertBefore(Sentinel, Element) \
(Element)->Prev = (Sentinel)->Prev;                     \
(Element)->Next = (Sentinel);                           \
(Element)->Prev->Next = (Element);                      \
(Element)->Next->Prev = (Element)

#define DoublyLinkedListRemove(Element)  \
(Element)->Prev->Next = (Element)->Next; \
(Element)->Next->Prev = (Element)->Prev; \
(Element)->Next = 0;                     \
(Element)->Prev = 0

#define DoublyLinkedListIsEmpty(Sentinel) \
(((Sentinel)->Next) == (Sentinel))

#define DoublyLinkedListInit(Sentinel) \
(Sentinel)->Next = (Sentinel);         \
(Sentinel)->Prev = (Sentinel)

// TODO: Rename to power of two
// TODO: Possibly static assert the alignment is power of two?

#define AlignUp(Value, Alignment) (((Value) + ((Alignment)-1)) & ~((Alignment)-1))
#define AlignDown(Value, Alignment) ((Value) & ~((Alignment)-1))
#define IsAligned(Value, Alignment) (((Value) & ((Alignment)-1)) == 0)
#define IsPowerOfTwo(Value) IsAligned(Value, Value)

#define KB(Value) ((umm)(1024 * Value))
#define MB(Value) ((umm)(1024 * KB(Value)))
#define GB(Value) ((umm)(1024 * MB(Value)))
#define TB(Value) ((umm)(1024 * GB(Value)))
#define PB(Value) ((umm)(1024 * TB(Value)))

#define Min(A, B) ((A) < (B) ? (A) : (B))
#define Max(A, B) ((A) > (B) ? (A) : (B))

#define Abs(Value) ((Value) > 0 ? (Value) : -(Value))

#define Clamp(Value, Bottom, Top) Max(Min(Value, Top), Bottom)
#define Clamp01(Value) Clamp(Value, 0.0f, 1.0f)

internal umm GetPageSize(void);
internal inline umm AlignUpToPowerOfTwo(umm Value);

internal void StandardOutput(u8 *Buffer, umm Size);
internal void StandardError(u8 *Buffer, umm Size);

internal void AssertCallback(char *Expression, char *File, int LineNumber, char *Function);
internal void AssertHandler(char *Expression, char *File, int LineNumber, char *Function);

internal inline b32 IsValid(buffer Buffer);

internal void Logf(enum32(log_severity) Severity, char *Format, ...);

internal void LogRegionBegin(void);
internal void LogRegionEnd(void);
internal void LogRegionClear(void);

// TODO: I'm not sure how much I like this getter business, we might just want to return the region instead?
internal inline log_entry *LogRegionGetEntries(void);
internal inline enum32(log_severity) LogRegionGetHighestSeverity(void);

/*
  IMPLEMENTATION
*/

internal inline umm
AlignUpToPowerOfTwo(umm Value)
{
    if(Value == 0)
    {
        Value = 1;
    }
    else
    {
        Value--;
        Value |= Value >> 1;
        Value |= Value >> 2;
        Value |= Value >> 4;
        Value |= Value >> 8;
        Value |= Value >> 16;
#if BitSize_64
        Value |= Value >> 32;
#endif
        Value++;
    }
    
    umm Result = Value;
    return(Result);
}

internal inline b32
IsValid(buffer Buffer)
{
    b32 Result = Buffer.Size != 0;
    Assert(!Result || Buffer.Data); // NOTE: This make some scenarious less convenient but makes more sense in the general picture, and will prevent bugs (I believe there may already be some in the parser because of this...)
    return(Result);
}

#endif // WASP_H