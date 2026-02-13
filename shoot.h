#ifndef SHOOT_H
#define SHOOT_H

#include "shoot_definitions.h"

/* ------------------ Memory ------------------ */

struct ShootArena
{
    void *memory;
    uint64 cursor, memsize;
};

static void *shoot_arena_grab_memory(struct ShootArena *arena, uint64 bytes);
static void shoot_arena_alloc(struct ShootArena *arena, uint64 bytes);
static void shoot_arena_free(struct ShootArena *arena);

static void shoot_copy(void *dest, void *src, uint64 bytes);
static void shoot_copy_backwards(void *dest, void *src, uint64 bytes);
static void shoot_copy32(uint32 *dest, uint32 *src, uint64 count);
static void shoot_copy32_backwards(uint32 *dest, uint32 *src, uint64 count);

/* ------------------ Input ------------------ */

#define PRESSED 1
#define RELEASED 0
#define UP 1
#define UPRIGHT 3
#define RIGHT 2
#define DOWNRIGHT 6
#define DOWN 4
#define DOWNLEFT 12
#define LEFT 8
#define UPLEFT 9
enum ShootInputCode
{
    MOUSE_ID, /* ?? test with multiple mice */
    MOUSE_X, MOUSE_Y, MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT,

    KEYBOARD_ID, /* ?? test with multiple keyboards */
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
    KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_SHIFT, KEY_CTRL, KEY_TAB, KEY_ESC, KEY_SPACE, KEY_ENTER,

    PAD_ID, /* test with multiple pads (should be able to take two or more inputs at the same time) */
    PAD_AXIS0, PAD_AXIS1, PAD_AXIS2, PAD_AXIS3, PAD_AXIS4, PAD_AXIS5,
    PAD_AXIS6, PAD_AXIS7, PAD_AXIS8, PAD_AXIS9, PAD_AXIS10, PAD_AXIS11,
    PAD_HAT0, PAD_HAT1, PAD_HAT2, PAD_HAT3, PAD_HAT4, PAD_HAT5, PAD_HAT6, PAD_HAT7, PAD_HAT8,
    PAD_BUTTONA, PAD_BUTTONB, PAD_BUTTONX, PAD_BUTTONY, PAD_BUTTONL, PAD_BUTTONR,
    PAD_BUTTONSELECT, PAD_BUTTONSTART, PAD_BUTTONHOME, PAD_BUTTONSTICKL, PAD_BUTTONSTICKR,
    PAD_BUTTON_DPADUP, PAD_BUTTON_DPADRIGHT, PAD_BUTTON_DPADDOWN, PAD_BUTTON_DPADLEFT,

    JOYSTICK_ID, /* ditto */
    JOYSTICK_UP,

    total_input_count
};

struct ShootInputState
{
    int16 currentState[total_input_count];
    int16 lastState[total_input_count];
};

static void shoot_input_set(struct ShootInputState *state, enum ShootInputCode input, int16 value);
static void shoot_input_update(struct ShootInputState *state);
static int16 shoot_input_get(struct ShootInputState *state, enum ShootInputCode input);
static bool32 shoot_input_just_released(struct ShootInputState *state, enum ShootInputCode input);
static bool32 shoot_input_just_pressed(struct ShootInputState *state, enum ShootInputCode input);

/* ------------------ Image ------------------ */

#define BYTES_PER_PIXEL 4
struct ShootImage
{
    void *data;
    uint32 width;
    uint32 height;
    uint32 row;
    uint32 pixel_count;
};

static void shoot_image_draw_pixel(struct ShootImage *image, int xpos, int ypos, real red, real green, real blue, real alpha);
static void shoot_image_draw_rect(struct ShootImage *image, int left, int bottom, int right, int top, real red, real green, real blue, real alpha);
static void shoot_image_fill(struct ShootImage *a, uint32 color);

static struct ShootImage image_create(struct ShootArena *arena, uint32 width, uint32 height);

#define SPRITE_EDGE 16
#define SPRITE_COPY_TO_IMAGE(source, destination, dx, dy, sx, sy, sw, sh) imageCopyToImage(source, destination, dx, dy, sx * SPRITE_EDGE, sy * SPRITE_EDGE, sw * SPRITE_EDGE, sh * SPRITE_EDGE)
static void shoot_image_map_to_image(struct ShootImage *source, struct ShootImage *destination, int destination_x_pos, int destination_y_pos, int source_x, int source_y, int source_width, int source_height);
static void shoot_image_duplicate(struct ShootImage *source, struct ShootImage *destination);

static uint32 *shoot_load_bmp(struct ShootArena *arena, const char *path, uint32 *out_width, uint32 *out_height, bool32 swap_blue_red_check);
static struct ShootImage shoot_image_create_bmp(struct ShootArena *arena, const char *path, bool32 swapBlueRed);

void shoot_image_scale_to_image(struct ShootImage *source, struct ShootImage *destination);

static void shoot_image_flip_vertical(struct ShootImage *source, struct ShootImage *destination);
static void shoot_image_flip_horizontal(struct ShootImage *source, struct ShootImage *destination);

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

/* ------------------ Window ------------------ */

#define MAX_PLAYERS 8
#define RESIZE_FUNCTION(name) void(name)(int window_width, int window_height, int screen_width, int screen_height)
#define RESIZE_FUNCTION_POINTER(name) RESIZE_FUNCTION(*name)

#define PHYSICS_TICK_SPEED 1.0/144.0
struct ShootWindowData
{
    struct ShootImage screen;

    int32 true_width, true_height;
    int32 pixel_width, pixel_height;

    uint8 background_red, background_green, background_blue;

    struct ShootInputState input[MAX_PLAYERS];
    RESIZE_FUNCTION_POINTER(resize_function);

    real accumulated_time, current_time;

    uint32 cursor_type;
};

struct ShootWindow
{
    uint32 opengl_texture, opengl_shader, opengl_vao;
    void *glfw_window_handle;
    struct ShootWindowData *data;
    struct ShootArena *memory;
};

static struct ShootWindow shoot_window_setup(struct ShootWindowData *hints);
static void shoot_window_close(struct ShootWindow window);
static bool32 shoot_window_is_open(struct ShootWindow window);
static void shoot_window_clear(struct ShootWindow window);
static void shoot_window_start_physics_tick(struct ShootWindow window);
static void shoot_window_poll_events(struct ShootWindow window);
static void shoot_window_draw(struct ShootWindow window);

/* ------------------ Math ------------------ */

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? (-(a)) : (a))

static real shoot_math_lerp(real source, real destination, real alpha);
static int32 shoot_math_round_to_int(real number);

static void shoot_math_letterbox(real width, real height, real box_width, real box_height, real *out_x, real *out_y, real *out_width, real *out_height);

/* ------------------ OpenGL ------------------ */

static uint32 shoot_opengl_create_vao();
static uint32 shoot_opengl_create_vbo(real *vertices, uint32 size);
static void shoot_opengl_add_vbo_attributes(uint32 VAO, uint32 VBO, uint32 index, uint32 size, uint32 stride, uint32 offset);
static uint32 shoot_opengl_create_shader(const char *vertex_shader_path, const char *fragment_shader_path);

static void shoot_opengl_window_setup(struct ShootWindow *window);
static void shoot_opengl_resize_viewport(int width, int height, int target_width, int target_height);

/* This needs to go somewhere */
static real shoot_opengl_screen_quad_vertices[] = {
    -1.0, -1.0, 0.0, 0.0, 0.0,
     1.0, -1.0, 0.0, 1.0, 0.0,
     1.0, 1.0, 0.0, 1.0, 1.0,

    -1.0, -1.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 1.0, 1.0,
     -1.0, 1.0, 0.0, 0.0, 1.0
};


/* ------------------ Sound ------------------ */


/* ------------------ Generic ------------------ */

static const char *shoot_read_file_string(struct ShootArena *memory, const char *path);
static void *shoot_read_file_raw(struct ShootArena *memory, const char *path);

/* ------------------ Shapes ------------------ */

struct ShootRect
{
    int32 left, bottom, right, top;
};

/* ------------------ Networking ---------------- */

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef AI_ALL
#define AI_ALL 0x0100
#endif
#else

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#endif

#ifdef _WIN32
#define ISVALIDSOCKET(sock) ((sock) != INVALID_SOCKET)
#define CLOSESOCKET(sock) closesocket(sock)
#else
#define ISVALIDSOCKET(sock) ((sock) >= 0)
#define CLOSESOCKET(sock) close(sock)
#define SOCKET int
#endif

#ifdef _WIN32
#define NET_STARTUP WSADATA d; verify(!WSAStartup(MAKEWORD(2, 2), &d), "WSAStartup failure");
#define NET_SHUTDOWN WSACleanup();
#else
#define NET_STARTUP
#define NET_SHUTDOWN
#endif

static void shoot_net_get_socket_ip(SOCKET host_socket, char *out_hostname, uint32 out_hostname_length,
            char *out_port, uint32 out_port_length);
static void shoot_net_get_address_ip(struct addrinfo *address, char *out_hostname, uint32 out_hostname_length,
            char *out_port, uint32 out_port_length);
static void shoot_net_get_socket_address_ip(struct sockaddr *socket_address, uint32 socket_address_length,
            char *out_hostname, uint32 out_hostname_length, char *out_port, uint32 out_port_length);
#endif