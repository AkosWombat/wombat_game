#include "wasp.h"
#include "wasp_math.h"
#include "wasp_memory.h"
#include "wasp_win32.h"

#include <SDL3/SDL.h>

s32 main(s32 ArgsCount, char **Args)
{
    return(0);
}

#if 0
SDL_Window* gWindow = NULL; 
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;

bool init()
{
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        gWindow = SDL_CreateWindow( "wombat_game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {

            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    gHelloWorld = SDL_LoadBMP( "02_getting_an_image_on_the_screen/hello_world.bmp" );
    if( gHelloWorld == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError() );
        success = false;
    }

    return success;
}

void close()
{
    SDL_FreeSurface( gHelloWorld );
    gHelloWorld = NULL;

    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    SDL_Quit();
}
#endif