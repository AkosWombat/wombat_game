#ifndef WASP_H
#error This module depends on wasp.h
#endif

#ifndef WASP_STRING_H
#define WASP_STRING_H

// TODO: UTF8/UTF16 support
// TODO: Rewrite string->int int->string convertions, some good API for it, also for 32 bit floats, without STB
// TODO: String formatting without specifiers (so type info instead)
// TODO: Hex back and forth convertion

#include <stdarg.h>

/*
  CONSTANTS
*/

/*
  TYPES
*/

typedef buffer string;

/*
  GLOBALS
*/

/*
  FUNCTIONS
*/

internal inline b32 CharacterIsLower(u8 Character);
internal inline b32 CharacterIsUpper(u8 Character);

internal inline b32 CharacterIsWhitespace(u8 Character);
internal inline b32 CharacterIsAlpha(u8 Character);
internal inline b32 CharacterIsNumber(u8 Character);
internal inline b32 CharacterIsASCII(u8 Character);

internal inline u8 CharacterToLower(u8 Character);
internal inline u8 CharacterToUpper(u8 Character);

internal inline umm StringLengthZ(char *Z);
internal inline string StringBundleZ(char *Z);

#define string(String) StringBundleZ((char *)(String))

internal inline void StringAdvance(string *String, umm Count);

// TODO: Possibly have more combinations of these, and possibly don't suffix with Z?
internal inline b32 StringEqualsZ(string A, char *B);
internal inline b32 StringEqualsIgnoreCaseZ(string A, char *B);

internal b32 StringEqualsSafe(string A, string B); // NOTE: Constant time comparison
internal b32 StringEquals(string A, string B);
internal b32 StringEqualsIgnoreCase(string A, string B);
internal b32 StringContains(string A, string B);
internal b32 StringStartsWith(string A, string B);

internal inline string StringPeek(string String, umm Count);
internal inline string StringEat(string *String, umm Count);

internal inline u8 StringFirstCharacterOrZero(string String);

// TODO: Remove/replace with below
internal inline b32 StringEatIfFirstCharacterMatches(string *String, u8 Character);

// TODO: Instead implement:
// internal inline u8 StringEatCharacter(string *String);
// internal inline b32 StringEatCharacterIfFirstMatches(string *String, u8 Character);

// TODO: Some option to return the original if the character was not found (example use case in LinuxGetVersion)
// TODO: Some option to include the character that we split on (example use case in TokenizerParseComment)
internal string StringSplit(string *Original, u8 Character);

// TODO: Remove this function and replace with simpler individual string parsers
// TODO: Handle negative hex
// TODO: Allow ignore (null) signed
internal b32 StringToInteger(string String, u64 *Value, b32 *Signed, u32 ExpectedBase, u64 *RealSize);

internal umm StringFormatList(u8 *Buffer, umm Size, char *Format, va_list Args);
internal umm StringFormat(u8 *Buffer, umm Size, char *Format, ...);

internal void Outf(char *Format, ...);

#ifdef WASP_MEMORY_H
internal string PushfList(memory_arena *Arena, char *Format, va_list Args);
internal string Pushf(memory_arena *Arena, char *Format, ...);
#endif

/*
  IMPLEMENTATION
*/

// TODO: Assert where needed

internal inline b32
CharacterIsLower(u8 Character)
{
    return(Character >= 'a' && Character <= 'z');
}

internal inline b32
CharacterIsUpper(u8 Character)
{
    return(Character >= 'A' && Character <= 'Z');
}

internal inline b32
CharacterIsWhitespace(u8 Character)
{
    return(Character == ' '  ||
           Character == '\r' ||
           Character == '\n' ||
           Character == '\t' ||
           Character == '\f');
}

internal inline b32
CharacterIsAlpha(u8 Character)
{
    return((Character >= 'A' && Character <= 'Z') ||
           (Character >= 'a' && Character <= 'z'));
}

internal inline b32
CharacterIsNumber(u8 Character)
{
    return(Character >= '0' && Character <= '9');
}

internal inline b32
CharacterIsASCII(u8 Character)
{
    return(Character < (u8)MaxS8);
}

internal inline u8
CharacterToLower(u8 Character)
{
    u8 Result = Character;
    
    if(CharacterIsUpper(Character))
    {
        Result = Character - 'A' + 'a';
    }
    
    return Result;
}

internal inline u8
CharacterToUpper(u8 Character)
{
    u8 Result = Character;
    
    if(CharacterIsLower(Character))
    {
        Result = Character - 'a' + 'A';
    }
    
    return Result;
}

internal inline umm
StringLengthZ(char *Z)
{
    umm Result = 0;
    
    if(Z)
    {
        while(*Z++)
        {
            Result++;
        }
    }
    
    return(Result);
}

internal inline string
StringBundleZ(char *Z)
{
    string Result;
    Result.Data = (u8 *)Z;
    Result.Size = StringLengthZ(Z);
    return(Result);
}

internal inline void
StringAdvance(string *String, umm Count)
{
    Assert(String);
    Assert(!String->Size || String->Data);
    Assert(String->Size >= Count);
    
    String->Data += Count;
    String->Size -= Count;
}

internal inline b32
StringEqualsZ(string A, char *B)
{
    return(StringEquals(A, StringBundleZ(B)));
}

internal inline b32
StringEqualsIgnoreCaseZ(string A, char *B)
{
    return(StringEqualsIgnoreCase(A, StringBundleZ(B)));
}

internal b32
StringEqualsSafe(string A, string B)
{
    b32 Result = 0;
    
    if(A.Size == B.Size)
    {
        u64 Value = 0;
        
        for(u64 Count = 0;
            Count < A.Size;
            Count++)
        {
            Value |= A.Data[Count] ^ B.Data[Count];
        }
        
        if(Value == 0)
        {
            Result = 1;
        }
    }
    
    return(Result);
}

internal b32
StringEquals(string A, string B)
{
    b32 Result = 1;
    
    if(A.Size == B.Size)
    {
        for(u64 Count = 0;
            Count < A.Size;
            Count++)
        {
            if(A.Data[Count] != B.Data[Count])
            {
                Result = 0;
                break;
            }
        }
    }
    else
    {
        Result = 0;
    }
    
    return(Result);
}

internal b32
StringEqualsIgnoreCase(string A, string B)
{
    b32 Result = 1;
    
    if(A.Size == B.Size)
    {
        for(u64 Count = 0;
            Count < A.Size;
            Count++)
        {
            if(CharacterToLower(A.Data[Count]) != CharacterToLower(B.Data[Count]))
            {
                Result = 0;
                break;
            }
        }
    }
    else
    {
        Result = 0;
    }
    
    return(Result);
}

internal b32
StringContains(string A, string B)
{
    NotImplemented; // TODO: Check that this is even correct...
    
    b32 Result = 0;
    
    while(A.Size)
    {
        string Test;
        Test.Data = A.Data;
        Test.Size = B.Size;
        
        if(StringEquals(Test, B))
        {
            Result = 1;
            break;
        }
        
        A.Data++;
        A.Size--;
    }
    
    return(Result);
}

internal b32
StringStartsWith(string A, string B)
{
    string Test = {A.Data, B.Size};
    b32 Result = (A.Size >= B.Size) && StringEquals(Test, B);
    return(Result);
}

internal inline string
StringPeek(string String, umm Count)
{
    string Result = {};
    
    if(String.Data &&
       String.Size >= Count)
    {
        Result.Data = String.Data;
        Result.Size = Count;
    }
    
    return(Result);
}

internal inline string
StringEat(string *String, umm Count)
{
    string Result = {};
    
    if(IsValid(StringPeek(*String, Count)))
    {
        Result.Data = String->Data;
        Result.Size = Count;
        
        StringAdvance(String, Count);
    }
    
    return(Result);
}

internal inline u8
StringFirstCharacterOrZero(string String)
{
    u8 Result = 0;
    
    if(IsValid(String))
    {
        Result = String.Data[0];
    }
    
    return(Result);
}

internal inline b32
StringEatIfFirstCharacterMatches(string *String, u8 Character)
{
    b32 Result = String->Size && String->Data[0] == Character;
    
    if(Result)
    {
        StringAdvance(String, 1);
    }
    
    return(Result);
}

// TODO: Some option to return the original if the character was not found (example use case in LinuxGetVersion)

internal string
StringSplit(string *Original, u8 Character)
{
    Assert(Original);
    
    string Result = {};
    
    for(u64 Index = 0;
        Index < Original->Size;
        Index++)
    {
        if(Original->Data[Index] == Character)
        {
            Result.Data = Original->Data;
            Result.Size = Index;
            
            if(Original->Size == Index-1)
            {
                Original->Data = 0;
                Original->Size = 0;
            }
            else
            {
                StringAdvance(Original, Index + 1);
            }
            
            break;
        }
    }
    
    return(Result);
}

// TODO: Handle negative hex
// TODO: Allow ignore (null) signed

internal b32
StringToInteger(string String, u64 *Value, b32 *Signed, u32 ExpectedBase, u64 *RealSize)
{
    Assert(Value);
    Assert(Signed);
    
    b32 Result = 0;
    
    if(String.Size)
    {
        Assert(String.Data);
        
        u64 OriginalStringSize = String.Size;
        
        *Signed = *String.Data == '-';
        if(*Signed)
        {
            StringEat(&String, 1);
        }
        
        if(String.Size)
        {
            u32 Base = 10;
            if(String.Size >= 2 &&
               String.Data[0] == '0')
            {
                if(String.Data[1] == 'x' ||
                   String.Data[1] == 'X')
                {
                    Base = 16;
                }
                else if(String.Data[1] == 'o' ||
                        String.Data[1] == 'O')
                {
                    Base = 8;
                }
                else if(String.Data[1] == 'b' ||
                        String.Data[1] == 'B')
                {
                    Base = 2;
                }
            }
            
            if(Base != 10)
            {
                StringEat(&String, 2);
            }
            
            if(!ExpectedBase || ExpectedBase == Base)
            {
                Assert(Base == 2  ||
                       Base == 8  ||
                       Base == 10 ||
                       Base == 16);
                
                // TODO: Some cleaner way to do this, so we can avoid the duplication of code
                if(RealSize)
                {
                    u64 FullSize = String.Size;
                    u64 PrefixSize = OriginalStringSize - String.Size;
                    String.Size = 0;
                    
                    for(u64 Index = 0;
                        Index < FullSize;
                        Index++)
                    {
                        u8 Character = String.Data[Index];
                        if(Character >= '0')
                        {
                            if(Character > '9')
                            {
                                if(Base == 16 &&
                                   ((Character >= 'a' && Character <= 'f') ||
                                    (Character >= 'A' && Character <= 'F')))
                                {
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            break;
                        }
                        
                        String.Size++;
                    }
                    
                    *RealSize = PrefixSize + String.Size;
                }
                
                if(String.Size)
                {
                    u64 Exponent = 1;
                    
                    Result = 1;
                    *Value = 0;
                    
                    for(u64 Index = 0;
                        Index < String.Size;
                        Index++)
                    {
                        u8 Character = String.Data[String.Size - Index - 1];
                        if(Character >= '0')
                        {
                            u64 Number;
                            if(Character > '9')
                            {
                                if(Base == 16 &&
                                   ((Character >= 'a' && Character <= 'f') ||
                                    (Character >= 'A' && Character <= 'F')))
                                {
                                    Number = CharacterToLower(Character) - 'a' + 10;
                                }
                                else
                                {
                                    Result = 0;
                                    break;
                                }
                            }
                            else
                            {
                                Number = Character - '0';
                            }
                            
                            *Value += Number * Exponent;
                        }
                        else
                        {
                            Result = 0;
                            break;
                        }
                        
                        Exponent *= Base;
                    }
                }
            }
        }
    }
    
    return(Result);
}

internal void
StringFormatListWriteByte(u8 *Buffer, umm Size, umm *Cursor, u8 Byte)
{
    if(Buffer)
    {
        if(*Cursor < Size)
        {
            Buffer[(*Cursor)++] = Byte;
        }
    }
    else
    {
        (*Cursor)++;
    }
}

internal umm
StringFormatList(u8 *Buffer, umm Size, char *Format, va_list Args)
{
    //
    // NOTE: Following specification https://cplusplus.com/reference/cstdio/printf/
    //
    
    umm Cursor = 0;
    while(*Format)
    {
        if(*Format == '%')
        {
            Format++;
            Assert(*Format);
            
            enum
            {
                Flag_LeftJustify         = 1 << 0,
                Flag_ForceSignPrefix     = 1 << 1,
                Flag_InsertSpaceIfNoSign = 1 << 2,
                Flag_ForcePrefixOrSuffix = 1 << 3,
                Flag_LeftPadWithZeroes   = 1 << 4,
            };
            
            u32 Flags = 0;
            b32 ParsingFlags = 1;
            while(ParsingFlags)
            {
                switch(*Format++)
                {
                    case '-':
                    {
                        Flags |= Flag_LeftJustify;
                    } break;
                    
                    case '+':
                    {
                        Flags |= Flag_ForceSignPrefix;
                    } break;
                    
                    case ' ':
                    {
                        Flags |= Flag_InsertSpaceIfNoSign;
                    } break;
                    
                    case '#':
                    {
                        Flags |= Flag_ForcePrefixOrSuffix;
                    } break;
                    
                    case '0':
                    {
                        Flags |= Flag_LeftPadWithZeroes;
                    } break;
                    
                    default:
                    {
                        ParsingFlags = 0;
                    } break;
                }
            }
            
            Format--;
            Assert(*Format);
            
            int Width = 0;
            if(*Format == '*')
            {
                Format++;
                
                Width = va_arg(Args, int);
            }
            else
            {
                int Count = 0;
                while(Format[Count] &&
                      Format[Count] >= '0' &&
                      Format[Count] <= '9')
                {
                    Count++;
                }
                
                int Exponent = 1;
                for(int Index = 0;
                    Index < Count;
                    Index++)
                {
                    char Character = Format[Count - Index - 1];
                    int Number = Character - '0';
                    
                    Width += Number * Exponent;
                    Exponent *= 10;
                }
                
                Format += Count;
            }
            
            Assert(*Format);
            
            b32 HasPrecision = 0;
            int Precision = 0;
            
            if(*Format == '.')
            {
                Format++;
                
                if(*Format == '*')
                {
                    Format++;
                    
                    Precision = va_arg(Args, int);
                }
                else
                {
                    int Count = 0;
                    while(Format[Count] &&
                          Format[Count] >= '0' &&
                          Format[Count] <= '9')
                    {
                        Count++;
                    }
                    
                    int Exponent = 1;
                    for(int Index = 0;
                        Index < Count;
                        Index++)
                    {
                        char Character = Format[Count - Index - 1];
                        int Number = Character - '0';
                        
                        Precision += Number * Exponent;
                        Exponent *= 10;
                    }
                    
                    Format += Count;
                }
                
                HasPrecision = 1;
            }
            
            Assert(*Format);
            
            u32 Length = 0;
            switch(*Format)
            {
                case 'h':
                {
                    Format++;
                    
                    if(*Format == 'h')
                    {
                        Format++;
                        
                        Length = SizeOf(char);
                    }
                    else
                    {
                        Length = SizeOf(short int);
                    }
                } break;
                
                case 'l':
                {
                    Format++;
                    
                    if(*Format == 'l')
                    {
                        Format++;
                        
                        Length = SizeOf(long long int);
                    }
                    else
                    {
                        Length = SizeOf(long int);
                    }
                } break;
                
                case 'j':
                {
                    InvalidCase;
                } break;
                
                case 'z':
                {
                    Length = SizeOf(uintptr_t);
                    Format++;
                } break;
                
                case 't':
                {
                    InvalidCase;
                } break;
                
                default:
                {
                    Length = SizeOf(int);
                } break;
            }
            
            Assert(*Format);
            
            char Specifier = *Format++;
            switch(Specifier)
            {
                //
                // NOTE: Standard specifiers
                //
                
                case 'd':
                case 'i':
                case 'u':
                case 'o':
                case 'x':
                case 'X':
                case 'p':
                case 'b':
                {
                    if(Specifier == 'p')
                    {
                        Length = SizeOf(void *);
                    }
                    
                    u64 Value = 0;
                    if(Specifier == 'u' ||
                       Specifier == 'p' ||
                       Specifier == 'x')
                    {
                        switch(Length)
                        {
                            case 1:
                            {
                                Value = va_arg(Args, u8);
                            } break;
                            
                            case 2:
                            {
                                Value = va_arg(Args, u16);
                            } break;
                            
                            case 4:
                            {
                                Value = va_arg(Args, u32);
                            } break;
                            
                            case 8:
                            {
                                Value = va_arg(Args, u64);
                            } break;
                            
                            default: Assert(0);
                        }
                    }
                    else
                    {
                        s64 SignedValue = 0;
                        switch(Length)
                        {
                            case 1:
                            {
                                SignedValue = va_arg(Args, s8);
                            } break;
                            
                            case 2:
                            {
                                SignedValue = va_arg(Args, s16);
                            } break;
                            
                            case 4:
                            {
                                SignedValue = va_arg(Args, s32);
                            } break;
                            
                            case 8:
                            {
                                SignedValue = va_arg(Args, s64);
                            } break;
                            
                            default: Assert(0);
                        }
                        
                        if(SignedValue < 0)
                        {
                            Value = (u64)(-SignedValue);
                            
                            StringFormatListWriteByte(Buffer, Size, &Cursor, '-');
                        }
                        else
                        {
                            Value = (u64)SignedValue;
                            
                            if(Flags & Flag_ForceSignPrefix)
                            {
                                StringFormatListWriteByte(Buffer, Size, &Cursor, '+');
                            }
                        }
                    }
                    
                    u64 Base;
                    if(Specifier == 'o')
                    {
                        Base = 8;
                    }
                    else if(Specifier == 'x' ||
                            Specifier == 'X' ||
                            Specifier == 'p')
                    {
                        Base = 16;
                    }
                    else if(Specifier == 'b')
                    {
                        Base = 2;
                    }
                    else
                    {
                        Base = 10;
                    }
                    
                    // TODO: Properly handle precision
                    
                    int Count = 1;
                    u64 Exponent = 1;
                    
                    for(u64 Temp = Value / Base;
                        Temp;
                        Temp /= Base)
                    {
                        Exponent *= Base;
                        Count++;
                    }
                    
                    int Delta = Precision - Count;
                    if(Delta > 0)
                    {
                        for(int Index = 0;
                            Index < Delta;
                            Index++)
                        {
                            StringFormatListWriteByte(Buffer, Size, &Cursor, '0');
                        }
                    }
                    
                    while(Exponent)
                    {
                        u64 Number = Value / Exponent;
                        Value = Value % Exponent;
                        Exponent /= Base;
                        
                        u8 Character = 0;
                        if(Number > 9)
                        {
                            Assert(Number <= 0xF);
                            
                            if(Specifier == 'x')
                            {
                                Character = 'a' + (u8)Number - 10;
                            }
                            else if(Specifier == 'X' ||
                                    Specifier == 'p')
                            {
                                Character = 'A' + (u8)Number - 10;
                            }
                            else
                            {
                                Assert(0);
                            }
                        }
                        else
                        {
                            Character = '0' + (u8)Number;
                        }
                        
                        StringFormatListWriteByte(Buffer, Size, &Cursor, Character);
                    }
                } break;
                
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                {
#if defined(STB_SPRINTF_NOFLOAT) || !defined(STB_SPRINTF_H_INCLUDE)
                    Assert(0);
#else
                    f64 Value = va_arg(Args, f64);
                    
                    u32 TempCount = 0;
                    char Temp[8];
                    
                    Temp[TempCount++] = '%';
                    if(HasPrecision)
                    {
                        Temp[TempCount++] = '.';
                        Temp[TempCount++] = '*';
                    }
                    
                    switch(Specifier)
                    {
                        case 'f': {Temp[TempCount++] = 'f';} break;
                        case 'F': {Temp[TempCount++] = 'F';} break;
                        case 'g': {Temp[TempCount++] = 'g';} break;
                        case 'G': {Temp[TempCount++] = 'G';} break;
                    }
                    
                    Assert(TempCount < SizeOf(Temp));
                    Temp[TempCount] = 0;
                    
                    Assert(Size >= Cursor);
                    int BufferSize = (int)(Size - Cursor);
                    
                    if(HasPrecision)
                    {
                        Cursor += stbsp_snprintf((char *)(Buffer + Cursor), BufferSize, Temp, Precision, Value);
                    }
                    else
                    {
                        Cursor += stbsp_snprintf((char *)(Buffer + Cursor), BufferSize, Temp, Value);
                    }
#endif
                } break;
                
                case 'e':
                case 'E':
                {
                    Assert(0);
                } break;
                
                case 'a':
                case 'A':
                {
                    Assert(0);
                } break;
                
                case 'c':
                {
                    int Value = va_arg(Args, int);
                    char Character = (char)Value;
                    
                    StringFormatListWriteByte(Buffer, Size, &Cursor, Character);
                } break;
                
                case 's':
                {
                    char *Value = va_arg(Args, char *);
                    if(!Value && (!HasPrecision || Precision))
                    {
                        Value = "(null)";
                    }
                    
                    u64 Count = 0;
                    while((!HasPrecision || Count < Precision) && *Value)
                    {
                        StringFormatListWriteByte(Buffer, Size, &Cursor, *Value);
                        
                        Value++;
                        Count++;
                    }
                } break;
                
                case 'n':
                {
                    Assert(0);
                } break;
                
                case '%':
                {
                    StringFormatListWriteByte(Buffer, Size, &Cursor, '%');
                } break;
                
                //
                // NOTE: Non-standard specifiers
                //
                
                case 'S':
                {
                    string Value = va_arg(Args, string);
                    
                    if(Precision)
                    {
                        Value.Size = Precision;
                    }
                    
                    if(!Value.Data)
                    {
                        Value.Size = 0;
                    }
                    
                    for(u64 Index = 0;
                        Index < Value.Size;
                        Index++)
                    {
                        StringFormatListWriteByte(Buffer, Size, &Cursor, Value.Data[Index]);
                    }
                } break;
                
                default: Assert(0);
            }
        }
        else
        {
            StringFormatListWriteByte(Buffer, Size, &Cursor, *Format);
            Format++;
        }
    }
    
    return(Cursor);
}

internal umm
StringFormat(u8 *Buffer, umm Size, char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    umm Result = StringFormatList(Buffer, Size, Format, Args);
    va_end(Args);
    
    return(Result);
}

internal void
Outf(char *Format, ...)
{
    // TODO: Instead just allocate from scratch arena
    
    // TODO: Just use PushfList for that
    
    u8 Buffer[1024];
    
    va_list Args;
    va_start(Args, Format);
    umm Count = StringFormatList(Buffer, SizeOf(Buffer), Format, Args);
    va_end(Args);
    
    StandardOutput(Buffer, Count);
}

#ifdef WASP_MEMORY_H

internal string
PushfList(memory_arena *Arena, char *Format, va_list Args)
{
    string Result = {};
    
    umm Count = StringFormatList(0, 0, Format, Args);
    u8 *Memory = MemoryArenaPush(Arena, Count, 1);
    if(Memory)
    {
        umm Test = StringFormatList(Memory, Count, Format, Args);
        Assert(Test == Count);
        
        Result.Data = Memory;
        Result.Size = Count;
    }
    
    return(Result);
}

internal string
Pushf(memory_arena *Arena, char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    string Result = PushfList(Arena, Format, Args);
    va_end(Args);
    
    return(Result);
}

#endif

#endif // WASP_STRING_H