#include "shoot.h"

/**
 * 
 * TODO: Add timestep implementation for the window, write usage code in here first as you know and make it worry-free for the user (developer)
 * 
 *      Then feel free to just make a video game (ignore audio and networking for now)
 *          Start with pong or collect game (the one where things fall from sky and you have to collect to make number go up)
 * 
**/

/** NOTE: This is not necessary. Removing these and adding them to a build-script should work fine, if that's what you prefer.
 *  I think this looks and feels much nicer though. - chief **/
#include "shoot_opengl.c"
#include "shoot_generic.c"
#include "shoot_image.c"
#include "shoot_math.c"
#include "shoot_memory.c"
#include "shoot_sound.c"
#include "shoot_window.c"
#include "shoot_input.c"

/** NOTE: The game proper. - chief **/
#include "shoot_system.c"

static struct ShootWindow window;

static const real ball_width = 10.0, ball_height = 10.0;
static const real player_height = 40.0;

static real ball_x = 0.0, ball_y = 0.0, ball_speed_x = 100.0, ball_speed_y = 100.0;
static real player_bottom = 0.0, player_top = player_height, player_width = 10.0, player_speed = 200.0;

static bool32 game_end = FALSE;

static bool32 game_loop()
{
    ball_x += ball_speed_x * PHYSICS_TICK_SPEED;
    ball_y += ball_speed_y * PHYSICS_TICK_SPEED;

    if (ball_x > SCREEN_WIDTH)
    {
        printf("game over!\n");
        game_end = TRUE;
    }

    if (ball_y > SCREEN_HEIGHT - ball_height)
    {
        ball_y = SCREEN_HEIGHT - ball_height;
        ball_speed_y = -ball_speed_y;
    }
    if (ball_y < 0)
    {
        ball_y = 0;
        ball_speed_y = -ball_speed_y;
    }

    if (ball_x > SCREEN_WIDTH - player_width - ball_width && ball_y > player_bottom && ball_y < player_top)
    {
        ball_x = SCREEN_WIDTH - player_width - ball_width;
        ball_speed_x = -ball_speed_x;
    }
    if (ball_x < 0)
    {
        ball_x = 0;
        ball_speed_x = -ball_speed_x;
    }

    if (shoot_input_get(&window.data->input[0], KEY_W))
    {
        player_bottom += player_speed * PHYSICS_TICK_SPEED;
        player_top = player_bottom + player_height;
    }
    if (shoot_input_get(&window.data->input[0], KEY_S))
    {
        player_bottom -= player_speed * PHYSICS_TICK_SPEED;
        player_top = player_bottom + player_height;
    }
    if (player_bottom < 0.0)
    {
        player_bottom = 0.0;
        player_top = player_bottom + player_height;
    }
    if (player_top > SCREEN_HEIGHT)
    {
        player_top = SCREEN_HEIGHT;
        player_bottom = player_top - player_height;
    }

    window.data->accumulated_time -= PHYSICS_TICK_SPEED;
    if (window.data->accumulated_time >= PHYSICS_TICK_SPEED) { return TRUE; }
    return FALSE;
}
static void game_render()
{
    shoot_image_draw_rect(&window.data->screen, ball_x, ball_y, ball_x + ball_width, ball_y + ball_height, 255, 255, 0, 255);
    shoot_image_draw_rect(&window.data->screen, SCREEN_WIDTH - player_width, player_bottom,
                    SCREEN_WIDTH, player_top, 255, 255, 255, 255);
}

int main()
{
    shoot_system_check_compatibility();

    glfwInit();
    window = shoot_window_setup(0);
    shoot_opengl_window_setup(&window);
    
    while(shoot_window_is_open(window))
    {
        shoot_window_clear(window);

        shoot_window_poll_events(window);

        while (game_loop()) {}
        game_render();

        if (game_end)
        {
            shoot_window_close(window);
            return 0;
        }

        shoot_window_draw(window);
    }

    shoot_window_close(window);

    return 0;
}