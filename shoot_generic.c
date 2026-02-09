#include "shoot.h"

static const char *shoot_read_file_string(struct ShootArena *memory, const char *path)
{
    FILE *f = fopen(path, "rb");
    verify(f != NULL, "failed to open file (might not exist)");
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *out = shoot_arena_grab_memory(memory, length + 1);
    fread(out, length, 1, f);
    *(out + length) = '\0';

    fclose(f);
    return out;
}
static void *shoot_read_file_raw(struct ShootArena *memory, const char *path)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    void *out = shoot_arena_grab_memory(memory, length);
    fread(out, length, 1, f);

    fclose(f);
    return out;
}

static bool32 shoot_is_string_equal(const char *strA, const char *strB, uint32 strLength)
{
    const char *cursorA = strA;
    const char *cursorB = strB;

    int i;
    for (i = 0; i < strLength; ++i)
    {
        if (*cursorA != *cursorB) { return FALSE; }

        ++cursorA;
        ++cursorB;
    }

    return TRUE;
}