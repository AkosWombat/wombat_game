#include <SDL3/SDL.h>

#include "wasp.h"
#include "wasp_math.h"
#include "wasp_memory.h"
#include "wasp_string.h"
#include "wasp_win32.h"

#define PixelScale 6
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

struct collision_map
{
    u32 SizeX;
    u32 SizeY;
    b8 *Layout;
};

struct enemy
{
    u32 Type;
    v2 Position;
    f32 Angle;
    f32 TimeOfNextShot;
    b32 Dead;
};

struct projectile
{
    projectile *Prev;
    projectile *Next;

    u32 Type;
    v2 Position;
    v2 LastPosition;
    v2 Velocity;
    b32 IsEnemy;
};

struct tile_map
{
    u32 SizeX;
    u32 SizeY;
    u8 *Layout;

    animation *Animations;
    u32 AnimationsCount;
};

struct font
{
    texture Atlas;
    u32 GlyphWidth;
    u32 GlyphHeight;
};

struct context
{
    SDL_Surface *WindowSurface;
    SDL_Window *Window;

    f32 PlayerX;
    f32 PlayerY;

    f32 DirectionX;
    f32 DirectionY;

    f32 MovementSpeed;
    f32 EnemySpeed;
    u32 ProjectileDamage;
    f32 ProjectileSpeed;
    f32 FireRate;
    f32 FireRateRandomFactor;
    f32 WaveCooldown;

    u32 Seed;

    enemy *Enemies;
    u32 EnemiesCapacity;
    u32 EnemiesCount;
    u32 EnemiesRemaining;

    projectile Projectiles;
    projectile FreeProjectiles;

    u32 Health;
    u32 MaxHealth;

    f32 TimeOfLastRegenerate;
    f32 TimeOfLastWaveEnd;
    b32 IsWaitingForNextWave;
    u32 WaveIndex;

    v2 ViewDirection;
};

global context GlobalContext;

internal void AssertCallback(char *Expression, char *File, int LineNumber, char *Function)
{
}

internal u32
GetRandom()
{
    u32 X = GlobalContext.Seed;
	X ^= X << 13;
	X ^= X >> 17;
	X ^= X << 5;
	return GlobalContext.Seed = X;
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

    MemoryCopy(Result.Animations, Animations, SizeOf(animation) * AnimationsCount);

    return(Result);
}

internal collision_map
CollisionMapCreate(memory_arena *Arena, tile_map *TileMap, b8 *WalkableBlocks, u32 WalkableBlocksCount)
{
    collision_map Result = {};

    Result.SizeX = TileMap->SizeX;
    Result.SizeY = TileMap->SizeY;
    Result.Layout = MemoryArenaPushArray(Arena, b8, 0, TileMap->SizeX * TileMap->SizeY);

    for(u32 IndexY = 0;
        IndexY < TileMap->SizeY;
        IndexY++)
    {
        for(u32 IndexX = 0;
            IndexX < TileMap->SizeX;
            IndexX++)
        {
            b8 IsWalkable = 0;

            for(u32 BlockIndex = 0;
                BlockIndex < WalkableBlocksCount;
                BlockIndex++)
            {
                if(WalkableBlocks[BlockIndex] == TileMap->Layout[IndexY * TileMap->SizeX + IndexX])
                {
                    IsWalkable = 1;
                    break;
                }
            }

            Result.Layout[IndexY * TileMap->SizeX + IndexX] = IsWalkable;
        }
    }

    return(Result);
}

internal void
CollisionMapIntersection(collision_map *CollisionMap, tile_map *TileMap, b8 *WalkableBlocks, u32 WalkableBlocksCount)
{
    for(u32 IndexY = 0;
        IndexY < TileMap->SizeY;
        IndexY++)
    {
        for(u32 IndexX = 0;
            IndexX < TileMap->SizeX;
            IndexX++)
        {
            if(CollisionMap->Layout[IndexY * TileMap->SizeX + IndexX] && TileMap->Layout[IndexY * TileMap->SizeX + IndexX])
            {
                b8 IsWalkable = 0;
    
                for(u32 BlockIndex = 0;
                    BlockIndex < WalkableBlocksCount;
                    BlockIndex++)
                {
                    if(WalkableBlocks[BlockIndex] == TileMap->Layout[IndexY * TileMap->SizeX + IndexX])
                    {
                        IsWalkable = 1;
                        break;
                    }
                }
    
                CollisionMap->Layout[IndexY * TileMap->SizeX + IndexX] = IsWalkable;
            }
        }
    }
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

internal font
FontCreate(char *Path, u32 GlyphWidth, u32 GlyphHeight)
{
    font Result = {};

    Result.Atlas = TextureCreate(Path);
    Result.GlyphWidth = GlyphWidth;
    Result.GlyphHeight = GlyphHeight;

    return(Result);
}

internal void
FontDraw(font *Font, string String, u32 X, u32 Y, b32 Centered)
{
    if(Centered)
    {
        X -= (u32)String.Size * Font->GlyphWidth * PixelScale / 2;
    }

    for(u32 CharacterIndex = 0;
        CharacterIndex < String.Size;
        CharacterIndex++)
    {
        u32 Codepoint = String.Data[CharacterIndex];
        u32 Index = 0;
        b32 FoundIndex = 0;
        if(Codepoint >= 'A' && Codepoint <= 'Z')
        {
            Index = Codepoint - 'A';
            FoundIndex = 1;
        }
        if(Codepoint >= '0' && Codepoint <= '9')
        {
            Index = Codepoint - '0' + 'Z' - 'A' + 1;
            FoundIndex = 1;
        }

        if(FoundIndex)
        {
            u32 Row = Index / 9;
            u32 Column = Index % 9;
    
            SDL_Rect SourceRect = {};
            SourceRect.x = Font->GlyphWidth * Column;
            SourceRect.y = Font->GlyphHeight * Row;
            SourceRect.w = Font->GlyphWidth;
            SourceRect.h = Font->GlyphHeight;
    
            SDL_Rect DestRect = {};
            DestRect.x = X;
            DestRect.y = Y;
            DestRect.w = Font->GlyphWidth * PixelScale;
            DestRect.h = Font->GlyphHeight * PixelScale;
    
            SDL_BlitSurfaceScaled(
                Font->Atlas.Surface,
                &SourceRect,
                GlobalContext.WindowSurface,
                &DestRect,
                SDL_SCALEMODE_NEAREST);
        }

        X += (Font->GlyphWidth + 1) * PixelScale;
    }
}

internal void
TextureDrawCustom(texture *Texture, u32 X, u32 Y)
{
    SDL_Rect SourceRect = {};
    SourceRect.x = 0;
    SourceRect.y = 0;
    SourceRect.w = Texture->Width;
    SourceRect.h = Texture->Height;

    SDL_Rect DestRect = {};
    DestRect.x = X;
    DestRect.y = Y;
    DestRect.w = Texture->Width * PixelScale;
    DestRect.h = Texture->Height * PixelScale;

    SDL_BlitSurfaceScaled(
        Texture->Surface,
        &SourceRect,
        GlobalContext.WindowSurface,
        &DestRect,
        SDL_SCALEMODE_NEAREST);
}

internal void
TextureDrawCustomPartial(texture *Texture, u32 X, u32 Y, f32 Percentage)
{
    SDL_Rect SourceRect = {};
    SourceRect.x = 0;
    SourceRect.y = 0;
    SourceRect.w = (u32)(Texture->Width * Percentage);
    SourceRect.h = (u32)(Texture->Height * Percentage);

    SDL_Rect DestRect = {};
    DestRect.x = X;
    DestRect.y = Y;
    DestRect.w = (u32)(Texture->Width * Percentage) * PixelScale;
    DestRect.h = (u32)(Texture->Height * Percentage) * PixelScale;

    SDL_BlitSurfaceScaled(
        Texture->Surface,
        &SourceRect,
        GlobalContext.WindowSurface,
        &DestRect,
        SDL_SCALEMODE_NEAREST);
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

internal void
TileMapDraw(tile_map *TileMap, u32 X, u32 Y, u32 Frame)
{
    u32 CurrentX = X;
    u32 CurrentY = Y;

    for(u32 IndexY = 0;
        IndexY < TileMap->SizeY;
        IndexY++)
    {
        for(u32 IndexX = 0;
            IndexX < TileMap->SizeX;
            IndexX++)
        {
            animation *Animation = &TileMap->Animations[TileMap->Layout[IndexY * TileMap->SizeX + IndexX]];

            if(Animation->FramesCount)
            {
                AnimationDraw(Animation, CurrentX, CurrentY, Frame);
            }

            CurrentX += TilePixelSize;
        }

        CurrentY += TilePixelSize;
        CurrentX = X;
    }
}

internal void
TileMapFillRandom(tile_map *TileMap, u32 X1, u32 Y1, u32 X2, u32 Y2, u8 First, u8 Count)
{
    u32 Pitch = TileMap->SizeX;
    u32 RowStart = X1 + Y1 * TileMap->SizeX;

    for(u32 Y = Y1;
        Y < Y2;
        Y++)
    {
        u32 CurrentElement = RowStart;
        for(u32 X = X1;
            X < X2;
            X++)
        {
            f32 Clamped = (f32)GetRandom() / (f32)MaxU32;

            u8 Type = 0;
            while(Clamped < 0.33)
            {
                Type++;

                if(Type >= Count - 1)
                {
                    break;
                }

                Clamped = (f32)GetRandom() / (f32)MaxU32;
            }

            TileMap->Layout[CurrentElement] = First + Type;

            CurrentElement++;
        }

        RowStart += Pitch;
    }
}

internal void
TileMapFill(tile_map *TileMap, u32 X1, u32 Y1, u32 X2, u32 Y2, u8 Index)
{
    u32 Pitch = TileMap->SizeX;
    u32 RowStart = X1 + Y1 * TileMap->SizeX;

    for(u32 Y = Y1;
        Y < Y2;
        Y++)
    {
        u32 CurrentElement = RowStart;
        for(u32 X = X1;
            X < X2;
            X++)
        {
            TileMap->Layout[CurrentElement] = Index;
            CurrentElement++;
        }

        RowStart += Pitch;
    }
}

internal void
TileMapFillWithTexture(tile_map *TileMap, u32 X1, u32 Y1, u32 X2, u32 Y2, u8 First)
{
    u32 Pitch = TileMap->SizeX;
    u32 RowStart = X1 + Y1 * TileMap->SizeX;
    u8 Index = First;

    for(u32 Y = Y1;
        Y < Y2;
        Y++)
    {
        u32 CurrentElement = RowStart;
        for(u32 X = X1;
            X < X2;
            X++)
        {
            TileMap->Layout[CurrentElement] = Index++;
            CurrentElement++;
        }

        RowStart += Pitch;
    }
}

internal b8
IsWalkable(collision_map *CollisionMap, u32 X, u32 Y)
{
    b8 Result = CollisionMap->Layout[Y * CollisionMap->SizeX + X];
    return(Result);
}

internal void
SpawnEnemies(u32 Count, f32 X, f32 Y, f32 Radius)
{
    GlobalContext.EnemiesCount = Count;
    GlobalContext.EnemiesRemaining = Count;

    for(u32 Index = 0;
        Index < Count;
        Index++)
    {
        GlobalContext.Enemies[Index].Type = GetRandom() % 2;

        f32 RandomAngle = (f32)GetRandom() / (f32)MaxU32 * 2 * Pi;

        GlobalContext.Enemies[Index].Position = V2(Cos(RandomAngle) * Radius + X, Abs(Sin(RandomAngle)) * Radius + Y);
    }
}

internal void
InitProjectiles(memory_arena *Arena, u32 Capacity)
{
    DoublyLinkedListInit(&GlobalContext.Projectiles);
    DoublyLinkedListInit(&GlobalContext.FreeProjectiles);

    projectile *Projectiles = MemoryArenaPushArray(Arena, projectile, 0, Capacity);
    for(u32 Index = 0;
        Index < Capacity;
        Index++)
    {
        DoublyLinkedListInsertAfter(&GlobalContext.FreeProjectiles, &Projectiles[Index]);
    }
}

internal void
ProjectileSpawn(v2 Position, v2 Velocity, b32 IsEnemy, u32 Type)
{
    projectile *Projectile = GlobalContext.FreeProjectiles.Next;

    DoublyLinkedListRemove(Projectile);
    DoublyLinkedListInsertAfter(&GlobalContext.Projectiles, Projectile);

    Projectile->IsEnemy = IsEnemy;
    Projectile->LastPosition = Position;
    Projectile->Position = Position;
    Projectile->Velocity = Velocity;
    Projectile->Type = Type;
}

internal b32
LineIntersection(f32 X1, f32 Y1, f32 X2, f32 Y2,
                    f32 X3, f32 Y3, f32 X4, f32 Y4) {
    f32 Denom = (X1 - X2) * (Y3 - Y4) - (Y1 - Y2) * (X3 - X4);
    if (Denom == 0.0f)
    {
        return false;
    }

    f32 T = ((X1 - X3) * (Y3 - Y4) - (Y1 - Y3) * (X3 - X4)) / Denom;
    f32 U = -((X1 - X2) * (Y1 - Y3) - (Y1 - Y2) * (X1 - X3)) / Denom;

    return (T >= 0 && T <= 1 && U >= 0 && U <= 1);
}

internal b32
RectLineIntersection(f32 PosX, f32 PosY, f32 Width, f32 Height,
                                 f32 StartX, f32 StartY, f32 EndX, f32 EndY) {
    f32 Left   = PosX;
    f32 Right  = PosX + Width;
    f32 Top    = PosY;
    f32 Bottom = PosY + Height;

    if (LineIntersection(StartX, StartY, EndX, EndY, Left, Top, Right, Top))
    {
        return true;
    }

    if (LineIntersection(StartX, StartY, EndX, EndY, Left, Bottom, Right, Bottom))
    {
        return true;
    }

    if (LineIntersection(StartX, StartY, EndX, EndY, Left, Top, Left, Bottom))
    {
        return true;
    }

    if (LineIntersection(StartX, StartY, EndX, EndY, Right, Top, Right, Bottom))
    {
        return true;
    }

    if (StartX >= Left && StartX <= Right && StartY >= Top && StartY <= Bottom &&
        EndX >= Left && EndX <= Right && EndY >= Top && EndY <= Bottom)
    {
        return true;
    }

    return false;
}

internal void
ProjectileUpdate(f32 DeltaTime)
{
    projectile *ProjectileNext = 0;
    for(projectile *Projectile = GlobalContext.Projectiles.Next;
        Projectile != &GlobalContext.Projectiles;
        Projectile = ProjectileNext)
    {
        ProjectileNext = Projectile->Next;

        Projectile->LastPosition = Projectile->Position;
        Projectile->Position = Projectile->Position + Projectile->Velocity * DeltaTime;

        b32 ShouldDelete = 0;

        if(Projectile->IsEnemy)
        {
            if(RectLineIntersection(
                GlobalContext.PlayerX - 0.5f,
                GlobalContext.PlayerY - 0.5f,
                1.0f,
                1.0f,
                Projectile->LastPosition.X,
                Projectile->LastPosition.Y,
                Projectile->Position.X,
                Projectile->Position.Y))
            {
                ShouldDelete = 1;

                if(GlobalContext.Health > GlobalContext.ProjectileDamage)
                {
                    GlobalContext.Health -= GlobalContext.ProjectileDamage;
                }
                else
                {
                    GlobalContext.Health = 0;
                }
            }
        }
        else
        {
            for(u32 EnemyIndex = 0; 
                EnemyIndex < GlobalContext.EnemiesCount;
                EnemyIndex++)
            {
                enemy *Enemy = &GlobalContext.Enemies[EnemyIndex];
    
                if(RectLineIntersection(
                    Enemy->Position.X - 0.5f,
                    Enemy->Position.X - 0.5f,
                    1.0f,
                    1.0f,
                    Projectile->LastPosition.X,
                    Projectile->LastPosition.Y,
                    Projectile->Position.X,
                    Projectile->Position.Y))
                {
                    ShouldDelete = 1;
                    
                    Enemy->Dead = 1;
                    GlobalContext.EnemiesRemaining--;
                }
            }
        }

        if(Projectile->Position.X < 0 || Projectile->Position.Y < 0 || Projectile->Position.X > 32 || Projectile->Position.Y > 32)
        {
            ShouldDelete = 1;
        }

        if(ShouldDelete)
        {
            DoublyLinkedListRemove(Projectile);
            DoublyLinkedListInsertBefore(&GlobalContext.FreeProjectiles, Projectile);
        }
    }
}

internal v2
GetScreenPos(v2 World)
{
    v2 Result = V2(
        -GlobalContext.PlayerX * TilePixelSize + WindowWidth / 2.0f + (f32)TilePixelSize * World.X,
        -GlobalContext.PlayerY * TilePixelSize + WindowHeight / 2.0f + (f32)TilePixelSize * World.Y);
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

    animation CharacterAnimation = AnimationCreate(&Arena, "assets/cuki_", 4);
    animation SeaAnimation = AnimationCreate(&Arena, "assets/sea", 16);

    animation Grass0 = AnimationCreate(&Arena, "assets/tile_0", 1);
    animation Grass1 = AnimationCreate(&Arena, "assets/tile_1", 1);
    animation Grass2 = AnimationCreate(&Arena, "assets/tile_2", 1);
    animation Grass3 = AnimationCreate(&Arena, "assets/tile_3", 1);
    
    animation ShallowWaterAnimation = AnimationCreate(&Arena, "assets/shallow_water", 4);

    animation Collide0 = AnimationCreate(&Arena, "assets/collide0", 1);
    animation Collide1 = AnimationCreate(&Arena, "assets/collide1", 1);
    animation Collide2 = AnimationCreate(&Arena, "assets/collide2", 1);
    animation Collide3 = AnimationCreate(&Arena, "assets/collide3", 1);
    animation Collide4 = AnimationCreate(&Arena, "assets/collide4", 1);
    animation Collide5 = AnimationCreate(&Arena, "assets/collide5", 1);
    animation Collide6 = AnimationCreate(&Arena, "assets/collide6", 1);
    animation Collide7 = AnimationCreate(&Arena, "assets/collide7", 1);

    animation Temple0 = AnimationCreate(&Arena, "assets/temple_0", 1);
    animation Temple1 = AnimationCreate(&Arena, "assets/temple_1", 1);
    animation Temple2 = AnimationCreate(&Arena, "assets/temple_2", 1);
    animation Temple3 = AnimationCreate(&Arena, "assets/temple_3", 1);
    animation Temple4 = AnimationCreate(&Arena, "assets/temple_4", 1);
    animation Temple5 = AnimationCreate(&Arena, "assets/temple_5", 1);
    animation Temple6 = AnimationCreate(&Arena, "assets/temple_6", 1);
    animation Temple7 = AnimationCreate(&Arena, "assets/temple_7", 1);
    animation Temple8 = AnimationCreate(&Arena, "assets/temple_8", 1);
    animation Temple9 = AnimationCreate(&Arena, "assets/temple_9", 1);
    animation Temple10 = AnimationCreate(&Arena, "assets/temple_10", 1);
    animation Temple11 = AnimationCreate(&Arena, "assets/temple_11", 1);
    animation Temple12 = AnimationCreate(&Arena, "assets/temple_12", 1);

    texture BlankHealthBar = TextureCreate("assets/blank_health_bar.bmp");
    texture HealthBar = TextureCreate("assets/health_bar.bmp");

    animation Fire = AnimationCreate(&Arena, "assets/fire", 1);
    animation Water = AnimationCreate(&Arena, "assets/water", 1);

    animation Ball = AnimationCreate(&Arena, "assets/ball", 1);

    animation Enemies[] =
    {
        Fire, Water
    };

    animation Projectiles[] =
    {
        Ball
    };

    animation MapAnimations[] =
    {
        {},

        SeaAnimation,
        ShallowWaterAnimation,

        Grass0, // 2
        Grass1,
        Grass2,
        Grass3,

        Collide0, // 6
        Collide1,
        Collide2,
        Collide3,
        Collide4,
        Collide5,
        Collide6,
        Collide7,

        {}, // 14
        Temple0,
        Temple1,
        {},

        Temple2, // 18
        Temple3,
        Temple4,
        {},

        Temple5, // 22
        Temple6,
        Temple7,
        Temple8,

        Temple9, // 26
        Temple10,
        Temple11,
        Temple12,
    };

    b8 WalkableBlocks[] = 
    {
        3, 4, 5, 6,
    };

    GlobalContext.Seed = 1847569;

    u32 MapSizeX = 24;
    u32 MapSizeY = 24;
    u32 MapMargin = 8;

    tile_map IslandMap = TileMapCreate(&Arena, MapSizeX, MapSizeY, MapAnimations, ArrayCount(MapAnimations));
    tile_map SeaMap = TileMapCreate(&Arena, MapSizeX, MapSizeY, MapAnimations, ArrayCount(MapAnimations));
    tile_map OverlayMap = TileMapCreate(&Arena, MapSizeX, MapSizeY, MapAnimations, ArrayCount(MapAnimations));

    TileMapFill(&SeaMap, 0, 0, MapSizeX, MapSizeY, 1);

    TileMapFillRandom(&IslandMap, MapMargin, MapMargin, MapSizeX - MapMargin, MapSizeY - MapMargin, 3, 4);

    TileMapFill(&IslandMap, MapMargin - 1, MapMargin, MapMargin, MapSizeY - MapMargin, 7 + 3);
    TileMapFill(&IslandMap, MapSizeX - MapMargin, MapMargin, MapSizeX - MapMargin + 1, MapSizeY - MapMargin, 7 + 4);

    TileMapFill(&IslandMap, MapMargin, MapMargin - 1, MapSizeY - MapMargin, MapMargin, 7 + 1);
    TileMapFill(&IslandMap, MapMargin, MapSizeY - MapMargin, MapSizeY - MapMargin, MapSizeY - MapMargin + 1, 7 + 6);

    TileMapFill(&IslandMap, MapMargin - 1, MapMargin - 1, MapMargin, MapMargin, 7 + 0);
    TileMapFill(&IslandMap, MapSizeY - MapMargin, MapMargin - 1, MapSizeY - MapMargin + 1, MapMargin, 7 + 2);

    TileMapFill(&IslandMap, MapMargin - 1, MapSizeY - MapMargin, MapMargin, MapSizeY - MapMargin + 1, 7 + 5);
    TileMapFill(&IslandMap, MapSizeY - MapMargin, MapSizeY - MapMargin, MapSizeY - MapMargin + 1, MapSizeY - MapMargin + 1, 7 + 7);

    TileMapFillWithTexture(&OverlayMap, MapSizeY / 2 - 2, MapMargin - 3, MapSizeY / 2 + 2, MapMargin + 1, 15);

    collision_map CollisionMap = CollisionMapCreate(&Arena, &IslandMap, WalkableBlocks, ArrayCount(WalkableBlocks));
    CollisionMapIntersection(&CollisionMap, &OverlayMap, WalkableBlocks, ArrayCount(WalkableBlocks));

    u32 SlowAnimFrame = 0;
    u32 FastAnimFrame = 0;

    GlobalContext.PlayerX = MapMargin + 0.5f;
    GlobalContext.PlayerY = MapMargin + 0.5f;

    u64 StartTime = SDL_GetPerformanceCounter();
    u64 LastTime = StartTime;

    GlobalContext.MovementSpeed = 5.0f;
    GlobalContext.EnemySpeed = 2.0f;

    GlobalContext.EnemiesCapacity = 1024;
    GlobalContext.Enemies = MemoryArenaPushArray(&Arena, enemy, 0, GlobalContext.EnemiesCapacity);
    
    GlobalContext.MaxHealth = 100;
    GlobalContext.Health = 100;
    GlobalContext.ProjectileDamage = 3;
    GlobalContext.ProjectileSpeed = 7.0f;
    GlobalContext.FireRate = 1.4f;
    GlobalContext.FireRateRandomFactor = 0.8f;
    GlobalContext.WaveCooldown = 8.0f;

    font Font = FontCreate("assets/fontatlas2.bmp", 5, 5);

    InitProjectiles(&Arena, 128);
    
    b32 Running = 1;
    while(Running)
    {
        u64 CurrentTime = SDL_GetPerformanceCounter();
        u64 Frequency = SDL_GetPerformanceFrequency();

        f32 DeltaTime = (f32)(CurrentTime - LastTime) / (f32)Frequency;
        f32 Time = (f32)(CurrentTime - StartTime) / (f32)Frequency;

        b32 ShouldShoot = 0;

        SDL_Event Event;
        while(SDL_PollEvent(&Event))
        {
            if(Event.type == SDL_EVENT_QUIT)
            {
                Running = 0;
            }

            if(!Event.key.repeat)
            {

                if(Event.type == SDL_EVENT_KEY_UP)
                {
                    switch(Event.key.key)
                    {
                        case 'w':
                        {
                            GlobalContext.DirectionY += 1;
                        } break;
    
                        case 's':
                        {
                            GlobalContext.DirectionY -= 1;
                        } break;
    
                        case 'd':
                        {
                            GlobalContext.DirectionX -= 1;
                        } break;
    
                        case 'a':
                        {
                            GlobalContext.DirectionX += 1;
                        } break;
                    }
                }
    
                if(Event.type == SDL_EVENT_KEY_DOWN)
                {
                    switch(Event.key.key)
                    {
                        case 'w':
                        {
                            GlobalContext.DirectionY -= 1;
                        } break;
    
                        case 's':
                        {
                            GlobalContext.DirectionY += 1;
                        } break;
    
                        case 'd':
                        {
                            GlobalContext.DirectionX += 1;
                        } break;
    
                        case 'a':
                        {
                            GlobalContext.DirectionX -= 1;
                        } break;
                    }
                }
            }

            if(Event.type == SDL_EVENT_MOUSE_MOTION)
            {
                GlobalContext.ViewDirection = Normalize(V2(Event.motion.x, Event.motion.y) - V2(WindowWidth / 2.0f, WindowHeight / 2.0f));
            }

            if(Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && Event.button.button == SDL_BUTTON_LEFT)
            {
                ShouldShoot = 1;
            }
        }

        f32 VelocityX = GlobalContext.DirectionX * DeltaTime * GlobalContext.MovementSpeed;

        GlobalContext.PlayerX += GlobalContext.DirectionX * DeltaTime * GlobalContext.MovementSpeed;
        if(GlobalContext.DirectionX > 0.0f)
        {
            if(!IsWalkable(&CollisionMap, (u32)GlobalContext.PlayerX, (u32)GlobalContext.PlayerY))
            {
                GlobalContext.PlayerX = (u32)GlobalContext.PlayerX - 0.01f;
                VelocityX = 0.0f;
            }
        }
        else if(GlobalContext.DirectionX < 0.0f)
        {
            if(!IsWalkable(&CollisionMap, (u32)GlobalContext.PlayerX, (u32)GlobalContext.PlayerY))
            {
                GlobalContext.PlayerX = (u32)GlobalContext.PlayerX + 1.0f;
                VelocityX = 0.0f;
            }
        }

        f32 VelocityY = GlobalContext.DirectionY * DeltaTime * GlobalContext.MovementSpeed;
        
        GlobalContext.PlayerY += GlobalContext.DirectionY * DeltaTime * GlobalContext.MovementSpeed;
        if(GlobalContext.DirectionY > 0.0f)
        {
            if(!IsWalkable(&CollisionMap, (u32)GlobalContext.PlayerX, (u32)GlobalContext.PlayerY))
            {
                GlobalContext.PlayerY = (f32)(u32)GlobalContext.PlayerY - 0.01f;
                VelocityY = 0.0f;
            }
        }
        else if(GlobalContext.DirectionY < 0.0f)
        {
            if(!IsWalkable(&CollisionMap, (u32)GlobalContext.PlayerX, (u32)GlobalContext.PlayerY))
            {
                GlobalContext.PlayerY = (u32)GlobalContext.PlayerY + 1.0f;
                VelocityY = 0.0f;
            }
        }

        SDL_ClearSurface(WindowSurface, 0, 0, 0, 1);

        u32 TileMapX = (u32)(-GlobalContext.PlayerX * TilePixelSize + WindowWidth / 2);
        u32 TileMapY = (u32)(-GlobalContext.PlayerY * TilePixelSize + WindowHeight / 2);

        TileMapDraw(&SeaMap, TileMapX, TileMapY, FastAnimFrame);
        TileMapDraw(&IslandMap, TileMapX, TileMapY, FastAnimFrame);
        TileMapDraw(&OverlayMap, TileMapX, TileMapY, FastAnimFrame);

        b32 ShouldRegenerateAngle = 0;
        if(GlobalContext.TimeOfLastRegenerate + 1.0f < Time)
        {
            ShouldRegenerateAngle = 1;
            GlobalContext.TimeOfLastRegenerate = Time;
        }

        for(u32 Index = 0;
            Index < GlobalContext.EnemiesCount;
            Index++)
        {
            enemy *Enemy = &GlobalContext.Enemies[Index];

            if(!Enemy->Dead)
            {
                animation *Animation = &Enemies[Enemy->Type];
    
                v2 Transformed = GetScreenPos(Enemy->Position);
                AnimationDraw(Animation, (u32)Transformed.X - TilePixelSize / 2, (u32)Transformed.Y - TilePixelSize / 2, SlowAnimFrame);
    
                v2 Target = V2(GlobalContext.PlayerX + Cos(Enemy->Angle) * 3, GlobalContext.PlayerY + Sin(Enemy->Angle) * 3);
    
                v2 Delta = Normalize(Target - V2(Enemy->Position.X, Enemy->Position.Y)) * GlobalContext.EnemySpeed * DeltaTime;
                Enemy->Position.X += Delta.X;
                Enemy->Position.Y += Delta.Y;
    
                if(ShouldRegenerateAngle)
                {
                    Enemy->Angle = (f32)GetRandom() / (f32)MaxU32 * 2 * Pi;
                }
    
                if(Time > Enemy->TimeOfNextShot)
                {
                    Enemy->TimeOfNextShot = Time + GlobalContext.FireRate + GlobalContext.FireRateRandomFactor * (f32)GetRandom() / (f32)MaxU32;
    
                    v2 ShotDelta = Normalize(V2(GlobalContext.PlayerX, GlobalContext.PlayerY) - Enemy->Position) * GlobalContext.ProjectileSpeed;
    
                    ProjectileSpawn(Enemy->Position, ShotDelta, 1, 0);
                }
            }
        }

        ProjectileUpdate(DeltaTime);

        for(projectile *Projectile = GlobalContext.Projectiles.Next;
            Projectile != &GlobalContext.Projectiles;
            Projectile = Projectile->Next)
        {
            v2 Transformed = GetScreenPos(Projectile->Position);
            animation *Animation = &Projectiles[Projectile->Type];
            AnimationDraw(Animation, (u32)Transformed.X - TilePixelSize / 2, (u32)Transformed.Y - TilePixelSize / 2, SlowAnimFrame);
        }

        if(ShouldShoot && GlobalContext.Health > 10)
        {
            GlobalContext.Health -= 10;
            ProjectileSpawn(V2(GlobalContext.PlayerX, GlobalContext.PlayerY) + V2(VelocityX, VelocityY), GlobalContext.ViewDirection * GlobalContext.ProjectileSpeed, 0, 0);
        }

        if(GlobalContext.EnemiesRemaining == 0 && !GlobalContext.IsWaitingForNextWave)
        {
            GlobalContext.TimeOfLastWaveEnd = Time;
            GlobalContext.IsWaitingForNextWave = 1;
        }

        if(GlobalContext.IsWaitingForNextWave && GlobalContext.TimeOfLastWaveEnd + GlobalContext.WaveCooldown < Time)
        {
            GlobalContext.IsWaitingForNextWave = 0;

            SpawnEnemies((GlobalContext.WaveIndex + 1) * 2 + GetRandom() % 2, CollisionMap.SizeX / 2.0f, CollisionMap.SizeY / 2.0f, 5);
        }

        AnimationDraw(&CharacterAnimation, WindowWidth / 2 - TilePixelSize / 2, WindowHeight / 2 - TilePixelSize, SlowAnimFrame);

        TextureDrawCustomPartial(&HealthBar, WindowWidth / 2 - PixelScale * BlankHealthBar.Width / 2 + 5 * PixelScale, WindowHeight - 100 + 2 * PixelScale, (f32)GlobalContext.Health / (f32)GlobalContext.MaxHealth);
        TextureDrawCustom(&BlankHealthBar, WindowWidth / 2 - PixelScale * BlankHealthBar.Width / 2, WindowHeight - 100);

        if(GlobalContext.PlayerX >= MapSizeY / 2 - 3 && GlobalContext.PlayerX <= MapSizeY / 2 + 3 && GlobalContext.PlayerY >= MapMargin - 4 && GlobalContext.PlayerY <= MapMargin + 2)
        {
            FontDraw(&Font, StringBundleZ("SPACE FOR MENU"), WindowWidth / 2, WindowHeight - 150, 1);
        }

        SDL_UpdateWindowSurface(Window);

        SlowAnimFrame = (u32)(Time * 2);
        FastAnimFrame = (u32)(Time * 3);

        LastTime = CurrentTime;
    }

    SDL_Quit();

    return(0);
}