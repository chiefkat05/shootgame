#include "shoot.h"

#include <GLFW/glfw3.h>

static void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    struct ShootWindowData *data = (struct ShootWindowData *)glfwGetWindowUserPointer(window);
    data->resize_function(width, height, SCREEN_WIDTH, SCREEN_HEIGHT);
    data->true_width = width;
    data->true_height = height;
}

/** TODO: MAKE THIS CONFIGUREABLE */
#define JOYSTICK_AXES_DEADZONE 0.1
static void joystickPoll(struct ShootWindowData *winData, int jid)
{
    if (glfwJoystickPresent(jid) == GLFW_FALSE)
    {
        return;
    }
    int i, axesCount, hatCount, buttonCount;
    const float *axes = glfwGetJoystickAxes(jid, &axesCount);
    for (i = 0; i < axesCount; ++i)
    {
        real axesValue = axes[i];
        if(ABSOLUTE(axes[i]) < JOYSTICK_AXES_DEADZONE)
        {
            axesValue = 0.0;
        }
        enum ShootInputCode inputCode = (enum ShootInputCode)(PAD_AXIS0 + i);
        int16 inputValue = (int16)(axesValue * (real)INT16_MAX);

        shoot_input_set(&winData->input[jid], inputCode, inputValue);
    }

    const uint8 *hats = glfwGetJoystickHats(jid, &hatCount);
    for (i = 0; i < hatCount; ++i)
    {
        shoot_input_set(&winData->input[jid], (enum ShootInputCode)(PAD_HAT0 + i), hats[i]);
    }

    const uint8 *buttons = glfwGetJoystickButtons(jid, &buttonCount);
    for (i = 0; i < buttonCount; ++i)
    {
        shoot_input_set(&winData->input[jid], (enum ShootInputCode)(PAD_BUTTONA + i), buttons[i]);
    }
}
static void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT)
    { return; }

    struct ShootWindowData *data = (struct ShootWindowData *)glfwGetWindowUserPointer(window);
    switch(scancode)
    {
        case 1: { shoot_input_set(&data->input[0], KEY_ESC, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 2: { shoot_input_set(&data->input[0], KEY_1, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 3: { shoot_input_set(&data->input[0], KEY_2, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 4: { shoot_input_set(&data->input[0], KEY_3, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 5: { shoot_input_set(&data->input[0], KEY_4, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 6: { shoot_input_set(&data->input[0], KEY_5, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 7: { shoot_input_set(&data->input[0], KEY_6, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 8: { shoot_input_set(&data->input[0], KEY_7, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 9: { shoot_input_set(&data->input[0], KEY_8, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 10: { shoot_input_set(&data->input[0], KEY_9, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 11: { shoot_input_set(&data->input[0], KEY_0, action == GLFW_PRESS ? PRESSED : RELEASED); } break;

        case 15: { shoot_input_set(&data->input[0], KEY_TAB, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 16: { shoot_input_set(&data->input[0], KEY_Q, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 17: { shoot_input_set(&data->input[0], KEY_W, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 18: { shoot_input_set(&data->input[0], KEY_E, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 19: { shoot_input_set(&data->input[0], KEY_R, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 20: { shoot_input_set(&data->input[0], KEY_T, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 21: { shoot_input_set(&data->input[0], KEY_Y, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 22: { shoot_input_set(&data->input[0], KEY_U, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 23: { shoot_input_set(&data->input[0], KEY_I, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 24: { shoot_input_set(&data->input[0], KEY_O, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 25: { shoot_input_set(&data->input[0], KEY_P, action == GLFW_PRESS ? PRESSED : RELEASED); } break;

        case 28: { shoot_input_set(&data->input[0], KEY_ENTER, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 29: { shoot_input_set(&data->input[0], KEY_CTRL, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 30: { shoot_input_set(&data->input[0], KEY_A, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 31: { shoot_input_set(&data->input[0], KEY_S, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 32: { shoot_input_set(&data->input[0], KEY_D, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 33: { shoot_input_set(&data->input[0], KEY_F, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 34: { shoot_input_set(&data->input[0], KEY_G, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 35: { shoot_input_set(&data->input[0], KEY_H, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 36: { shoot_input_set(&data->input[0], KEY_J, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 37: { shoot_input_set(&data->input[0], KEY_K, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 38: { shoot_input_set(&data->input[0], KEY_L, action == GLFW_PRESS ? PRESSED : RELEASED); } break;

        case 42: { shoot_input_set(&data->input[0], KEY_SHIFT, action == GLFW_PRESS ? PRESSED : RELEASED); } break;

        case 44: { shoot_input_set(&data->input[0], KEY_Z, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 45: { shoot_input_set(&data->input[0], KEY_X, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 46: { shoot_input_set(&data->input[0], KEY_C, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 47: { shoot_input_set(&data->input[0], KEY_V, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 48: { shoot_input_set(&data->input[0], KEY_B, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 49: { shoot_input_set(&data->input[0], KEY_N, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 50: { shoot_input_set(&data->input[0], KEY_M, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        case 51: { shoot_input_set(&data->input[0], KEY_Z, action == GLFW_PRESS ? PRESSED : RELEASED); } break;

        case 57: { shoot_input_set(&data->input[0], KEY_SPACE, action == GLFW_PRESS ? PRESSED : RELEASED); } break;
        default: break;
    }
}
static void mousePosCallback(GLFWwindow *window, double xpos, double ypos)
{
    struct ShootWindowData *data = (struct ShootWindowData *)glfwGetWindowUserPointer(window);

    real screen_x, screen_y, screen_width, screen_height;
    shoot_math_letterbox(data->true_width, data->true_height, SCREEN_WIDTH, SCREEN_HEIGHT, &screen_x, &screen_y, &screen_width, &screen_height);

    xpos -= screen_x;
    ypos -= screen_y;
    xpos /= screen_width;
    ypos /= screen_height;
    shoot_input_set(&data->input[0], MOUSE_X, xpos * INT16_MAX);
    shoot_input_set(&data->input[0], MOUSE_Y, (1.0 - ypos) * INT16_MAX);
}
static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    struct ShootWindowData *data = (struct ShootWindowData *)glfwGetWindowUserPointer(window);

    shoot_input_set(&data->input[0], (enum ShootInputCode)(MOUSE_BUTTON_LEFT + button), action == GLFW_PRESS ? PRESSED : RELEASED);
}


/** NOTE: Hints may be NULL, in which case a set of default window hints will be allocated in the given arena.
 *  Defaults are as follows: true_width = 1280, true_height = 720, pixel_width = SCREEN_WIDTH (defined in shoot.h), pixel_height = SCREEN_HEIGHT (defined in shoot.h),
 *  background color = 255 red 255 green 255 blue,
 *  memory size 64 megabytes
 */
static struct ShootWindow shoot_window_setup(struct ShootWindowData *hints)
{
    struct ShootWindow window = (struct ShootWindow){};

    /** NOTE: Kind of iffy, but since there's only one of these and it's controlled I'm leaving this for now.
     *  Feel free to make it better. - chief */
    window.memory = malloc(sizeof(struct ShootArena));

    struct ShootWindowData *default_hints;
    if (!hints)
    {
        shoot_arena_alloc(window.memory, MEGABYTES(64));
        default_hints = shoot_arena_grab_memory(window.memory, sizeof(struct ShootWindowData));
        *default_hints = (struct ShootWindowData){
            .true_width = 1280,
            .true_height = 720,
            .pixel_width = SCREEN_WIDTH,
            .pixel_height = SCREEN_HEIGHT,
            .background_red = 255,
            .background_green = 0,
            .background_blue = 255,
            .cursor_type = GLFW_CURSOR_HIDDEN,
        };

        hints = default_hints;
    }
    else
    {
        shoot_arena_alloc(window.memory, MEGABYTES(16));
    }
    glfwInit();

    window.data = hints;
    if (window.data->resize_function == NULL)
    {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }
    window.glfw_window_handle = glfwCreateWindow(hints->true_width, hints->true_height, "window", NULL, NULL);
    glfwSetInputMode(window.glfw_window_handle, GLFW_CURSOR, window.data->cursor_type);

    glfwMakeContextCurrent(window.glfw_window_handle);

    window.data->screen = image_create(window.memory, hints->pixel_width, hints->pixel_height);

    glfwSetWindowUserPointer((GLFWwindow *)window.glfw_window_handle, window.data);
    glfwSetFramebufferSizeCallback((GLFWwindow *)window.glfw_window_handle, framebufferSizeCallback);
    glfwSetKeyCallback((GLFWwindow *)window.glfw_window_handle, keyboardCallback);
    glfwSetMouseButtonCallback((GLFWwindow *)window.glfw_window_handle, mouseButtonCallback);
    glfwSetCursorPosCallback((GLFWwindow *)window.glfw_window_handle, mousePosCallback);

    return window;
}
static void shoot_window_close(struct ShootWindow window)
{
    /* could use an extra check here like a boolean to make sure this function doesn't get called more than once */
    glfwSetWindowShouldClose((GLFWwindow *)window.glfw_window_handle, GLFW_TRUE);

    shoot_arena_free(window.memory);
    if (window.memory) { free(window.memory); }
}

static bool32 shoot_window_is_open(struct ShootWindow window)
{
    return (glfwWindowShouldClose((GLFWwindow *)window.glfw_window_handle) == 0);
}

static void shoot_window_clear(struct ShootWindow window)
{
    shoot_image_fill(&window.data->screen, 255 << 24 | window.data->background_blue << 16 | window.data->background_green << 8 | window.data->background_red);
}

static void shoot_window_start_physics_tick(struct ShootWindow window)
{
    real previous_time = window.data->current_time;
    window.data->current_time = glfwGetTime();
    real deltaTime = window.data->current_time - previous_time;
    window.data->accumulated_time += deltaTime;
}
static void shoot_window_poll_events(struct ShootWindow window)
{
    int i;
    for (i = 0; i < MAX_PLAYERS; ++i)
    {
        shoot_input_update(&window.data->input[i]);
    }

    glfwPollEvents();
    joystickPoll(window.data, 0);
}

static void shoot_window_draw(struct ShootWindow window)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shoot_opengl_copy_data_to_texture(window.opengl_texture, window.data->screen.data, window.data->screen.width, window.data->screen.height);

    glUseProgram(window.opengl_shader);
    glBindTexture(GL_TEXTURE_2D, window.opengl_texture);
    glBindVertexArray(window.opengl_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers((GLFWwindow *)window.glfw_window_handle);
}