#include "shoot.h"

static void shoot_system_check_compatibility()
{
    if (sizeof(long) != 8)
    {
        printf("WARNING: sizeof(long) is not 8 bytes, this game uses 8 byte longs in a few places.\n"
                "Game will still open but might not work as expected.\n");
    }
    if (sizeof(int) != 4)
    {
        printf("CRITICAL WARNING: sizeof(int) is not 4 bytes, this game relies on 4 byte integers to work properly.\n"
                "Game will still open but will almost certainly not work.\n");
    }
    if (sizeof(short) != 2)
    {
        printf("CRITICAL WARNING: sizeof(short) is not 2 bytes, this game relies on 2 byte shorts to work properly.\n"
                "Game will still open but will almost certainly not work.\n");
    }
    if (sizeof(char) != 1)
    {
        printf("CRITICAL WARNING: sizeof(char) is not 1 byte, this game relies on 1 byte chars to work properly.\n"
                "Game will still open but will almost certainly not work.\n");
    }
}

static struct ShootSystem shoot_system_open()
{
    struct ShootSystem out_system = (struct ShootSystem){};

    return out_system;
}

static void shoot_system_load_scene(struct ShootSystem *system, enum ShootSystemScene scene)
{
    
}

static void shoot_system_update_scene(struct ShootSystem *system)
{

}
static void shoot_system_draw_to_window(struct ShootSystem *system, struct ShootWindow window)
{
    
}