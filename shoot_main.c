#include "shoot.h"

/**
 * 
 * WHEN: COMING: BACK: AFTER: AWHILE: DEBUG:
 *          Emscripten keeps throwing an "indirect call to null" error when I try to load a file that wasn't preloaded or doesn't exist
 *          for some other reason. Be aware of this possibility when seeing that error since it's the most vaguely worded error to ever exist
 *          and I don't want to spend ages looking for the issue again.
 * 
 * TODO: resize function fix (pieces should already exist go look in window.c and opengl.c etc.)
 * 
 * TODO: take out all dependancy on SCREEN_WIDTH & SCREEN_HEIGHT, replace with change-able values
 * 
 * TODO: add basic text glyph support using the font that you made for snakegame
 * 
 * TODO: add easier drawing function for positions in-between with accumulated time? (look up the formula again for that yes)
 * 
 * TODO: basic audio thing with openal??? (follow online tutorial lol and decide on what the demo thing is (probably constant looping music))
 * 
 * TODO: basic network socket thing (connect to given ip (localhost) and send message, and receive message)
 *      program 1 clicks green button in menu and starts listening, program 2 clicks yellow button and sends broadcast message
 *      program 1 sees message (end demo 1), program 1 sends hello message and program 2 receives hello and pings back
 *      (end demo 2, basically ready for multiplayer now)
 * 
 * TODO: basic multiplayer SOMETHING (maybe pong) that's local
 * TODO: the basic multiplayer again but networking support now
 * 
 *      Then feel free to just make a video game (ignore audio and networking for now)
 *          Start with pong or collect game (the one where things fall from sky and you have to collect to make number go up)
 * 
**/

/** NOTE: This is not necessary. Removing these and adding them to a build-script should work fine, if that's what you prefer.
 *  I think this looks and feels much nicer though. - chief **/
#include "shoot_opengl.c"
#include "shoot_generic.c"
#include "shoot_window.c"
#include "shoot_image.c"
#include "shoot_math.c"
#include "shoot_memory.c"
#include "shoot_sound.c"
#include "shoot_input.c"
#include "shoot_network.c"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static struct ShootWindow window;

static bool32 game_end = FALSE;

enum game_state {
    GAME_STATE_MENU,
    GAME_STATE_PONG,
};
enum game_state state = GAME_STATE_MENU;

/** ------------ PONG ------------ **/

static const real ball_width = 10.0, ball_height = 10.0, ball_start_speed = 50.0;
static const real player_height = 40.0, player_2_height = 40.0;

static real ball_x = SCREEN_WIDTH / 2, ball_y = SCREEN_HEIGHT / 2,
        ball_speed = ball_start_speed, ball_x_direction = 1.0, ball_y_direction = 1.0;
static real player_bottom = 0.0, player_top = player_height, player_width = 10.0, player_speed = 200.0;
static real player_2_bottom = 0.0, player_2_top = player_2_height, player_2_width = 10.0, player_2_speed = 200.0;

static void pong_loop()
{
    ball_speed += 5.0 * PHYSICS_TICK_SPEED;
    
    ball_x += ball_speed * ball_x_direction * PHYSICS_TICK_SPEED;
    ball_y += ball_speed * ball_y_direction * PHYSICS_TICK_SPEED;

    ball_speed += 50.0 * PHYSICS_TICK_SPEED;

    if (ball_x > SCREEN_WIDTH || ball_x < 0)
    {
        state = GAME_STATE_MENU;
        ball_x = SCREEN_WIDTH / 2;
        ball_y = SCREEN_HEIGHT / 2;
        ball_speed = ball_start_speed;
    }

    if (ball_y > SCREEN_HEIGHT - ball_height)
    {
        ball_y = SCREEN_HEIGHT - ball_height;
        ball_y_direction = -ball_y_direction;
    }
    if (ball_y < 0)
    {
        ball_y = 0;
        ball_y_direction = -ball_y_direction;
    }

    if (ball_x > SCREEN_WIDTH - player_2_width - ball_width && ball_y > player_2_bottom && ball_y < player_2_top)
    {
        ball_x = SCREEN_WIDTH - player_2_width - ball_width;
        ball_x_direction = -ball_x_direction;
    }
    if (ball_x < player_width && ball_y > player_bottom && ball_y < player_top)
    {
        ball_x = player_width;
        ball_x_direction = -ball_x_direction;
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

    if (shoot_input_get(&window.data->input[0], KEY_I))
    {
        player_2_bottom += player_2_speed * PHYSICS_TICK_SPEED;
        player_2_top = player_2_bottom + player_2_height;
    }
    if (shoot_input_get(&window.data->input[0], KEY_K))
    {
        player_2_bottom -= player_2_speed * PHYSICS_TICK_SPEED;
        player_2_top = player_2_bottom + player_2_height;
    }
    if (player_2_bottom < 0.0)
    {
        player_2_bottom = 0.0;
        player_2_top = player_2_bottom + player_2_height;
    }
    if (player_2_top > SCREEN_HEIGHT)
    {
        player_2_top = SCREEN_HEIGHT;
        player_2_bottom = player_2_top - player_2_height;
    }
}
static void pong_render()
{
    shoot_image_draw_rect(&window.data->screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.2, 0.7, 0.8, 1.0);

    shoot_image_draw_rect(&window.data->screen, ball_x, ball_y, ball_x + ball_width, ball_y + ball_height, 1.0, 1.0, 1.0, 1.0);
    shoot_image_draw_rect(&window.data->screen, 0, player_bottom,
                    player_width, player_top, 1.0, 1.0, 1.0, 1.0);
    shoot_image_draw_rect(&window.data->screen, SCREEN_WIDTH - player_2_width, player_2_bottom,
                    SCREEN_WIDTH, player_2_top, 1.0, 1.0, 1.0, 1.0);
}

/** ------------ MENU ------------ **/

static int32 mouseX, mouseY;

static bool32 pong_hovered, pong_held, server_open_hovered, server_open_held, connect_hovered, connect_held;
enum ShootNetStatus {
    SHOOT_NET_STATUS_OFFLINE,
    SHOOT_NET_STATUS_OPEN_TO_LAN,
    SHOOT_NET_STATUS_SEARCHING_FOR_CONNECTION,
    SHOOT_NET_STATUS_CONNECTING,
    SHOOT_NET_STATUS_CONNECTED,
};
static enum ShootNetStatus network_state;
static bool32 network_setup;

struct ShootRect pong_button = {20, 20, 60, 60};
struct ShootRect server_open_button = {20, 120, 60, 160};
struct ShootRect connect_button = {120, 120, 160, 160};

static void shoot_button_update(struct ShootRect rect, bool32 *hovered, bool32 *held)
{
    if (mouseX > rect.left && mouseY > rect.bottom && mouseX < rect.right && mouseY < rect.top)
    {
        *hovered = TRUE;
        if (shoot_input_get(&window.data->input[0], MOUSE_BUTTON_LEFT)) { *held = TRUE; }
        else { *held = FALSE; }
    }
    else
    {
        *hovered = FALSE;
        *held = FALSE;
    }
}
static void shoot_toggle_update(struct ShootRect rect, bool32 *hovered, bool32 *held, int32 *toggled, int32 new_toggle_state)
{
    if (*toggled) { return; }

    if (mouseX > rect.left && mouseY > rect.bottom && mouseX < rect.right && mouseY < rect.top)
    {
        *hovered = TRUE;
        if (shoot_input_get(&window.data->input[0], MOUSE_BUTTON_LEFT)) { *held = TRUE; }
        else { *held = FALSE; }
    }
    else
    {
        *hovered = FALSE;
        *held = FALSE;
    }

    if (*hovered && shoot_input_just_released(&window.data->input[0], MOUSE_BUTTON_LEFT))
    {
        *toggled = new_toggle_state;
    }
}
static void menu_loop()
{
    real mouseXDiv = (real)INT16_MAX / (real)SCREEN_WIDTH;
    real mouseYDiv = (real)INT16_MAX / (real)SCREEN_HEIGHT;
    mouseX = (int16)((real)shoot_input_get(&window.data->input[0], MOUSE_X) / mouseXDiv);
    mouseY = (int16)((real)shoot_input_get(&window.data->input[0], MOUSE_Y) / mouseYDiv);

    shoot_button_update(pong_button, &pong_hovered, &pong_held);
    shoot_toggle_update(server_open_button, &server_open_hovered, &server_open_held, (int32 *)&network_state, SHOOT_NET_STATUS_OPEN_TO_LAN);
    shoot_toggle_update(connect_button, &connect_hovered, &connect_held, (int32 *)&network_state, SHOOT_NET_STATUS_SEARCHING_FOR_CONNECTION);

    if (shoot_input_get(&window.data->input[0], KEY_SPACE))
        printf("%i - %i\n", window.data->input[0].currentState[MOUSE_BUTTON_LEFT], window.data->input[0].lastState[MOUSE_BUTTON_LEFT]);
        
    if (pong_hovered && shoot_input_just_released(&window.data->input[0], MOUSE_BUTTON_LEFT))
    {
        state = GAME_STATE_PONG;
    }
    if (network_state && !network_setup)
    {
        NET_STARTUP
        network_setup = TRUE;
    }
}
static void shoot_button_draw(struct ShootRect rect, real red, real green, real blue, bool32 hovered, bool32 held)
{
    shoot_image_draw_rect(&window.data->screen, rect.left, rect.bottom, rect.right, rect.top,
            red * (1.0 + (hovered * 0.1)) * (1.0 - (held * 0.25)),
            green * (1.0 + (hovered * 0.1)) * (1.0 - (held * 0.25)),
            blue * (1.0 + (hovered * 0.1)) * (1.0 - (held * 0.25)), 1.0);
}
static void shoot_toggle_draw(struct ShootRect rect, real red, real green, real blue, bool32 hovered, bool32 held, bool32 toggled)
{
    shoot_image_draw_rect(&window.data->screen, rect.left, rect.bottom, rect.right, rect.top,
            red * (1.0 + (hovered * 0.1)) * (1.0 - (held * 0.25)) * (1.0 - 0.7 * (toggled != 0)),
            green * (1.0 + (hovered * 0.1)) * (1.0 - (held * 0.25)) * (1.0 - 0.7 * (toggled != 0)),
            blue * (1.0 + (hovered * 0.1)) * (1.0 - (held * 0.25)) * (1.0 - 0.7 * (toggled != 0)), 1.0);
}
static void menu_render()
{
    shoot_image_draw_rect(&window.data->screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.2, 0.25, 0.2, 1.0);

    shoot_button_draw(pong_button, 0.4, 0.7, 0.7, pong_hovered, pong_held);

    shoot_toggle_draw(server_open_button, 0.4, 0.9, 0.8, server_open_hovered, server_open_held, network_state);
    shoot_toggle_draw(connect_button, 0.4, 0.8, 0.9, connect_hovered, connect_held, network_state);

    shoot_image_draw_rect(&window.data->screen, mouseX - 5, mouseY - 5, mouseX + 5, mouseY + 5, 0.0, 0.0, 0.0, 1.0);
}

static bool32 game_loop()
{
    if (shoot_input_just_released(&window.data->input[0], KEY_ESC))
    {
        state = GAME_STATE_MENU;
    }
    switch(state)
    {
        case GAME_STATE_MENU:
            menu_loop();
            break;
        case GAME_STATE_PONG:
            pong_loop();
            break;
        default:
            game_end = TRUE;
            break;
    }

    window.data->accumulated_time -= PHYSICS_TICK_SPEED;
    if (window.data->accumulated_time >= PHYSICS_TICK_SPEED) { return TRUE; }
    return FALSE;
}
static void game_render()
{
    switch(state)
    {
        case GAME_STATE_MENU:
            menu_render();
            break;
        case GAME_STATE_PONG:
            pong_render();
            break;
        default:
            game_end = TRUE;
            break;
    }
}

static void main_loop()
{
    shoot_window_clear(window);

    shoot_window_start_physics_tick(window);

    while (game_loop()) {}
    game_render();

    if (game_end)
    {
        shoot_window_close(window);
        return;
    }

    shoot_window_draw(window);
    shoot_window_poll_events(window);
}

int main()
{
    shoot_check_compatibility();

    window = shoot_window_setup(0);
    shoot_opengl_window_setup(&window);
    
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop, 0, 1);
#else
    while(shoot_window_is_open(window))
    {
        main_loop();
    }
#endif

    if (network_setup)
    {
        NET_SHUTDOWN
    }
    shoot_window_close(window);

    return 0;
}