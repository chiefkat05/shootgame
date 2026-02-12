#include "shoot.h"

static void *shoot_arena_grab_memory(struct ShootArena *arena, uint64 bytes)
{
    if (arena->cursor + bytes > arena->memsize)
    {
        printf("%p ran out of memory\n", arena);
        exit(1);
    }
    void *out = (arena->memory + arena->cursor);
    arena->cursor += bytes;

    return out;
}
static void shoot_arena_alloc(struct ShootArena *arena, uint64 bytes)
{
    *arena = (struct ShootArena){};
    arena->memory = malloc(bytes);
    arena->memsize = bytes;
    arena->cursor = 0;
}
static void shoot_arena_free(struct ShootArena *arena)
{
    if (arena->memory) { free(arena->memory); }
}

static void shoot_copy(void *dest, void *src, uint64 bytes)
{
    uint32 i;
    for (i = 0; i < bytes; ++i)
    { ((uint8 *)dest)[i] = ((uint8 *)src)[i]; }
}
static void shoot_copy_backwards(void *dest, void *src, uint64 bytes)
{
    uint32 i;
    for (i = 0; i < bytes; ++i)
    { ((uint8 *)dest)[bytes - (i + 1)] = ((uint8 *)src)[i]; }
}
static void shoot_copy32(uint32 *dest, uint32 *src, uint64 count)
{
    uint32 i;
    for (i = 0; i < count; ++i)
    { ((uint32 *)dest)[i] = ((uint32 *)src)[i]; }
}
static void shoot_copy32_backwards(uint32 *dest, uint32 *src, uint64 count)
{
    uint32 i;
    for (i = 0; i < count; ++i)
    { ((uint32 *)dest)[count - (i + 1)] = ((uint32 *)src)[i]; }
}