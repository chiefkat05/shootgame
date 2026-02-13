#include "shoot.h"

/**
 * 
 * WHEN: COMING: BACK: AFTER: AWHILE: DEBUG:
 *          Emscripten keeps throwing an "indirect call to null" error when I try to load a file that wasn't preloaded or doesn't exist
 *          for some other reason. Be aware of this possibility when seeing that error since it's the most vaguely worded error to ever exist
 *          and I don't want to spend ages looking for the issue again.
 * 
 * TODO: build glad.c into wasm object with emscripten so you can link it in build_web and build_game
 * 
 * TODO: take out all dependancy on SCREEN_WIDTH & SCREEN_HEIGHT, replace with change-able values
 * 
 * TODO: add basic text glyph support using the font that you made for snakegame
 * 
 * TODO: add easier drawing function for positions in-between with accumulated time? (look up the formula again for that yes)
 * 
 * TODO: basic audio thing with openal??? (follow online tutorial lol and decide on what the demo thing is (probably constant looping music))
 * 
 * TODOC: implement broadcast message when joining as player 1 or 2, and store the ip address, setting the destination socket to that.
 * TODOD: test cross-computers
 * 
 * 
 *      Then feel free to just make a video game (ignore audio for now)
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

/** ------------ GAME ------------ **/

static struct ShootWindow window;

static bool32 game_end = FALSE;

enum ShootGameState {
    GAME_STATE_NULL,
    GAME_STATE_MENU,
    GAME_STATE_PONG,
};
enum ShootGameState state = GAME_STATE_MENU;

enum ShootNetStatus {
    SHOOT_NET_STATUS_OFFLINE,
    SHOOT_NET_STATUS_PLAYER_ONE,
    SHOOT_NET_STATUS_PLAYER_TWO,
};

#define SHOOT_NET_BROADCAST_ADDRESS "0.0.0.0"
#define SHOOT_NET_PORT "4444"
#define SHOOT_NET_HEADER_ID "SHOOT"
struct ShootNetHeader {
    char ID[5];
    // char hostname[128], port[32];
    enum ShootGameState state;
    bool32 was_here_first;
};

static enum ShootNetStatus network_state;
static bool32 network_setup, network_up_to_date_with_peers;
fd_set network_master_set;
SOCKET network_socket, destination_socket = -1, max_socket;
struct addrinfo *network_address, *peer_address;
char broadcast_address[100];

static void shoot_game_change_state(enum ShootGameState new_state)
{
    state = new_state;

    if (network_state != SHOOT_NET_STATUS_OFFLINE)
    {
        network_up_to_date_with_peers = FALSE;
    }
}
static struct ShootNetHeader shoot_net_make_send_header()
{
    char hostname_buffer[128], port_buffer[32];
    shoot_net_get_address_ip(network_address, hostname_buffer, 128, port_buffer, 32);

    struct ShootNetHeader send_header = { .ID = SHOOT_NET_HEADER_ID, .state = state };
    // strcpy(send_header.hostname, hostname_buffer);
    // strcpy(send_header.port, port_buffer);
        
    return send_header;
}

/** ------------ PONG ------------ **/

static const real ball_width = 10.0, ball_height = 10.0, ball_speed = 50.0;
static const real player_height = 40.0, player_width = 10.0, player_speed = 200.0;

struct ShootPongPlayer
{
    real bottom, top;
};
struct ShootPongBall
{
    real ball_x, ball_y;
    real ball_x_direction, ball_y_direction;
};

struct ShootPongGameData
{
    struct ShootPongPlayer player_one, player_two;
    struct ShootPongBall ball;
};
static struct ShootPongGameData pong_data = {
    .player_one = {.bottom = 0.0, .top = player_height},
    .player_two = {.bottom = 0.0, .top = player_height},
    .ball = {.ball_x = SCREEN_WIDTH / 2, .ball_y = SCREEN_HEIGHT / 2,
    .ball_x_direction = 1.0, .ball_y_direction = 1.0}};

static void pong_loop()
{
    pong_data.ball.ball_x += ball_speed * pong_data.ball.ball_x_direction * PHYSICS_TICK_SPEED;
    pong_data.ball.ball_y += ball_speed * pong_data.ball.ball_y_direction * PHYSICS_TICK_SPEED;

    if (pong_data.ball.ball_x > SCREEN_WIDTH || pong_data.ball.ball_x < 0)
    {
        pong_data.ball.ball_x = SCREEN_WIDTH / 2;
        pong_data.ball.ball_y = SCREEN_HEIGHT / 2;
    }

    if (pong_data.ball.ball_y > SCREEN_HEIGHT - ball_height)
    {
        pong_data.ball.ball_y = SCREEN_HEIGHT - ball_height;
        pong_data.ball.ball_y_direction = -pong_data.ball.ball_y_direction;
    }
    if (pong_data.ball.ball_y < 0)
    {
        pong_data.ball.ball_y = 0;
        pong_data.ball.ball_y_direction = -pong_data.ball.ball_y_direction;
    }

    if (pong_data.ball.ball_x > SCREEN_WIDTH - player_width - ball_width &&
        pong_data.ball.ball_y > pong_data.player_two.bottom && pong_data.ball.ball_y < pong_data.player_two.top)
    {
        pong_data.ball.ball_x = SCREEN_WIDTH - player_width - ball_width;
        pong_data.ball.ball_x_direction = -pong_data.ball.ball_x_direction;
    }
    if (pong_data.ball.ball_x < player_width && pong_data.ball.ball_y > pong_data.player_one.bottom &&
        pong_data.ball.ball_y < pong_data.player_one.top)
    {
        pong_data.ball.ball_x = player_width;
        pong_data.ball.ball_x_direction = -pong_data.ball.ball_x_direction;
    }

    if (shoot_input_get(&window.data->input[0], KEY_W))
    {
        pong_data.player_one.bottom += player_speed * PHYSICS_TICK_SPEED;
        pong_data.player_one.top = pong_data.player_one.bottom + player_height;
    }
    if (shoot_input_get(&window.data->input[0], KEY_S))
    {
        pong_data.player_one.bottom -= player_speed * PHYSICS_TICK_SPEED;
        pong_data.player_one.top = pong_data.player_one.bottom + player_height;
    }
    if (pong_data.player_one.bottom < 0.0)
    {
        pong_data.player_one.bottom = 0.0;
        pong_data.player_one.top = pong_data.player_one.bottom + player_height;
    }
    if (pong_data.player_one.top > SCREEN_HEIGHT)
    {
        pong_data.player_one.top = SCREEN_HEIGHT;
        pong_data.player_one.bottom = pong_data.player_one.top - player_height;
    }

    if (shoot_input_get(&window.data->input[0], KEY_I))
    {
        pong_data.player_two.bottom += player_speed * PHYSICS_TICK_SPEED;
        pong_data.player_two.top = pong_data.player_two.bottom + player_height;
    }
    if (shoot_input_get(&window.data->input[0], KEY_K))
    {
        pong_data.player_two.bottom -= player_speed * PHYSICS_TICK_SPEED;
        pong_data.player_two.top = pong_data.player_two.bottom + player_height;
    }
    if (pong_data.player_two.bottom < 0.0)
    {
        pong_data.player_two.bottom = 0.0;
        pong_data.player_two.top = pong_data.player_two.bottom + player_height;
    }
    if (pong_data.player_two.top > SCREEN_HEIGHT)
    {
        pong_data.player_two.top = SCREEN_HEIGHT;
        pong_data.player_two.bottom = pong_data.player_two.top - player_height;
    }

    if (network_state == SHOOT_NET_STATUS_PLAYER_ONE)
    {
        struct ShootPongGameData peer_pong_data = {};
        struct ShootNetHeader peer_header = {};
        shoot_net_poll(network_socket, max_socket, &peer_header, sizeof(peer_header), 0, 0, 0, 0);
        shoot_net_poll(network_socket, max_socket, &peer_pong_data, sizeof(peer_pong_data), 0, 0, 0, 0);

        if (shoot_is_string_equal(peer_header.ID, SHOOT_NET_HEADER_ID, sizeof(SHOOT_NET_HEADER_ID) - 1) && peer_header.state == GAME_STATE_PONG)
        {
            if (peer_header.was_here_first && !network_up_to_date_with_peers)
            {
                printf("other player was here first\n");
                pong_data = peer_pong_data;
                network_up_to_date_with_peers = TRUE;
            }
            else
            {
                pong_data.player_two = peer_pong_data.player_two;
            }
        }

        struct ShootNetHeader send_header = { .ID = SHOOT_NET_HEADER_ID, .state = state,
            .was_here_first = !(peer_header.was_here_first) };

        if (send_header.was_here_first)
        {
            network_up_to_date_with_peers = TRUE;
        }

        shoot_net_send(destination_socket, peer_address, &send_header, sizeof(send_header));
        shoot_net_send(destination_socket, peer_address, &pong_data, sizeof(pong_data));
    }
    if (network_state == SHOOT_NET_STATUS_PLAYER_TWO)
    {
        struct ShootPongGameData peer_pong_data = {};
        struct ShootNetHeader peer_header = {};
        shoot_net_poll(network_socket, max_socket, &peer_header, sizeof(peer_header), 0, 0, 0, 0);
        shoot_net_poll(network_socket, max_socket, &peer_pong_data, sizeof(peer_pong_data), 0, 0, 0, 0);

        if (shoot_is_string_equal(peer_header.ID, SHOOT_NET_HEADER_ID, sizeof(SHOOT_NET_HEADER_ID) - 1) && peer_header.state == GAME_STATE_PONG)
        {
            if (peer_header.was_here_first && !network_up_to_date_with_peers)
            {
                printf("other player was here first\n");
                pong_data = peer_pong_data;
                network_up_to_date_with_peers = TRUE;
            }
            else
            {
                pong_data.ball = peer_pong_data.ball;
                pong_data.player_one = peer_pong_data.player_one;
            }
        }

        struct ShootNetHeader send_header = { .ID = SHOOT_NET_HEADER_ID, .state = state,
            .was_here_first = !(peer_header.was_here_first) };

        if (send_header.was_here_first)
        {
            network_up_to_date_with_peers = TRUE;
        }

        shoot_net_send(destination_socket, peer_address, &send_header, sizeof(send_header));
        shoot_net_send(destination_socket, peer_address, &pong_data, sizeof(pong_data));
    }
}
static void pong_render()
{
    shoot_image_draw_rect(&window.data->screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.2, 0.7, 0.8, 1.0);

    shoot_image_draw_rect(&window.data->screen, pong_data.ball.ball_x, pong_data.ball.ball_y,
        pong_data.ball.ball_x + ball_width, pong_data.ball.ball_y + ball_height, 1.0, 1.0, 1.0, 1.0);

    shoot_image_draw_rect(&window.data->screen, 0, pong_data.player_one.bottom,
                    player_width, pong_data.player_one.top, 1.0, 1.0, 1.0, 1.0);
    shoot_image_draw_rect(&window.data->screen, SCREEN_WIDTH - player_width, pong_data.player_two.bottom,
                    SCREEN_WIDTH, pong_data.player_two.top, 1.0, 1.0, 1.0, 1.0);
}

/** ------------ MENU ------------ **/

struct ShootPlayerCursor
{
    int32 mouseX, mouseY;
};
static struct ShootPlayerCursor menu_player_one, menu_player_two;

static bool32 pong_hovered, pong_held, server_open_hovered, server_open_held, connect_hovered, connect_held;

struct ShootRect pong_button = {20, 20, 60, 60};
struct ShootRect server_open_button = {20, 120, 60, 160};
struct ShootRect connect_button = {120, 120, 160, 160};

static void shoot_button_update(struct ShootRect rect, struct ShootPlayerCursor cursor, bool32 *hovered, bool32 *held)
{
    if (cursor.mouseX > rect.left && cursor.mouseY > rect.bottom && cursor.mouseX < rect.right && cursor.mouseY < rect.top)
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
static void shoot_toggle_update(struct ShootRect rect, struct ShootPlayerCursor cursor, bool32 *hovered, bool32 *held,
        int32 *toggled, int32 new_toggle_state)
{
    if (*toggled) { return; }

    if (cursor.mouseX > rect.left && cursor.mouseY > rect.bottom && cursor.mouseX < rect.right && cursor.mouseY < rect.top)
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

    if (network_state == SHOOT_NET_STATUS_OFFLINE || network_state == SHOOT_NET_STATUS_PLAYER_ONE)
    {
        menu_player_one.mouseX = (int16)((real)shoot_input_get(&window.data->input[0], MOUSE_X) / mouseXDiv);
        menu_player_one.mouseY = (int16)((real)shoot_input_get(&window.data->input[0], MOUSE_Y) / mouseYDiv);
        
        shoot_button_update(pong_button, menu_player_one, &pong_hovered, &pong_held);
        shoot_toggle_update(server_open_button, menu_player_one, &server_open_hovered, &server_open_held,
            (int32 *)&network_state, SHOOT_NET_STATUS_PLAYER_ONE);
        shoot_toggle_update(connect_button, menu_player_one, &connect_hovered, &connect_held, (int32 *)&network_state,
            SHOOT_NET_STATUS_PLAYER_TWO);
    }
    if (network_state == SHOOT_NET_STATUS_PLAYER_TWO)
    {
        menu_player_two.mouseX = (int16)((real)shoot_input_get(&window.data->input[0], MOUSE_X) / mouseXDiv);
        menu_player_two.mouseY = (int16)((real)shoot_input_get(&window.data->input[0], MOUSE_Y) / mouseYDiv);
        
        shoot_button_update(pong_button, menu_player_two, &pong_hovered, &pong_held);
        shoot_toggle_update(server_open_button, menu_player_two, &server_open_hovered, &server_open_held,
            (int32 *)&network_state, SHOOT_NET_STATUS_PLAYER_ONE);
        shoot_toggle_update(connect_button, menu_player_two, &connect_hovered, &connect_held, (int32 *)&network_state,
            SHOOT_NET_STATUS_PLAYER_TWO);
    }

    if (pong_hovered && shoot_input_just_released(&window.data->input[0], MOUSE_BUTTON_LEFT))
    {
        shoot_game_change_state(GAME_STATE_PONG);
    }
    if (network_state && !network_setup)
    {
        NET_STARTUP
        network_setup = TRUE;

        switch(network_state)
        {
            case SHOOT_NET_STATUS_PLAYER_ONE:
            {
                printf("connecting to local area network as player one\n");
                network_socket = shoot_net_open_listening_socket(0, SHOOT_NET_PORT, &network_address);

                // struct ShootNetHeader send_header = shoot_net_make_send_header();
                destination_socket = shoot_net_open_peer_socket(broadcast_address, SHOOT_NET_PORT, &peer_address);
                // shoot_net_broadcast(broadcast_address, SHOOT_NET_PORT, &send_header, sizeof(send_header));

                if (!ISVALIDSOCKET(network_socket))
                {
                    printf("Failed to join local network as player one, that player probably already exists.\n");
                    network_state = SHOOT_NET_STATUS_OFFLINE;
                    NET_SHUTDOWN;
                    network_setup = FALSE;
                    break;
                }

                max_socket = network_socket;

                FD_ZERO(&network_master_set);
                FD_SET(network_socket, &network_master_set);
            } break;
            case SHOOT_NET_STATUS_PLAYER_TWO:
            {
                printf("connecting to local area network as player two\n");
                network_socket = shoot_net_open_listening_socket(0, SHOOT_NET_PORT, &network_address);

                // struct ShootNetHeader send_header = shoot_net_make_send_header();
                destination_socket = shoot_net_open_peer_socket(broadcast_address, SHOOT_NET_PORT, &peer_address);
                // shoot_net_broadcast(broadcast_address, SHOOT_NET_PORT, &send_header, sizeof(send_header));
                
                if (!ISVALIDSOCKET(network_socket))
                {
                    printf("Failed to join local network as player two, that player probably already exists.\n");
                    network_state = SHOOT_NET_STATUS_OFFLINE;
                    NET_SHUTDOWN;
                    network_setup = FALSE;
                    break;
                }

                max_socket = network_socket;

                FD_ZERO(&network_master_set);
                FD_SET(network_socket, &network_master_set);
            } break;
            default:
                break;
        }
    }

    static int counter = 0;
    ++counter;
    if (network_state == SHOOT_NET_STATUS_PLAYER_ONE)
    {
        struct ShootNetHeader temp_header = {};
        struct ShootPlayerCursor temp_cursor = {};

        char temp_hostname[128], temp_port[32];
        shoot_net_poll(network_socket, max_socket, &temp_header, sizeof(temp_header), temp_hostname, 128, temp_port, 32);
        bool32 cursor_exists = shoot_net_poll(network_socket, max_socket, &temp_cursor, sizeof(temp_cursor), 0, 0, 0, 0);

        if (shoot_is_string_equal(temp_header.ID, SHOOT_NET_HEADER_ID, sizeof(SHOOT_NET_HEADER_ID) - 1))
        {
            printf("received data from %.*s %.*s\n", 128, temp_hostname, 32, temp_port);
            if (!ISVALIDSOCKET(destination_socket))
            {
                /* this is wrong just take the peer_address when you poll for the first time lmao */
                destination_socket = shoot_net_open_peer_socket(temp_hostname, temp_port, &peer_address);
                printf("connection established with peer %s %s\n", temp_hostname, temp_port);
            }
            
            if (cursor_exists)
            {
                menu_player_two = temp_cursor;
            }
        }
        if (ISVALIDSOCKET(destination_socket))
        {
            struct ShootNetHeader send_header = shoot_net_make_send_header();
            shoot_net_send(destination_socket, peer_address, &send_header, sizeof(send_header));
            shoot_net_send(destination_socket, peer_address, &menu_player_one, sizeof(menu_player_one));
        }
    }
    if (network_state == SHOOT_NET_STATUS_PLAYER_TWO)
    {
        struct ShootNetHeader temp_header = {};
        struct ShootPlayerCursor temp_cursor = {};
        char temp_hostname[128], temp_port[32];
        shoot_net_poll(network_socket, max_socket, &temp_header, sizeof(temp_header), temp_hostname, 128, temp_port, 32);
        bool32 cursor_exists = shoot_net_poll(network_socket, max_socket, &temp_cursor, sizeof(temp_cursor), 0, 0, 0, 0);

        if (shoot_is_string_equal(temp_header.ID, SHOOT_NET_HEADER_ID, sizeof(SHOOT_NET_HEADER_ID) - 1))
        {
            printf("received data from %.*s %.*s\n", 128, temp_hostname, 32, temp_port);
            if (!ISVALIDSOCKET(destination_socket))
            {
                destination_socket = shoot_net_open_peer_socket(temp_hostname, temp_port, &peer_address);
                printf("connection established with peer %s %s\n", temp_hostname, temp_port);
            }

            if (cursor_exists)
            {
                menu_player_one = temp_cursor;
            }
        }
        if (ISVALIDSOCKET(destination_socket))
        {
            struct ShootNetHeader send_header = shoot_net_make_send_header();
            shoot_net_send(destination_socket, peer_address, &send_header, sizeof(send_header));
            shoot_net_send(destination_socket, peer_address, &menu_player_two, sizeof(menu_player_two));
        }
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

    if (network_state == SHOOT_NET_STATUS_OFFLINE)
    {
        shoot_image_draw_rect(&window.data->screen, menu_player_one.mouseX - 5, menu_player_one.mouseY - 5,
                            menu_player_one.mouseX + 5, menu_player_one.mouseY + 5, 1.0, 1.0, 1.0, 1.0);
    }
    if (network_state == SHOOT_NET_STATUS_PLAYER_ONE)
    {
        shoot_image_draw_rect(&window.data->screen, menu_player_one.mouseX - 5, menu_player_one.mouseY - 5,
                            menu_player_one.mouseX + 5, menu_player_one.mouseY + 5, 1.0, 1.0, 1.0, 1.0);
        shoot_image_draw_rect(&window.data->screen, menu_player_two.mouseX - 5, menu_player_two.mouseY - 5,
                            menu_player_two.mouseX + 5, menu_player_two.mouseY + 5, 0.0, 0.0, 0.0, 1.0);
    }
    if (network_state == SHOOT_NET_STATUS_PLAYER_TWO)
    {
        shoot_image_draw_rect(&window.data->screen, menu_player_two.mouseX - 5, menu_player_two.mouseY - 5,
                            menu_player_two.mouseX + 5, menu_player_two.mouseY + 5, 1.0, 1.0, 1.0, 1.0);
        shoot_image_draw_rect(&window.data->screen, menu_player_one.mouseX - 5, menu_player_one.mouseY - 5,
                            menu_player_one.mouseX + 5, menu_player_one.mouseY + 5, 0.0, 0.0, 0.0, 1.0);
    }
}

static bool32 game_loop()
{
    if (shoot_input_just_released(&window.data->input[0], KEY_ESC))
    {
        shoot_game_change_state(GAME_STATE_MENU);
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

int main(int argc, char *argv[])
{
    strcpy(broadcast_address, SHOOT_NET_BROADCAST_ADDRESS);
    if (argc >= 2)
    {
        strcpy(broadcast_address, argv[1]);
    }
    shoot_check_compatibility();

    struct ShootWindowData hints = {
        .true_width = 512,
        .true_height = 512,
        .pixel_width = SCREEN_WIDTH,
        .pixel_height = SCREEN_HEIGHT,
        .background_red = 255,
        .background_green = 0,
        .background_blue = 255,
        .resize_function = shoot_opengl_resize_viewport,
        .cursor_type = GLFW_CURSOR_HIDDEN,
    };
    window = shoot_window_setup(&hints);
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