#include <SDL3/SDL.h>

#include "wasp.h"
#include "wasp_math.h"
#include "wasp_memory.h"
#include "wasp_string.h"
#include "wasp_win32.h"

#define PixelScale 8
#define WindowWidth 1280
#define WindowHeight 720
#define TilePixelSize PixelScale * 16

struct texture
{
    SDL_Surface *Surface;
    u32 Width;
    u32 Height;
};

struct animation
{
    u32 FramesCount;
    texture *Frames;
    u32 Width;
    u32 Height;
};

struct tile_map
{
    u32 SizeX;
    u32 SizeY;
    u8 *Layout;

    animation *Animations;
    u32 AnimationsCount;
};

struct context
{
    SDL_Surface *WindowSurface;
    SDL_Window *Window;
};

global context GlobalContext;

internal void AssertCallback(char *Expression, char *File, int LineNumber, char *Function)
{
}

internal tile_map
TileMapCreate(memory_arena *Arena, u32 SizeX, u32 SizeY, animation *Animations, u32 AnimationsCount)
{
    tile_map Result = {};

    Result.SizeX = SizeX;
    Result.SizeY = SizeY;
    Result.Layout = MemoryArenaPushArray(Arena, u8, 0, SizeX * SizeY);
    Result.Animations = MemoryArenaPushArray(Arena, animation, 0, AnimationsCount);
    Result.AnimationsCount = AnimationsCount;

    return(Result);
}

internal texture
TextureCreate(char *Path)
{
    texture Result = {};

    SDL_Surface *LoadedSurface = SDL_LoadBMP(Path);

    if(!LoadedSurface)
    {
        Outf("Failed to load texture: %s", SDL_GetError());
        Assert(0);
    }

    // Result.Surface = SDL_ConvertSurface(LoadedSurface, GlobalContext.WindowSurface->format);
    Result.Surface = LoadedSurface;
    Result.Width = Result.Surface->w;
    Result.Height = Result.Surface->h;

    // SDL_DestroySurface(LoadedSurface);

    return(Result);
}

internal animation
AnimationCreate(memory_arena *Arena, char *BaseName, u32 FramesCount)
{
    animation Result = {};

    Result.FramesCount = FramesCount;
    Result.Frames = MemoryArenaPushArray(Arena, texture, 0, FramesCount);

    for(u32 FrameIndex = 0;
        FrameIndex < FramesCount;
        FrameIndex++)
    {
        u8 Buffer[1028];
        u64 Length = StringFormat(Buffer, SizeOf(Buffer), "%s%d.bmp", BaseName, FrameIndex);

        Buffer[Length] = 0;

        Result.Frames[FrameIndex] = TextureCreate((char *)Buffer);

        if(FrameIndex == 0)
        {
            Result.Width = Result.Frames[0].Width;
            Result.Height = Result.Frames[0].Height;
        }
    }

    return(Result);
}

internal void
TextureDraw(texture *Texture, u32 X, u32 Y)
{
    SDL_Rect SourceRect = {};
    SourceRect.x = 0;
    SourceRect.y = 0;
    SourceRect.w = Texture->Width;
    SourceRect.h = Texture->Height;

    SDL_Rect DestRect = {};
    DestRect.x = X;
    DestRect.y = Y;
    DestRect.w = 16 * PixelScale;
    DestRect.h = 16 * PixelScale;

    SDL_BlitSurfaceScaled(
        Texture->Surface,
        &SourceRect,
        GlobalContext.WindowSurface,
        &DestRect,
        SDL_SCALEMODE_NEAREST);
}

internal void
AnimationDraw(animation *Animation, u32 X, u32 Y, u32 Frame)
{
    u32 WrappedFrame = Frame % Animation->FramesCount;
    texture *Texture = &Animation->Frames[WrappedFrame];
    TextureDraw(Texture, X, Y);
}

s32 main(s32 ArgsCount, char **Args)
{
    SDL_Init(SDL_INIT_VIDEO);

    memory_arena Arena = MemoryArenaCreate(0, MB(8), 0);

    SDL_Window *Window = SDL_CreateWindow("Wombat", WindowWidth, WindowHeight, 0);
    SDL_Surface *WindowSurface = SDL_GetWindowSurface(Window);

    GlobalContext.Window = Window;
    GlobalContext.WindowSurface = WindowSurface;

    animation Animation = AnimationCreate(&Arena, "assets/cuki_", 4);

    u32 FrameCounter = 0;
    u32 SlowAnimFrame = 0;
    
    b32 Running = 1;
    while(Running)
    {
        SDL_Event Event;
        while(SDL_PollEvent(&Event))
        {
            if(Event.type == SDL_EVENT_QUIT)
            {
                Running = 0;
            }
        }

        SDL_ClearSurface(WindowSurface, 0, 0, 0, 1);

        AnimationDraw(&Animation, WindowWidth / 2 - TilePixelSize / 2, WindowHeight / 2 - TilePixelSize / 2, SlowAnimFrame);

        SDL_UpdateWindowSurface(Window);

        FrameCounter++;
        if(FrameCounter % 100 == 0)
        {
            SlowAnimFrame++;
        }
    }

    SDL_Quit();

    return(0);
}