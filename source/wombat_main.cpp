#include <SDL3/SDL.h>

#include "wasp.h"
#include "wasp_math.h"
#include "wasp_memory.h"
#include "wasp_string.h"
#include "wasp_win32.h"

#define PixelScale 4
#define WindowWidth 1280
#define WindowHeight 720

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

internal texture
TextureCreate(char *Path)
{
    texture Result = {};

    // TODO: Replace with loading form file.
    SDL_Surface *LoadedSurface = SDL_CreateSurface(32 * PixelScale, 32 * PixelScale, GlobalContext.WindowSurface->format);

    Result.Surface = SDL_ConvertSurface(LoadedSurface, GlobalContext.WindowSurface->format);
    Result.Width = Result.Surface->w;
    Result.Height = Result.Surface->h;

    SDL_DestroySurface(LoadedSurface);

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
        StringFormat(Buffer, SizeOf(Buffer), "%s%d.bmp", BaseName, FrameIndex);

        Result.Frames[FrameIndex] = TextureCreate((char *)Buffer);
    }


    return(Result);
}


s32 main(s32 ArgsCount, char **Args)
{
    SDL_Init(SDL_INIT_VIDEO);

    memory_arena Arena = MemoryArenaCreate(0, MB(8), 0);

    SDL_Window *Window = SDL_CreateWindow("Wombat", WindowWidth, WindowHeight, 0);
    SDL_Surface *WindowSurface = SDL_GetWindowSurface(Window);

    GlobalContext.Window = Window;
    GlobalContext.WindowSurface = WindowSurface;

    

    // texture Test = TextureCreate(WindowSurface, "assets/");

    // SDL_Surface *Test = SDL_CreateSurface(16 * PixelScale, 16 * PixelScale, WindowSurface->format);
    // MemorySet(Test->pixels, 0xFF, Test->h * Test->pitch);

    
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

        SDL_Rect SourceRect = {};
        SourceRect.x = 0;
        SourceRect.y = 0;
        SourceRect.w = 16 * PixelScale;
        SourceRect.h = 16 * PixelScale;

        SDL_Rect DestRect = {};
        DestRect.x = 0;
        DestRect.y = 0;
        DestRect.w = 16 * PixelScale;
        DestRect.h = 16 * PixelScale;

        SDL_BlitSurface(Test, &SourceRect, WindowSurface, &DestRect);

        SDL_UpdateWindowSurface(Window);
    }

    SDL_Quit();

    return(0);
}