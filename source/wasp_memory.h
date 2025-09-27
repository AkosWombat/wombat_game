#ifndef WASP_H
#error This module depends on wasp.h
#endif

#ifndef WASP_MEMORY_H
#define WASP_MEMORY_H

// TODO: Probably support arena chaining so we can grow on systems without a MMU, like WASM

/*
  CONSTANTS
*/

/*
  TYPES
*/

enum memory_arena_flag
{
    // MemoryArenaFlag_DecommitOnReset = 1 << 0, // TODO: Possibly do this
    
    MemoryArenaFlag_AllowFailedPush    = 1 << 1,
    MemoryArenaFlag_AllowFailedCreate  = 1 << 2,
    MemoryArenaFlag_OnlyCommitAsNeeded = 1 << 3,
};

struct memory_arena
{
    u8 *Memory_; // TODO: Remove _
    umm Reserved;
    umm Committed;
    // TODO: Have some baseline committed here, if we want to decommit on reset
    umm Used;
    
    flag32(memory_arena_flag) Flags;
    
    b8 Released;
    b8 CantReset; // NOTE: In case that we have pushed an arena, this could cause holes of decommitted memory
};

struct memory_temporary
{
    memory_arena *Arena;
    umm Used;
};

/*
  GLOBALS
*/

/*
  FUNCTIONS
*/

internal inline void MemorySet(void *DestinationInit, u8 Value, umm Size);
internal inline void MemoryCopy(void *DestinationInit, void *SourceInit, umm Size);

#define ZeroStruct(Pointer) MemorySet(Pointer, 0, SizeOf(*Pointer))
#define ZeroArray(Array) MemorySet(Array, 0, SizeOf(Array))
#define ZeroBuffer(Buffer) MemorySet((Buffer).Data, 0, (Buffer).Size)

// NOTE: These reserve/release functions wont do any syscalls if a size of zero is passed in, to avoid having to check for zero allocations (example FileReadEntire)
internal void *MemoryReserveAndCommit(umm Size);
internal void *MemoryReserve(umm Size);
internal void MemoryRelease(void *Memory, umm Size);

internal inline buffer MemoryReserveAndCommitBuffer(umm Size);
internal inline buffer MemoryReserveBuffer(umm Size);
internal inline void MemoryReleaseBuffer(buffer *Buffer);

// TODO: Figure out if we want to accept zero size like in the reserve/release or if we should assert (we assert for now)
internal b32 MemoryCommit(void *Memory, umm Size);
internal void MemoryDecommit(void *Memory, umm Size);

internal memory_arena MemoryArenaFromBuffer(void *Data, umm Size, flag32(memory_arena_flag) Flags);

internal memory_arena MemoryArenaCreate(umm Reserved, umm Committed, flag32(memory_arena_flag) Flags);
internal void MemoryArenaDestroy(memory_arena *Arena);

// TODO: Possibly always return "valid" memory for these pushes, just have the same zeroed out page repeatedly mapped, and log to error system on failure?

internal u8 *MemoryArenaPush(memory_arena *Arena, umm Size, umm Alignment);
internal memory_arena MemoryArenaPushArena(memory_arena *Arena, umm Reserved, umm Committed, flag32(memory_arena_flag) Flags);
internal buffer MemoryArenaPushBufferOfSize(memory_arena *Arena, umm Size);
internal buffer MemoryArenaPushBuffer(memory_arena *Arena, buffer Buffer);
internal buffer MemoryArenaPushBuffers(memory_arena *Arena, buffer A, buffer B);
internal b32 MemoryArenaPushBufferInPlace(memory_arena *Arena, buffer *Buffer);

#define MemoryArenaPushArray(Arena, Type, Aligned, Count) ((Type *)MemoryArenaPush(Arena, (Count) * SizeOf(Type), (Aligned) ? AlignOf(Type) : 1))
#define MemoryArenaPushType(Arena, Type, Aligned) MemoryArenaPushArray(Arena, Type, Aligned, 1)

// TODO: Array begin/end API

internal void MemoryArenaReset(memory_arena *Arena);

internal inline memory_temporary MemoryTemporaryBegin(memory_arena *Arena);
internal inline void MemoryTemporaryEnd(memory_temporary Temp); // TODO: Probably take in a pointer instead and invalidate the temp, it's maybe a bit weird this way? If we keep it like this at least call it reset instead, that makes more sense

#define MemoryScratchBegin(Conflict) MemoryTemporaryBegin(MemoryScratchGet(Conflict))
#define MemoryScratchEnd(Scratch) MemoryTemporaryEnd(Scratch)

/*
  IMPLEMENTATION
*/

internal inline void
MemorySet(void *DestinationInit, u8 Value, umm Size) // TODO: Make sure that this cant be optimized out (for zeroing)
{
    u8 *Destination = (u8 *)DestinationInit;
    
    while(Size--)
    {
        *Destination++ = Value;
    }
}

internal inline void
MemoryCopy(void *DestinationInit, void *SourceInit, umm Size)
{
    u8 *Destination = (u8 *)DestinationInit;
    u8 *Source = (u8 *)SourceInit;
    
    while(Size--)
    {
        *Destination++ = *Source++;
    }
}

internal inline buffer
MemoryReserveAndCommitBuffer(umm Size)
{
    buffer Result = {};
    
    void *Memory = MemoryReserveAndCommit(Size);
    if(Memory)
    {
        Result.Data = (u8 *)Memory;
        Result.Size = Size;
    }
    
    return(Result);
}

internal inline buffer
MemoryReserveBuffer(umm Size)
{
    buffer Result = {};
    
    void *Memory = MemoryReserve(Size);
    if(Memory)
    {
        Result.Data = (u8 *)Memory;
        Result.Size = Size;
    }
    
    return(Result);
}

internal inline void
MemoryReleaseBuffer(buffer *Buffer)
{
    Assert(Buffer);
    
    MemoryRelease(Buffer->Data, Buffer->Size);
    
    ZeroStruct(Buffer);
}

internal memory_arena
MemoryArenaFromBuffer(void *Data, umm Size, flag32(memory_arena_flag) Flags)
{
    memory_arena Result = {};
    
    Result.Memory_ = (u8 *)Data;
    Result.Reserved = Size;
    Result.Committed = Size;
    Result.Flags = Flags;
    
    return(Result);
}

internal memory_arena
MemoryArenaCreate(umm Reserved, umm Committed, flag32(memory_arena_flag) Flags)
{
    if(!Reserved)
    {
        Reserved = Committed;
    }
    
    Assert(Reserved >= Committed);
    
    umm PageSize = GetPageSize();
    Assert(Reserved >= PageSize);
    Assert(IsAligned(Reserved, PageSize));
    Assert(IsAligned(Committed, PageSize));
    
    memory_arena Result = {};
    
    void *Memory = MemoryReserve(Reserved);
    if(Memory)
    {
        if(!Committed || MemoryCommit(Memory, Committed))
        {
            Result.Memory_ = (u8 *)Memory;
            Result.Reserved = Reserved;
            Result.Committed = Committed;
            Result.Flags = Flags;
        }
        else
        {
            MemoryRelease(Memory, Reserved);
        }
    }
    
    if(!Result.Reserved && !(Result.Flags & MemoryArenaFlag_AllowFailedCreate))
    {
        Assert(0); // TODO: Some better more specific crash function, but don't use the logging system, it relies on this function to begin with!
    }
    
    return(Result);
}

internal void
MemoryArenaDestroy(memory_arena *Arena)
{
    Assert(Arena);
    Assert(!Arena->Released);
    
    if(Arena->Reserved)
    {
        MemoryRelease(Arena->Memory_, Arena->Reserved);
    }
    
    ZeroStruct(Arena);
    Arena->Released = 1;
}

internal u8 *
MemoryArenaPush(memory_arena *Arena, umm Size, umm Alignment)
{
    Assert(Arena);
    Assert(!Arena->Released);
    
    Assert(Alignment);
    Assert(IsPowerOfTwo(Alignment)); // TODO: Probably remove this requirement... It's due to the AlignUp macro requiring power of two for now...
    
    u8 *Result = 0;
    
    if(Size)
    {
        umm Used = AlignUp(Arena->Used, Alignment);
        umm NewUsed = Used + Size;
        
        if(NewUsed <= Arena->Reserved)
        {
            if(NewUsed > Arena->Committed)
            {
                umm PageSize = GetPageSize();
                Assert(Arena->Reserved >= PageSize);
                Assert(IsAligned(Arena->Reserved, PageSize));
                Assert(IsAligned(Arena->Committed, PageSize));
                
                umm NewCommitted;
                
                if(Arena->Flags & MemoryArenaFlag_OnlyCommitAsNeeded)
                {
                    NewCommitted = AlignUp(NewUsed, PageSize);
                }
                else
                {
                    NewCommitted = AlignUpToPowerOfTwo(NewUsed);
                }
                
                NewCommitted = Clamp(NewCommitted, PageSize, Arena->Reserved);
                
                if(MemoryCommit(Arena->Memory_, NewCommitted)) // TODO: For speed we might want to only pass in the pages that are relevant?
                {
                    Arena->Committed = NewCommitted;
                }
            }
            
            if(NewUsed <= Arena->Committed)
            {
                Result = Arena->Memory_ + Used;
                Arena->Used = NewUsed;
            }
        }
        
        if(!Result)
        {
            if(Arena->Flags & MemoryArenaFlag_AllowFailedPush)
            {
                Logf(LogSeverity_Error, "Failed to push %llu bytes", Size); // TODO: More specific info
            }
            else
            {
                Assert(0); // TODO: Some better more specific crash function
            }
        }
    }
    
    return(Result);
}

internal memory_arena
MemoryArenaPushArena(memory_arena *Arena, umm Reserved, umm Committed, flag32(memory_arena_flag) Flags)
{
    Assert(Arena);
    Assert(!Arena->Released);
    
    if(!Reserved)
    {
        Reserved = Committed;
    }
    
    Assert(Reserved >= Committed);
    
    umm PageSize = GetPageSize();
    Assert(Reserved >= PageSize);
    Assert(IsAligned(Reserved, PageSize));
    Assert(IsAligned(Committed, PageSize));
    
    memory_arena Result = {};
    
    umm NewUsed = Arena->Committed + Reserved;
    if(NewUsed <= Arena->Reserved)
    {
        u8 *Memory = Arena->Memory_ + Arena->Committed;
        if(!Committed || MemoryCommit(Memory, Committed))
        {
            Result.Memory_ = (u8 *)Memory;
            Result.Reserved = Reserved;
            Result.Committed = Committed;
            Result.Flags = Flags;
            
            Arena->Committed = NewUsed;
            Arena->Used = NewUsed;
            Arena->CantReset = 1;
        }
    }
    
    return(Result);
}

internal buffer
MemoryArenaPushBufferOfSize(memory_arena *Arena, umm Size)
{
    buffer Result = {};
    
    u8 *Memory = MemoryArenaPush(Arena, Size, 1);
    if(Memory)
    {
        Result.Data = Memory;
        Result.Size = Size;
    }
    
    return(Result);
}

internal buffer
MemoryArenaPushBuffer(memory_arena *Arena, buffer Buffer)
{
    buffer Result = MemoryArenaPushBufferOfSize(Arena, Buffer.Size);
    
    if(IsValid(Result))
    {
        MemoryCopy(Result.Data, Buffer.Data, Buffer.Size);
    }
    
    return(Result);
}

internal buffer
MemoryArenaPushBuffers(memory_arena *Arena, buffer A, buffer B)
{
    buffer Result = {};
    
    u8 *Memory = MemoryArenaPush(Arena, A.Size + B.Size, 1);
    if(Memory)
    {
        Result.Data = Memory;
        Result.Size = A.Size + B.Size;
        
        MemoryCopy(Result.Data, A.Data, A.Size);
        MemoryCopy(Result.Data + A.Size, B.Data, B.Size);
    }
    
    return(Result);
}

internal b32
MemoryArenaPushBufferInPlace(memory_arena *Arena, buffer *Buffer)
{
    Assert(Arena);
    Assert(Buffer);
    
    b32 Result = 1;
    
    if(IsValid(*Buffer))
    {
        *Buffer = MemoryArenaPushBuffer(Arena, *Buffer);
        Result = IsValid(*Buffer);
    }
    
    return(Result);
}

internal void
MemoryArenaReset(memory_arena *Arena)
{
    Assert(Arena);
    Assert(!Arena->Released);
    Assert(!Arena->CantReset);
    
    Arena->Used = 0; // TODO: Possibly allow decommitting on reset with a flag?
}

internal inline memory_temporary
MemoryTemporaryBegin(memory_arena *Arena)
{
    Assert(Arena);
    Assert(!Arena->Released);
    
    memory_temporary Result;
    Result.Arena = Arena;
    Result.Used = Arena->Used;
    return(Result);
}

internal inline void
MemoryTemporaryEnd(memory_temporary Temp)
{
    // NOTE: Obviously this wont save you from a new arena relacing the old one, but we don't care "enough" for that case to have some sort of incremental ID attached
    Assert(Temp.Arena);
    Assert(!Temp.Arena->Released);
    
    Temp.Arena->Used = Temp.Used;
}

internal memory_arena *MemoryArenasGet(void); // NOTE: Implemented by platform layer

internal memory_arena *
MemoryScratchGet(memory_arena *Conflict)
{
    memory_arena *Result = 0;
    memory_arena *Arenas = MemoryArenasGet();
    
    for(u32 Index = 0;
        Index < 2;
        Index++)
    {
        memory_arena *Arena = Arenas + Index;
        if(Arena != Conflict)
        {
            Result = Arena;
            break;
        }
    }
    
    Assert(Result);
    return(Result);
}

internal void
MemoryInit(void)
{
    memory_arena *Arenas = MemoryArenasGet();
    
    for(u32 Index = 0;
        Index < 2;
        Index++)
    {
        Arenas[Index] = MemoryArenaCreate(MB(2), KB(32), 0); // TODO: Some way to change this
    }
}

#endif // WASP_MEMORY_H