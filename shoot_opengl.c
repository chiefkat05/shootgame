#ifndef CHIEFKAT_OPENGL_H
#define CHIEFKAT_OPENGL_H

#include "shoot.h"
#include <GL/glew.h>

/** NOTE: Be sure to call shoot_window_setup() first in order to prepare the opengl context. - chief **/
static void shoot_opengl_window_setup(struct ShootWindow *window)
{
    glewInit();

    shoot_opengl_resize_viewport(window->data->true_width, window->data->true_height, window->data->pixel_width, window->data->pixel_height);

#ifdef __EMSCRIPTEN__
    window->opengl_shader = shoot_opengl_create_shader("shaders/shader_web.vs", "shaders/shader_web.fs");
#else
    window->opengl_shader = shoot_opengl_create_shader("shaders/shader.vs", "shaders/shader.fs");
#endif

    window->opengl_vao = shoot_opengl_create_vao();
    uint32 vbo = shoot_opengl_create_vbo(shoot_opengl_screen_quad_vertices, sizeof(shoot_opengl_screen_quad_vertices));

#ifdef __EMSCRIPTEN__
/* Endless love to the user at https://stackoverflow.com/questions/42231698/how-to-convert-glsl-version-330-core-to-glsl-es-version-100,
 * and screw webgl from the bottom of my heart. - chief*/
    int32 vertex_position_index = glGetAttribLocation(window->opengl_shader, "aPos");
    int32 vertex_texture_index = glGetAttribLocation(window->opengl_shader, "aTex");
    shoot_opengl_add_vbo_attributes(window->opengl_vao, vbo, vertex_position_index, 3, 5, 0);
    shoot_opengl_add_vbo_attributes(window->opengl_vao, vbo, vertex_texture_index, 2, 5, 3);
#else
    shoot_opengl_add_vbo_attributes(window->opengl_vao, vbo, 0, 3, 5, 0);
    shoot_opengl_add_vbo_attributes(window->opengl_vao, vbo, 1, 2, 5, 3);
#endif

    glGenTextures(1, &window->opengl_texture);
    glBindTexture(GL_TEXTURE_2D, window->opengl_texture);
}

static uint32 shoot_opengl_create_vao()
{
    uint32 vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    return vao;
}
static uint32 shoot_opengl_create_vbo(real *vertices, uint32 size)
{
    uint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    return vbo;
}
static void shoot_opengl_add_vbo_attributes(uint32 VAO, uint32 VBO, uint32 index, uint32 size, uint32 stride, uint32 offset)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    uint32 gl_real_type = GL_FLOAT;
    if (sizeof(real) > sizeof(float))
    {
        gl_real_type = GL_DOUBLE;
    }
    glVertexAttribPointer(index, size, gl_real_type, GL_FALSE, stride * sizeof(real), (void *)(offset * sizeof(real)));
    glEnableVertexAttribArray(index);
}


static uint32 shoot_opengl_create_shader(const char *vertex_shader_path, const char *fragment_shader_path)
{
    int success;
    char infoLog[512];

    struct ShootArena local_memory;
    shoot_arena_alloc(&local_memory, MEGABYTES(16));

    uint32 vShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vsrc = shoot_read_file_string(&local_memory, vertex_shader_path);
    glShaderSource(vShader, 1, &vsrc, NULL);
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    
    if (!success)
    {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    uint32 fShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsrc = shoot_read_file_string(&local_memory, fragment_shader_path);
    glShaderSource(fShader, 1, &fsrc, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    uint32 program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    shoot_arena_free(&local_memory);

    return program;
}

static void shoot_opengl_resize_viewport(int width, int height, int target_width, int target_height)
{
    real screen_offset_x, screen_offset_y, screen_width, screen_height;
    shoot_math_letterbox(width, height, target_width, target_height, &screen_offset_x, &screen_offset_y, &screen_width, &screen_height);
    glViewport(screen_offset_x, screen_offset_y, screen_width, screen_height);
}
static void shoot_opengl_copy_data_to_texture(uint32 gl_texture, void *data, uint32 width, uint32 height)
{
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

#endif