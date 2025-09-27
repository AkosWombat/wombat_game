/* TODO:

  - Use wide win32 API calls

*/

#ifndef WASP_H
#error This module depends on wasp.h
#endif

#ifndef WASP_WIN32_H
#define WASP_WIN32_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "kernel32.lib")

global volatile LONG GlobalWin32AssertLock;

internal void
AssertHandler(char *Expression, char *File, int LineNumber, char *Function)
{
    // NOTE: No thread identifier will ever be 0
    
    LONG ThreadID = (LONG)GetCurrentThreadId();
    LONG OwningID = InterlockedCompareExchange(&GlobalWin32AssertLock, ThreadID, 0);
    if(OwningID)
    {
        if(OwningID != ThreadID)
        {
            ExitThread(2); // NOTE: Someone else asserted before us, so just exit our thread
        }
        else
        {
            // NOTE: Deadlock, we asserted somewhere in the assert callback, so just exit
        }
    }
    else
    {
        AssertCallback(Expression, File, LineNumber, Function);
    }
    
    DebugBreak();
    ExitProcess(1);
}

internal umm
GetPageSize(void)
{
    SYSTEM_INFO Info;
    GetSystemInfo(&Info);
    
    umm Result = (umm)Info.dwPageSize;
    return(Result);
}

internal void
StandardOutput(u8 *Buffer, umm Size)
{
    DWORD Ignored;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), (char *)Buffer, (DWORD)Size, &Ignored, 0);
}

internal void
StandardError(u8 *Buffer, umm Size)
{
    DWORD Ignored;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), (char *)Buffer, (DWORD)Size, &Ignored, 0);
}

#ifndef WASP_LOG_H

global log_entry GlobalLogEntrySentinel;

internal void Logf(enum32(log_severity) Severity, char *Format, ...) {(void)(Format);(void)(Severity);}
internal void LogRegionBegin(void) {}
internal void LogRegionEnd(void) {}
internal void LogRegionClear(void) {}

internal inline log_entry *LogRegionGetEntries(void)
{
    log_entry *Result = &GlobalLogEntrySentinel;
    
    if(!Result->Next)
    {
        DoublyLinkedListInit(Result);
    }
    
    return(Result);
}

internal inline enum32(log_severity)
LogRegionGetHighestSeverity(void)
{
    return(0);
}

#endif // WASP_LOG_H

#ifdef WASP_CRYPTO_H
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

internal void
CryptoRandomFill(void *Data, umm Size)
{
    Assert(Data);
    Assert(Size);
    Assert(Size < MaxU32); // TODO: Assert min size across platforms
    
    NTSTATUS Status = BCryptGenRandom(0, (PUCHAR)Data, (ULONG)Size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    Assert(Status == 0); // NOTE: STATUS_SUCCESS
}

internal void
CryptoSHA1Fill(void *SourceData, umm SourceSize, void *DestinationData, umm DestinationSize)
{
    Assert(SourceData);
    Assert(SourceSize);
    Assert(SourceSize < MaxU32);
    
    Assert(DestinationData);
    Assert(DestinationSize >= CryptoDigestSizeSHA1);
    
    NTSTATUS Status = BCryptHash(BCRYPT_SHA1_ALG_HANDLE, 0, 0, (PUCHAR)SourceData, (ULONG)SourceSize, (PUCHAR)DestinationData, (ULONG)DestinationSize);
    Assert(Status == 0);
}

internal void
CryptoSHA256Fill(void *SourceData, umm SourceSize, void *DestinationData, umm DestinationSize)
{
    Assert(SourceData);
    Assert(SourceSize);
    Assert(SourceSize < MaxU32);
    
    Assert(DestinationData);
    Assert(DestinationSize >= CryptoDigestSizeSHA256);
    
    NTSTATUS Status = BCryptHash(BCRYPT_SHA256_ALG_HANDLE, 0, 0, (PUCHAR)SourceData, (ULONG)SourceSize, (PUCHAR)DestinationData, (ULONG)DestinationSize);
    Assert(Status == 0);
}

#endif // WASP_CRYPTO_H

#ifdef WASP_THREAD_H

#include <intrin.h>

#define WriteBarrier() _WriteBarrier()
#define ReadBarrier() _ReadBarrier()

internal inline void
ThreadYield(void)
{
    _mm_pause();
}

internal inline u32
ThreadAtomicLoadU32(volatile u32 *Source)
{
    return(*Source);
}

internal inline u64
ThreadAtomicLoadU64(volatile u64 *Source)
{
    return(*Source);
}

internal inline void
ThreadAtomicStoreU32(volatile u32 *Destination, u32 Value)
{
    WriteBarrier();
    *Destination = Value;
}

internal inline void
ThreadAtomicStoreU64(volatile u64 *Destination, u64 Value)
{
    WriteBarrier();
    *Destination = Value;
}

internal inline u32
ThreadAtomicAddU32(volatile u32 *Destination, u32 Value)
{
    u32 Result = InterlockedAdd((volatile LONG *)Destination, (LONG)Value) - Value;
    return(Result);
}

internal inline u64
ThreadAtomicAddU64(volatile u64 *Destination, u64 Value)
{
    u64 Result = InterlockedAdd64((volatile LONG64 *)Destination, (LONG64)Value) - Value;
    return(Result);
}

internal u64
ThreadCreate(umm StackSize, thread_proc *ThreadProc, void *Parameter)
{
    if(!StackSize)
    {
        StackSize = MB(1); // TODO: Figure out how to get the stack size..., oh actually we can from the PE header I believe, or something like that
    }
    
    umm PageSize = GetPageSize();
    StackSize = AlignUp(StackSize, PageSize);
    
    DWORD ThreadID;
    HANDLE Handle = CreateThread(0, 0, (PTHREAD_START_ROUTINE)ThreadProc, Parameter, 0, &ThreadID);
    CloseHandle(Handle);
    
    u64 Result = (u64)ThreadID;
    return(Result);
}

internal u64
ThreadGetID(void)
{
    u64 Result = (u64)GetCurrentThreadId();
    return(Result);
}

internal u32
ThreadGetCPUCount(void)
{
    SYSTEM_INFO Info;
    GetSystemInfo(&Info);
    
    u32 Result = (u32)Info.dwNumberOfProcessors;
    return(Result);
}

#endif // WASP_THREAD_H

#ifdef WASP_MEMORY_H

internal void *
MemoryReserveAndCommit(umm Size)
{
    void *Result = 0;
    
    if(Size)
    {
        Result = VirtualAlloc(0, Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    }
    
    return(Result);
}

internal void *
MemoryReserve(umm Size)
{
    void *Result = 0;
    
    if(Size)
    {
        Result = VirtualAlloc(0, Size, MEM_RESERVE, PAGE_NOACCESS);
    }
    
    return(Result);
}

internal void
MemoryRelease(void *Memory, umm Size)
{
    if(Size)
    {
        Assert(Memory);
        Assert(VirtualFree(Memory, 0, MEM_RELEASE));
    }
}

internal b32
MemoryCommit(void *Memory, umm Size)
{
    Assert(Memory);
    Assert(Size);
    
    void *Test = VirtualAlloc(Memory, Size, MEM_COMMIT, PAGE_READWRITE);
    
    b32 Result = Test == Memory;
    if(!Result)
    {
        Assert(!Test);
    }
    
    return(Result);
}

internal void
MemoryDecommit(void *Memory, umm Size)
{
    Assert(Memory);
    Assert(Size);
    
    Assert(VirtualFree(Memory, Size, MEM_DECOMMIT));
}

#endif // WASP_MEMORY_H

#ifdef WASP_FILE_H

// TODO: Use wide versions of the win32 APIs and also prefix with the size thing if we can...

/*
internal b32
Win32MakeFilePath(u16 *Destination, s32 DestinationCount, char *Source)
{
    Assert(Destination);
    Assert(DestinationCount);
    Assert(Source);

    b32 Result = 0;

    s32 Count = MultiByteToWideChar(CP_UTF8, 0, Source, -1, 0, 0);
    if(Count <= DestinationCount)
    {
        if(Count == MultiByteToWideChar(CP_UTF8, 0, Source, Count, (LPWSTR)Destination, Count))
        {
            for(s32 Index = 0;
                Index < Count-1;
                Index++)
            {
                // TODO: Is this even correct with utf8 in mind?
                if(Destination[Index] == '/')
                {
                    Destination[Index] = '\\';
                }
            }

            Result = 1;
        }
    }

    return(Result);
}

internal HANDLE
Win32FileOpen(char *FileName, b32 Write)
{
    HANDLE Result = INVALID_HANDLE_VALUE;

    u16 Buffer[1024];
    if(Win32MakeFilePath(Buffer, ArrayCount(Buffer), FileName))
    {
        Result = CreateFileW((LPWSTR)Buffer,
                             Write ? GENERIC_WRITE : GENERIC_READ,
                             Write ? 0 : FILE_SHARE_READ, 0,
                             Write ? CREATE_ALWAYS : OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL, 0);
    }

    return(Result);
}
*/

internal void
Win32CopyFindData(WIN32_FIND_DATA *Data, directory_entry *Entry)
{
    ZeroStruct(Entry);
    
    Entry->NameSize = StringLengthZ(Data->cFileName);
    MemoryCopy(Entry->NameData, Data->cFileName, Entry->NameSize);
    
    if(Data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        Entry->Flags |= DirectoryEntryFlag_Directory;
    }
}

internal inline b32
IsValid(file_handle_ Handle)
{
    return((HANDLE)Handle.Value != INVALID_HANDLE_VALUE);
}

internal inline b32
IsValid(directory_handle Handle)
{
    return((HANDLE)Handle.Value != INVALID_HANDLE_VALUE);
}

// TODO: Figure out if we should use wide paths, convert backslashes, prefix paths, etc...

internal file_handle_
FileOpen(char *FileName, b32 ForWriting)
{
    HANDLE Handle = CreateFileA(FileName,
                                ForWriting ? GENERIC_WRITE : GENERIC_READ,
                                ForWriting ? 0 : FILE_SHARE_READ, 0,
                                ForWriting ? CREATE_ALWAYS : OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, 0);
    
    file_handle_ Result = {(u64)Handle};
    return(Result);
}

internal void
FileClose(file_handle_ *Handle)
{
    if(IsValid(*Handle))
    {
        Assert(CloseHandle((HANDLE)Handle->Value));
        Handle->Value = (u64)INVALID_HANDLE_VALUE;
    }
}

internal b32
FileRead(file_handle_ Handle, buffer Buffer)
{
    b32 Result = 0;
    
    if(IsValid(Handle) &&
       IsValid(Buffer))
    {
        Assert(Buffer.Size <= MaxU32); // TODO: Allow for 64-bit reads
        
        DWORD BytesRead;
        if(ReadFile((HANDLE)Handle.Value, Buffer.Data, (u32)Buffer.Size, &BytesRead, 0))
        {
            Assert(BytesRead == (DWORD)Buffer.Size);
            Result = 1;
        }
    }
    
    return(Result);
}

internal b32
FileWrite(file_handle_ Handle, buffer Buffer)
{
    b32 Result = 0;
    
    if(IsValid(Handle) &&
       IsValid(Buffer))
    {
        Assert(Buffer.Size <= MaxU32); // TODO: Allow for 64-bit writes
        
        DWORD BytesWritten;
        if(WriteFile((HANDLE)Handle.Value, Buffer.Data, (u32)Buffer.Size, &BytesWritten, 0))
        {
            Assert(BytesWritten == (DWORD)Buffer.Size);
            Result = 1;
        }
    }
    
    return(Result);
}

internal u64
FileGetSizeU64(file_handle_ Handle)
{
    u64 Result = 0;
    
    if(IsValid(Handle))
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx((HANDLE)Handle.Value, &FileSize))
        {
            Result = FileSize.QuadPart;
        }
    }
    
    return(Result);
}

internal void
DirectoryClose(directory_handle *Handle) // TODO: Try to get rid of this if we can somehow...
{
    if(IsValid(*Handle))
    {
        Assert(FindClose((HANDLE)Handle->Value));
        Handle->Value = (u64)INVALID_HANDLE_VALUE;
    }
}

internal directory_iterator
DirectoryIterate(char *FileName)
{
    directory_iterator Result;
    
    WIN32_FIND_DATA Data;
    Result.Handle.Value = (u64)FindFirstFileA(FileName, &Data); // TODO: Use wide version of these APIs
    
    if(IsValid(Result.Handle))
    {
        Win32CopyFindData(&Data, &Result.Entry);
    }
    
    return(Result);
}

internal void
Advance(directory_iterator *Iterator)
{
    if(IsValid(Iterator->Handle))
    {
        WIN32_FIND_DATA Data;
        if(FindNextFileA((HANDLE)Iterator->Handle.Value, &Data))
        {
            Win32CopyFindData(&Data, &Iterator->Entry);
        }
        else
        {
            Assert(GetLastError() == ERROR_NO_MORE_FILES);
            DirectoryClose(&Iterator->Handle);
        }
    }
}

#endif // WASP_FILE_H

#ifdef WASP_DNS_H
#include <windns.h>

#pragma comment(lib, "dnsapi.lib")

internal dns_query_result
DNSQuery(memory_arena *Arena, char *Name, flag32(dns_record_flag) Flags)
{
    dns_query_result Result = {};
    
    while(Flags)
    {
        WORD Type = 0;
        if(Flags & DNSRecordFlag_A)
        {
            Flags &= ~DNSRecordFlag_A;
            Type = DNS_TYPE_A;
        }
        else if(Flags & DNSRecordFlag_AAAA)
        {
            Flags &= ~DNSRecordFlag_AAAA;
            Type = DNS_TYPE_AAAA;
        }
        else
        {
            InvalidCodePath; // TODO: Assert with message
            // break;
        }
        
        // TODO: Use wide/async version, or just make our own implementation with the IO module if we really need performance here...
        DNS_RECORDA *Win32Records;
        if(DnsQuery_A(Name, Type, 0 /*DNS_QUERY_BYPASS_CACHE*/, 0, &Win32Records, 0) == DNS_RCODE_NOERROR)
        {
            for(DNS_RECORDA *Win32Record = Win32Records;
                Win32Record;
                Win32Record = Win32Record->pNext)
            {
                dns_record *Record = MemoryArenaPushType(Arena, dns_record, 1);
                if(Record)
                {
                    // TODO: Probably a use for an array memory API? Or is that not really needed...
                    if(!Result.Records)
                    {
                        Result.Records = Record;
                    }
                    
                    switch(Type)
                    {
                        case DNS_TYPE_A:
                        {
                            Record->Flags = DNSRecordFlag_A;
                            Record->A.Address = Win32Record->Data.A.IpAddress;
                        } break;
                        
                        case DNS_TYPE_AAAA:
                        {
                            Record->Flags = DNSRecordFlag_AAAA;
                            Record->AAAA.Address = *(u128 *)&Win32Record->Data.AAAA.Ip6Address;
                        } break;
                        
                        default: InvalidCase;
                    }
                    
                    Result.RecordsCount++;
                }
                else
                {
                    Result.NumberOfRecordsSkipped++;
                }
            }
            
            DnsRecordListFree(Win32Records, DnsFreeRecordList);
        }
    }
    
    return(Result);
}

#endif // WASP_DNS_H

/*
#ifdef WASP_IO_H_OLD

#include <winsock2.h>
#include <mswsock.h>

#pragma comment(lib, "ws2_32.lib")

// TODO: Remove operations, merge into endpoints instead

struct win32_operation_list_entry
{
    OVERLAPPED Overlapped;
    
    win32_operation_list_entry *Next;
    win32_operation_list_entry *Prev;
    
    SOCKET Socket; // NOTE: If this is not INVALID_SOCKET, then it is the "return value"
    
    DWORD Error; // NOTE: If this is not zero, then the operation failed, and this is the win32 code
    
    u64 UserData;
};

struct win32_operation_list
{
    win32_operation_list_entry Sentinel;
    win32_operation_list_entry *Entries;
    u32 EntriesCount;
};

struct win32_io_endpoint_table_entry
{
    win32_io_endpoint_table_entry *Next;
    win32_io_endpoint_table_entry *Prev;
    
    SOCKET Socket;
    
    // TODO:
    // b8 QueuedClose; // NOTE: Just used to assert that we never call anything after a close has been queued, thats a illegal use after free!
};

struct win32_io_endpoint_table
{
    win32_io_endpoint_table_entry Sentinel;
    win32_io_endpoint_table_entry *Entries;
    u32 EntriesCount;
};

struct win32_io_accept_address
{
    SOCKADDR_IN Address;
    u8 Reserved[16];
};

struct win32_io_accept_data
{
    win32_io_accept_address Local;
    win32_io_accept_address Remote;
};

StaticAssert(SizeOf(win32_io_accept_data) <= SizeOf(io_accept_data));

struct win32_io_connect_data
{
    SOCKADDR_IN Address;
};

StaticAssert(SizeOf(win32_io_connect_data) <= SizeOf(io_connect_data));

struct io
{
    HANDLE CompletionPort;
    
    LPFN_ACCEPTEX AcceptEx;
    LPFN_CONNECTEX ConnectEx;
    LPFN_DISCONNECTEX DisconnectEx;
    
    OVERLAPPED_ENTRY *Entries;
    u32 EntriesCount;
    
    u32 SubmissionsCount;
    
    win32_operation_list OperationList;
    
    win32_io_endpoint_table EndpointTable;
};

internal win32_operation_list_entry *
Win32GetOperationEntry(io *IO)
{
    win32_operation_list_entry *Result = IO->OperationList.Sentinel.Next;
    
    Assert(!DoublyLinkedListIsEmpty(&IO->OperationList.Sentinel)); // TODO
    DoublyLinkedListRemove(Result);
    ZeroStruct(&Result->Overlapped);
    
    return(Result);
}

internal void
Win32PostOperationEntry(io *IO, win32_operation_list_entry *Win32Operation, DWORD Error)
{
    Win32Operation->Error = Error;
    
    if(PostQueuedCompletionStatus(IO->CompletionPort, 0, 0, &Win32Operation->Overlapped) == FALSE)
    {
        Assert(0); // TODO
    }
}

internal inline win32_io_endpoint_table_entry *
Win32GetEndpointEntry(io *IO)
{
    win32_io_endpoint_table_entry *Result = IO->EndpointTable.Sentinel.Next;
    
    Assert(!DoublyLinkedListIsEmpty(&IO->EndpointTable.Sentinel)); // TODO: Don't assert?
    DoublyLinkedListRemove(Result);
    
    return(Result);
}

internal inline win32_io_endpoint_table_entry *
Win32GetEndpointEntryFromIndex(io *IO, u32 EndpointIndex)
{
    Assert(EndpointIndex);
    
    win32_io_endpoint_table_entry *Result = IO->EndpointTable.Entries + (EndpointIndex - 1);
    return(Result);
}

internal inline u32
Win32GetEndpointEntryIndex(io *IO, win32_io_endpoint_table_entry *EndpointEntry)
{
    u32 Result = (u32)(EndpointEntry - IO->EndpointTable.Entries) + 1;
    return(Result);
}

internal inline SOCKET
Win32GetSocketFromEndpointIndex(io *IO, u32 EndpointIndex)
{
    win32_io_endpoint_table_entry *EndpointEntry = Win32GetEndpointEntryFromIndex(IO, EndpointIndex);
    SOCKET Result = EndpointEntry->Socket;
    return(Result);
}

internal inline void
Win32ReleaseEndpointEntry(io *IO, win32_io_endpoint_table_entry *EndpointEntry)
{
    Assert(EndpointEntry);
    
    DoublyLinkedListInsertAfter(&IO->EndpointTable.Sentinel, EndpointEntry);
}

internal io *
IOCreate(u32 Entries, u32 EndpointEntriesCount)
{
    io *Result = 0;
    
    u32 OperationListEntriesCount = Entries * 4; // TODO: This would probably grow but whatever...
    u32 OverlappedEntriesCount = Entries * 2;
    
    umm MemorySize = SizeOf(*Result);
    
    umm OperationListEntriesOffset = MemorySize;
    MemorySize += SizeOf(*Result->OperationList.Entries) * OperationListEntriesCount;
    
    umm OverlappedEntriesOffset = MemorySize;
    MemorySize += SizeOf(*Result->Entries) * OverlappedEntriesCount;
    
    umm EndpointEntriesOffset = MemorySize;
    MemorySize += SizeOf(*Result->EndpointTable.Entries) * EndpointEntriesCount;
    
    memory_allocation Memory = MemoryAllocate(MemorySize, 0);
    if(Memory.Memory)
    {
        Result = (io *)Memory.Memory;
        
        DoublyLinkedListInit(&Result->OperationList.Sentinel);
        Result->OperationList.Entries = (win32_operation_list_entry *)(Memory.Memory + OperationListEntriesOffset);
        Result->OperationList.EntriesCount = OperationListEntriesCount;
        
        for(u32 OperationIndex = 0;
            OperationIndex < OperationListEntriesCount;
            OperationIndex++)
        {
            win32_operation_list_entry *Operation = Result->OperationList.Entries + OperationIndex;
            DoublyLinkedListInsertAfter(&Result->OperationList.Sentinel, Operation);
        }
        
        Result->Entries = (OVERLAPPED_ENTRY *)(Memory.Memory + OverlappedEntriesOffset);
        Result->EntriesCount = OverlappedEntriesCount;
        
        DoublyLinkedListInit(&Result->EndpointTable.Sentinel);
        Result->EndpointTable.Entries = (win32_io_endpoint_table_entry *)(Memory.Memory + EndpointEntriesOffset);
        Result->EndpointTable.EntriesCount = EndpointEntriesCount;
        
        for(u32 RealEndpointIndex = 0;
            RealEndpointIndex < Result->EndpointTable.EntriesCount;
            RealEndpointIndex++)
        {
            win32_io_endpoint_table_entry *Endpoint = Result->EndpointTable.Entries + RealEndpointIndex;
            DoublyLinkedListInsertAfter(&Result->EndpointTable.Sentinel, Endpoint);
        }
        
        WSADATA Data;
        Assert(WSAStartup(MAKEWORD(2,2), &Data) == 0);
        
        SOCKET WhatWasMicrosoftDoingHere = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
        Assert(WhatWasMicrosoftDoingHere != INVALID_SOCKET);
        
        GUID GuidAcceptEx = WSAID_ACCEPTEX;
        GUID GuidConnectEx = WSAID_CONNECTEX;
        GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
        
        DWORD Ignored;
        b32 Ok = (WSAIoctl(WhatWasMicrosoftDoingHere,
                           SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &GuidAcceptEx, SizeOf(GUID),
                           &Result->AcceptEx, SizeOf(Result->AcceptEx),
                           &Ignored, 0, 0) == 0 &&
                  WSAIoctl(WhatWasMicrosoftDoingHere,
                           SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &GuidConnectEx, SizeOf(GUID),
                           &Result->ConnectEx, SizeOf(Result->ConnectEx),
                           &Ignored, 0, 0) == 0 &&
                  WSAIoctl(WhatWasMicrosoftDoingHere,
                           SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &GuidDisconnectEx, SizeOf(GUID),
                           &Result->DisconnectEx, SizeOf(Result->DisconnectEx),
                           &Ignored, 0, 0) == 0);
        
        Assert(Ok);
        
        closesocket(WhatWasMicrosoftDoingHere);
        
        HANDLE CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
        Assert(CompletionPort);
        
        Result->CompletionPort = CompletionPort;
    }
    
    return(Result);
}

internal u32
IOBegin(io *IO)
{
    u32 Result = 0;
    
    ULONG NumEntriesRemoved;
    if(GetQueuedCompletionStatusEx(IO->CompletionPort, IO->Entries, IO->EntriesCount, &NumEntriesRemoved, 0, FALSE))
    {
        Result = (u32)NumEntriesRemoved;
    }
    
    return(Result);
}

internal u32
IOEnd(io *IO)
{
    u32 Result = IO->SubmissionsCount;
    IO->SubmissionsCount = 0;
    return(Result);
}

internal io_completion
IOCompletion(io *IO, u32 Index)
{
    io_completion Result = {};
    
    OVERLAPPED_ENTRY *OverlappedEntry = IO->Entries + Index;
    OVERLAPPED *Overlapped = OverlappedEntry->lpOverlapped;
    
    win32_operation_list_entry *Win32OperationEntry = (win32_operation_list_entry *)Overlapped;
    DoublyLinkedListInsertAfter(&IO->OperationList.Sentinel, Win32OperationEntry);
    
    Result.OperationData = IOOperationDataUnpack(Win32OperationEntry->UserData);
    
    DWORD NumberOfBytesTransferred;
    if(!Win32OperationEntry->Error && GetOverlappedResult(0, Overlapped, &NumberOfBytesTransferred, FALSE))
    {
        if(Win32OperationEntry->Socket == INVALID_SOCKET)
        {
            Result.Value = (u64)NumberOfBytesTransferred;
        }
        else
        {
            Result.Value = (u64)Win32OperationEntry->Socket; // TODO: Check what the value of NumberOfBytesTransferred would be here, possibly the socket?
        }
        
        Result.Success = 1;
    }
    else
    {
        if(Win32OperationEntry->Error)
        {
            Result.Value = (u64)Win32OperationEntry->Error;
            Win32OperationEntry->Error = 0;
        }
        else
        {
            Result.Value = (u64)GetLastError();
        }
        
        Result.Success = 0;
        
        if(Win32OperationEntry->Socket != INVALID_SOCKET)
        {
            int Status = closesocket(Win32OperationEntry->Socket);
            Assert(Status == 0);
        }
    }
    
    if(Result.OperationData.OperationType == IOOperationType_Accept)
    {
        Assert(!Result.OperationData.EndpointIndex);
        
        if(Result.Success)
        {
            win32_io_endpoint_table_entry *EndpointEntry = Win32GetEndpointEntry(IO);
            Assert(EndpointEntry); // TODO: Probably close the connection instead
            
            EndpointEntry->Socket = (SOCKET)Result.Value;
            Result.Value = 0;
            
            Result.OperationData.EndpointIndex = Win32GetEndpointEntryIndex(IO, EndpointEntry);
        }
    }
    else if(Result.OperationData.OperationType == IOOperationType_Close)
    {
        win32_io_endpoint_table_entry *EndpointEntry = Win32GetEndpointEntryFromIndex(IO, Result.OperationData.EndpointIndex);
        Win32ReleaseEndpointEntry(IO, EndpointEntry);
        
        Result.OperationData.EndpointIndex = 0; // TODO: Do we want to give the user the endpoint index here, since a close without any index is sort of useless...
    }
    
    // TODO: We could also handle these errors differently depending on the OperationType
    
    if(!Result.Success)
    {
        switch(Result.Value)
        {
            // TODO
            
            case WSAENOTSOCK:
            {
                Assert(0); // NOTE: This would be very, very bad, risks double close
            } break;
            
            default:
            {
                Assert(0);
            } break;
        }
    }
    
    return(Result);
}

internal void
IORecv(io *IO, u32 UserType, u16 UserData, u32 EndpointIndex, u32 BufferIndex, void *DestinationData, umm DestinationSize)
{
    Assert(DestinationData);
    Assert(DestinationSize <= MaxS32); // NOTE: Unix limits
    
    IO->SubmissionsCount++;
    
    win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);
    
    SOCKET Socket = Win32GetSocketFromEndpointIndex(IO, EndpointIndex);
    
    io_operation_data_unpacked UnpackedData;
    UnpackedData.OperationType = IOOperationType_Recv;
    UnpackedData.UserType = UserType;
    UnpackedData.EndpointIndex = EndpointIndex;
    UnpackedData.BufferIndex = BufferIndex;
    UnpackedData.UserData = UserData;
    
    Win32Operation->Socket = INVALID_SOCKET;
    Win32Operation->UserData = IOOperationDataPack(UnpackedData);
    
    WSABUF Buffer;
    Buffer.len = (ULONG)DestinationSize;
    Buffer.buf = (PCHAR)DestinationData;
    
    DWORD Flags = 0;
    DWORD NumberOfBytesRecvd;
    if(WSARecv(Socket, &Buffer, 1, &NumberOfBytesRecvd, &Flags, &Win32Operation->Overlapped, 0) == SOCKET_ERROR)
    {
        DWORD Error = WSAGetLastError();
        if(Error != WSA_IO_PENDING)
        {
            Win32PostOperationEntry(IO, Win32Operation, Error);
        }
    }
}

internal void
IOSend(io *IO, u32 UserType, u16 UserData, u32 EndpointIndex, u32 BufferIndex, void *SourceData, umm SourceSize)
{
    Assert(SourceData);
    Assert(SourceSize <= MaxS32); // NOTE: Unix limits
    
    IO->SubmissionsCount++;
    
    win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);
    
    SOCKET Socket = Win32GetSocketFromEndpointIndex(IO, EndpointIndex);
    
    io_operation_data_unpacked UnpackedData;
    UnpackedData.OperationType = IOOperationType_Send;
    UnpackedData.UserType = UserType;
    UnpackedData.EndpointIndex = EndpointIndex;
    UnpackedData.BufferIndex = BufferIndex;
    UnpackedData.UserData = UserData;
    
    Win32Operation->UserData = IOOperationDataPack(UnpackedData);
    
    WSABUF Buffer;
    Buffer.len = (ULONG)SourceSize;
    Buffer.buf = (PCHAR)SourceData;
    
    DWORD Flags = 0;
    DWORD NumberOfBytesRecvd;
    if(WSASend(Socket, &Buffer, 1, &NumberOfBytesRecvd, Flags, &Win32Operation->Overlapped, 0) == SOCKET_ERROR)
    {
        DWORD Error = WSAGetLastError();
        if(Error != WSA_IO_PENDING)
        {
            Win32PostOperationEntry(IO, Win32Operation, Error);
        }
    }
}

internal void
IOAccept(io *IO, u32 UserType, u16 UserData, u64 ListnerSocket, io_accept_data *AcceptData)
{
    IO->SubmissionsCount++;
    
    win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);
    
    io_operation_data_unpacked UnpackedData;
    UnpackedData.OperationType = IOOperationType_Accept;
    UnpackedData.UserType = UserType;
    UnpackedData.EndpointIndex = 0;
    UnpackedData.BufferIndex = 0;
    UnpackedData.UserData = UserData;
    
    Win32Operation->Socket = (SOCKET)IOSocket(IO);
    Win32Operation->UserData = IOOperationDataPack(UnpackedData);
    
    win32_io_accept_data *Win32AcceptData = (win32_io_accept_data *)AcceptData;
    
    DWORD BytesReceived;
    if(IO->AcceptEx((SOCKET)ListnerSocket, Win32Operation->Socket,
                    Win32AcceptData, 0,
                    SizeOf(Win32AcceptData->Local),
                    SizeOf(Win32AcceptData->Remote),
                    &BytesReceived, &Win32Operation->Overlapped) == FALSE)
    {
        DWORD Error = WSAGetLastError();
        if(Error != WSA_IO_PENDING)
        {
            Win32PostOperationEntry(IO, Win32Operation, Error);
        }
    }
}

internal void
IOConnect(io *IO, u32 UserType, u16 UserData, u32 EndpointIndex, io_connect_data *ConnectData)
{
    IO->SubmissionsCount++;

    win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);

    io_operation_data_unpacked UnpackedData;
    UnpackedData.OperationType = IOOperationType_Connect;
    UnpackedData.UserType = UserType;
    UnpackedData.UserData = UserData;

    Win32Operation->Socket = (SOCKET)Socket;
    Win32Operation->UserData = IOOperationDataPack(UnpackedData);

    // NOTE: ConnectEx requires the socket to be initially bound

    {
        // NOTE: We bind to any port or address (since both are set to zero)

        SOCKADDR_IN Dummy = {};
        Dummy.sin_family = AF_INET;

        int Status = bind(Win32Operation->Socket, (SOCKADDR *)&Dummy, SizeOf(Dummy));
        Assert(Status == 0);
    }

    // TODO: It seems like we also have to set SO_UPDATE_CONNECT_CONTEXT for the linger to work
    Assert(0);

    win32_io_connect_data *Win32ConnectData = (win32_io_connect_data *)ConnectData;

    if(IO->ConnectEx(Win32Operation->Socket,
        (SOCKADDR *)&Win32ConnectData->Address,
        SizeOf(Win32ConnectData->Address),
        0, 0, 0, &Win32Operation->Overlapped) == FALSE)
    {
        DWORD Error = WSAGetLastError();
        if(Error != WSA_IO_PENDING)
        {
            Win32PostOperationEntry(IO, Win32Operation, Error);
        }
    }
}

internal void
IOShutdown(io *IO, u32 UserType, u16 UserData, u32 EndpointIndex, u32 BufferIndex)
{
    // IO->SubmissionsCount++;
    
    // win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);
    
    // Win32Operation->Socket = INVALID_SOCKET;
    // Win32Operation->UserData = UserData;
    
    // if(IO->DisconnectEx((SOCKET)Socket, &Win32Operation->Overlapped, 0, 0) == FALSE) // TODO: Figure out if this really is what we want
    // {
    //     DWORD Error = WSAGetLastError();
    //     if(Error != WSA_IO_PENDING)
    //     {
    //         Win32PostOperationEntry(IO, Win32Operation, Error);
    //     }
    // }
    
    SOCKET Socket = Win32GetSocketFromEndpointIndex(IO, EndpointIndex);
    
    int Status = shutdown(Socket, SD_SEND); // TODO: Find some better call for this?
    DWORD Error = Status == 0 ? 0 : WSAGetLastError();
    
    IO->SubmissionsCount++;
    
    win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);
    
    io_operation_data_unpacked UnpackedData;
    UnpackedData.OperationType = IOOperationType_Shutdown;
    UnpackedData.UserType = UserType;
    UnpackedData.EndpointIndex = EndpointIndex;
    UnpackedData.BufferIndex = BufferIndex;
    UnpackedData.UserData = UserData;
    
    Win32Operation->Socket = INVALID_SOCKET;
    Win32Operation->UserData = IOOperationDataPack(UnpackedData);
    
    Win32PostOperationEntry(IO, Win32Operation, Error);
}

internal void
IOClose(io *IO, u32 UserType, u16 UserData, u32 EndpointIndex, u32 BufferIndex)
{
    SOCKET Socket = Win32GetSocketFromEndpointIndex(IO, EndpointIndex);
    
    // NOTE: Set zero linger for abortive close
    
    LINGER Linger;
    Linger.l_onoff = 1;
    Linger.l_linger = 0;
    
    int Status = setsockopt(Socket, SOL_SOCKET, SO_LINGER, (char *)&Linger, SizeOf(Linger));
    Assert(Status == 0);
    
    Status = closesocket(Socket); // TODO: Find some better call for this?
    DWORD Error = Status == 0 ? 0 : WSAGetLastError();
    
    IO->SubmissionsCount++;
    
    win32_operation_list_entry *Win32Operation = Win32GetOperationEntry(IO);
    
    io_operation_data_unpacked UnpackedData;
    UnpackedData.OperationType = IOOperationType_Close;
    UnpackedData.UserType = UserType;
    UnpackedData.EndpointIndex = EndpointIndex;
    UnpackedData.BufferIndex = BufferIndex;
    UnpackedData.UserData = UserData;
    
    Win32Operation->Socket = INVALID_SOCKET;
    Win32Operation->UserData = IOOperationDataPack(UnpackedData);
    
    Win32PostOperationEntry(IO, Win32Operation, Error);
}

internal u64
IOSocket(io *IO)
{
    SOCKET Socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    Assert(Socket != INVALID_SOCKET);
    
    HANDLE Test = CreateIoCompletionPort((HANDLE)Socket, IO->CompletionPort, 0, 0);
    Assert(Test == IO->CompletionPort);
    
    u64 Result = (u64)Socket;
    return(Result);
}

internal u64
IOTCPListen(io *IO, u32 Address, u16 Port, u32 Backlog)
{
    SOCKET Socket = (SOCKET)IOSocket(IO);
    
    BOOL ReuseAddress = TRUE;
    Assert(setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&ReuseAddress, SizeOf(ReuseAddress)) != SOCKET_ERROR);
    
    sockaddr_in SocketAddress = {};
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = EndianSwap16(Port);
    SocketAddress.sin_addr.s_addr = EndianSwap32(Address);
    
    Assert(bind(Socket, (sockaddr *)&SocketAddress, SizeOf(SocketAddress)) != SOCKET_ERROR);
    Assert(listen(Socket, (int)Backlog) != SOCKET_ERROR);
    
    u64 Result = (u64)Socket;
    return(Result);
}

internal void
IOTCPTLS(io *IO, u32 Entries, u32 *OriginalUserTypes, umm OriginalUserTypesCount, u32 UserType, char *CertificatePath, char *CertificateAuthorityPath, char *PrivateKeyPath)
{
    Assert(0); // NOTE: Not supported yet!
}

#endif // WASP_IO_H
*/

#ifdef WASP_WINDOW_H

struct window_creation_info
{
    char *Name;
    char *Class;
    
    DWORD MainThreadID;
    
    HANDLE Semaphore;
    HANDLE Window;
};

internal LRESULT
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        // TODO: Handle all of these
        
        case WM_CHAR:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_DESTROY:
        {
            LONG_PTR UserData = GetWindowLongPtrW(Window, GWLP_USERDATA);
            DWORD MainThreadID = (DWORD)UserData;
            
            PostThreadMessageW(MainThreadID, Message, WParam, LParam);
            
            if(Message == WM_DESTROY)
            {
                PostQuitMessage(0);
            }
        } break;
        
        default:
        {
            Result = DefWindowProcW(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

internal DWORD
WindowThreadProc(LPVOID Parameter)
{
    window_creation_info *Info = (window_creation_info *)Parameter;
    
    // TODO: Convert the name & class
    wchar_t *WideName = L"My Window";
    wchar_t *WideClass = L"WASPWindowClass";
    
    HINSTANCE Instance = GetModuleHandle(0);
    
    WNDCLASSEXW WindowClass = {};
    WindowClass.cbSize = SizeOf(WNDCLASSEXW);
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.lpszClassName = WideClass;
    
    if(RegisterClassExW(&WindowClass))
    {
        HWND Window = CreateWindowExW(
                                      WS_EX_APPWINDOW,
                                      WindowClass.lpszClassName,
                                      WideName,
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      0, 0, WindowClass.hInstance, 0);
        
        if(Window)
        {
            SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)Info->MainThreadID);
            
            Info->Window = Window;
        }
    }
    
    ReleaseSemaphore(Info->Semaphore, 1, 0);
    
    if(Info->Window)
    {
        MSG Message;
        while(GetMessageW(&Message, 0, 0, 0) > 0)
        {
            TranslateMessage(&Message);
            DispatchMessageW(&Message);
        }
    }
    
    return(0);
}

internal window_handle
WindowCreate(char *Name, char *Class)
{
    window_creation_info Info = {};
    Info.Name = Name;
    Info.Class = Class;
    Info.MainThreadID = GetCurrentThreadId();
    Info.Semaphore = CreateSemaphoreExW(0, 0, 1, 0, 0, SEMAPHORE_ALL_ACCESS);
    
    DWORD ThreadID;
    HANDLE WindowThreadHandle = CreateThread(0, 0, WindowThreadProc, (LPVOID)&Info, 0, &ThreadID);
    CloseHandle(WindowThreadHandle);
    
    WaitForSingleObjectEx(Info.Semaphore, INFINITE, FALSE);
    CloseHandle(Info.Semaphore);
    
    window_handle Result = {(u64)Info.Window};
    return(Result);
}

internal b32
WindowPoll(window_handle Handle, window_event *Event)
{
    b32 Result = 0;
    
    MSG Message;
    if(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_CHAR:
            {
                Event->Type = WindowEventType_Character;
            } break;
            
            case WM_KEYDOWN:
            {
                Event->Type = WindowEventType_KeyDown;
            } break;
            
            case WM_KEYUP:
            {
                Event->Type = WindowEventType_KeyUp;
            } break;
            
            case WM_DESTROY:
            {
                Event->Type = WindowEventType_Close;
            } break;
            
            default: InvalidCase;
        }
        
        Result = 1;
    }
    
    return(Result);
}

internal inline b32
IsValid(window_handle Handle)
{
    return(Handle.Value != 0);
}

#endif // WASP_WINDOW_H

#ifdef WASP_RENDERER_VULKAN_H

#include <vulkan/vulkan_win32.h>

internal inline const char *
VulkanPlatformGetInstanceExtension(void)
{
    return(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
}

internal VkSurfaceKHR
VulkanPlatformCreateSurface(VkInstance Instance, window_handle Window)
{
    VkSurfaceKHR Result = 0;
    
    b32 Ok = Instance != VK_NULL_HANDLE;
    
    if(Ok)
    {
        VkWin32SurfaceCreateInfoKHR Win32SurfaceCreateInfoKHR = {};
        Win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        Win32SurfaceCreateInfoKHR.hinstance = GetModuleHandle(0);
        Win32SurfaceCreateInfoKHR.hwnd = (HWND)Window.Value;
        
        vkCreateWin32SurfaceKHR(Instance, &Win32SurfaceCreateInfoKHR, 0, &Result);
    }
    
    return(Result);
}

#endif // WASP_RENDERER_VULKAN_H

// TODO: Try to not have this duplicated in all platform layers...

struct thread_context
{
    u32 ThreadIndex;
    
#ifdef WASP_MEMORY_H
    memory_arena Arenas[2];
#endif
    
#ifdef WASP_LOG_H
    log Log;
#endif
};

// TODO: Use the Tls API (to support no CRT).
// We should do this with some sort of init once function, like we would need for assert,
// so probably rewrite that one.
// Unless we just make the entry point be in WASP, and therefore also the TLS init stuff?

// global DWORD GlobalWin32ThreadContextIndex;

#ifdef WASP_THREAD_H
#error We are doing some bad stuff here, it's time to implement TLS properly!
#endif

global thread_context *GlobalThreadContext;

internal thread_context *
ThreadContextGet()
{
    thread_context *Result = GlobalThreadContext;
    
    if(!Result)
    {
        Result = (thread_context *)VirtualAlloc(0, SizeOf(*Result), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
        Assert(Result);
        
        GlobalThreadContext = Result;
        Result->ThreadIndex = 0; // TODO
        
#ifdef WASP_MEMORY_H
        MemoryInit(); // NOTE: It's important that this happens first so that the log can allocate in it's init
#endif
        
#ifdef WASP_LOG_H
        LogInit();
#endif
    }
    
    return(Result);
}

#ifdef WASP_MEMORY_H
internal memory_arena *MemoryArenasGet(void) {return(ThreadContextGet()->Arenas);}
#endif

#ifdef WASP_LOG_H
internal log *LogGet(void) {return(&ThreadContextGet()->Log);}
#endif

#endif // WASP_WIN32_H