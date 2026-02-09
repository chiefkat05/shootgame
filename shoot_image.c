#include "shoot.h"

static void shoot_image_draw_pixel(struct ShootImage *image, int xpos, int ypos, real red, real green, real blue, real alpha)
{
    if (xpos < 0 || ypos < 0 || xpos >= image->width || ypos >= image->height || alpha < __FLT_EPSILON__)
    { return; }

    uint32 *pixel_location = (uint32 *)(image->data + (ypos * image->row) + xpos * BYTES_PER_PIXEL);

    uint32 destinationpixel = *pixel_location;

    real invalpha = 1.0f - alpha;
    real destination_redf = (real)(destinationpixel >> 16 & 255) / 255.0f;
    real destination_greenf = (real)(destinationpixel >> 8 & 255) / 255.0f;
    real destination_bluef = (real)(destinationpixel & 255) / 255.0f;
    
    real redmix = shoot_math_lerp(red, destination_redf, invalpha);
    real greenmix = shoot_math_lerp(green, destination_greenf, invalpha);
    real bluemix = shoot_math_lerp(blue, destination_bluef, invalpha);

    uint8 red8 = (uint8)shoot_math_round_to_int(redmix * 255.0f);
    uint8 green8 = (uint8)shoot_math_round_to_int(greenmix * 255.0f);
    uint8 blue8 = (uint8)shoot_math_round_to_int(bluemix * 255.0f);

    *pixel_location = 255 << 24 | red8 << 16 | green8 << 8 | blue8;
}
static void shoot_image_draw_rect(struct ShootImage *image, int left, int bottom, int right, int top, real red, real green, real blue, real alpha)
{
    left = MAX(left, 0);
    bottom = MAX(bottom, 0);
    right = MIN(right, (int)image->width);
    top = MIN(top, (int)image->height);

    int x, y;
    for (y = bottom; y < top; ++y)
    {
        for (x = left; x < right; ++x)
        {
            shoot_image_draw_pixel(image, x, y, red, green, blue, alpha);
        }
    }
}
static void shoot_image_fill(struct ShootImage *a, uint32 color)
{
    uint32 *dataCursor = (uint32 *)a->data;
    int p;
    for (p = 0; p < a->pixel_count; ++p)
    {
        *dataCursor++ = color;
    }
}

static struct ShootImage image_create(struct ShootArena *arena, uint32 width, uint32 height)
{
    struct ShootImage img;

    img.width = width;
    img.height = height;
    img.row = img.width * BYTES_PER_PIXEL;
    img.pixel_count = img.width * img.height;
    img.data = shoot_arena_grab_memory(arena, img.width * img.height * BYTES_PER_PIXEL);

    return img;
}

/* A heavy-duty version of the quake draw-pic function, built to (hopefully) tank any random values I chuck at it. */
/* Could also likely use optimization, as it's very brute-force at the moment. */
#define SPRITE_EDGE 16
#define SPRITE_COPY_TO_IMAGE(source, destination, dx, dy, sx, sy, sw, sh) imageCopyToImage(source, destination, dx, dy, sx * SPRITE_EDGE, sy * SPRITE_EDGE, sw * SPRITE_EDGE, sh * SPRITE_EDGE)
static void shoot_image_map_to_image(struct ShootImage *source, struct ShootImage *destination, int destination_x_pos, int destination_y_pos, int source_x, int source_y, int source_width, int source_height)
{
    if (destination_x_pos == DONT_CARE) { destination_x_pos = 0; }
    if (destination_y_pos == DONT_CARE) { destination_y_pos = 0; }
    if (source_x == DONT_CARE) { source_x = 0; }
    if (source_y == DONT_CARE) { source_y = 0; }
    if (source_width == DONT_CARE) { source_width = source->width; }
    if (source_height == DONT_CARE) { source_height = source->height; }

    if (destination_x_pos > (int)destination->width || destination_y_pos > (int)destination->height)
    { return; }

    source_width = MIN(source->width, source_width);
    source_height = MIN(source->height, source_height);

    source_x = MAX(0, source_x);
    source_y = MAX(0, source_y);
    source_x = MIN(source_x, source->width - source_width);
    source_y = MIN(source_y, source->height - source_height);

    source_x %= source->width;

    source_width = MIN(source_width, source->width);
    source_height = MIN(source_height, source->height);

    uint32 destination_x = MAX(0, destination_x_pos);
    uint32 destination_y = MAX(0, destination_y_pos);

    int draw_offset_x = 0 - destination_x_pos;
    int draw_offset_y = 0 - destination_y_pos;

    uint32 *sourceData = source->data;
    uint32 *destinationData = destination->data;
    destinationData += (destination_y * destination->width) + destination_x;

    uint32 draw_width = source_width - MAX(0, draw_offset_x);
    uint32 draw_height = source_height - MAX(0, draw_offset_y);

    int off_right_side = (destination_x_pos + source->width) - destination->width;
    int off_top_side = (destination_y_pos + source->height) - destination->height;

    if (draw_offset_x >= source_width)
    { draw_width = 0; }
    if (draw_offset_x < 0)
    { draw_width = source->width - (MAX(0, off_right_side));}

    if (draw_offset_y >= source_height)
    { draw_height = 0; }
    if (draw_offset_y < 0)
    { draw_height = source->height - MAX(0, off_top_side); }

    draw_width = MIN(draw_width, source->width);
    draw_height = MIN(draw_height, source->height);

    draw_offset_x = MAX(0, draw_offset_x);
    draw_offset_y = MAX(0, draw_offset_y);

    sourceData += source->width * draw_offset_y;
    sourceData += draw_offset_x;

    sourceData += source_y * source->width;
    sourceData += source_x;

    draw_height = MIN(draw_height, source_height);
    draw_width = MIN(draw_width, source_width);
    
    int dy;
    for (dy = draw_offset_y; dy < draw_offset_y + draw_height; ++dy)
    {
        shoot_copy32(destinationData, sourceData, draw_width);
        destinationData += destination->width;
        sourceData += source->width;
    }
}
static void shoot_image_duplicate(struct ShootImage *source, struct ShootImage *destination)
{
    verify(source->width == destination->width && source->height == destination->height, "cannot duplicate image onto differently sized image");

    uint32 *sourceData = (uint32 *)source->data;
    uint32 *destinationData = (uint32 *)destination->data;

    int dy;
    for (dy = 0; dy < destination->height; ++dy)
    {
        shoot_copy32(destinationData, sourceData, destination->width);
        destinationData += destination->width;
        sourceData += source->width;
    }
}

static uint32 *shoot_load_bmp(struct ShootArena *arena, const char *path, uint32 *out_width, uint32 *out_height, bool32 swap_blue_red_check)
{
    struct ShootArena localMemory;
    shoot_arena_alloc(&localMemory, MEGABYTES(8));
    uint8 *data = (uint8 *)shoot_read_file_raw(&localMemory, path);

    int start = (*(int *)(data + 10));

    int32 width = *(int32 *)(data + 18);
    *out_width = width;
    int32 height = *(int32 *)(data + 22);
    *out_height = height;

    uint8 *bmpData = (uint8 *)data + start;

    uint32 *pixelPointer = (uint32 *)bmpData;
    uint32 *outPixelData = shoot_arena_grab_memory(arena, width * height * BYTES_PER_PIXEL);
    uint32 *outPixelPointer = outPixelData;
    int i;
    for (i = 0; i < width * height; ++i)
    {
        uint8 *red = (uint8 *)pixelPointer;
        uint8 *green = (uint8 *)pixelPointer + 1;
        uint8 *blue = (uint8 *)pixelPointer + 2;
        uint8 *alpha = (uint8 *)pixelPointer + 3;

        /** NOTE: this could be branchless very easily */

        uint8 swapped_blue_value = (swap_blue_red_check * *red + (1 - swap_blue_red_check) * *blue);
        uint8 swapped_red_value = ((1 - swap_blue_red_check) * *red + swap_blue_red_check * *blue);

        *outPixelPointer = *alpha << 24 | swapped_blue_value << 16 | *green << 8 | swapped_red_value;

        ++outPixelPointer;
        ++pixelPointer;
    }

    uint32 *out_data = (uint32 *)outPixelData;

    shoot_arena_free(&localMemory);

    return out_data;
}
static struct ShootImage shoot_image_create_bmp(struct ShootArena *arena, const char *path, bool32 swapBlueRed)
{
    struct ShootImage img;

    uint32 bmpWidth, bmpHeight;
    uint32 *bmpData = shoot_load_bmp(arena, path, &bmpWidth, &bmpHeight, swapBlueRed);

    img.width = bmpWidth;
    img.height = bmpHeight;
    img.row = img.width * BYTES_PER_PIXEL;
    img.pixel_count = img.width * img.height;
    img.data = bmpData;

    return img;
}

void shoot_image_scale_to_image(struct ShootImage *source, struct ShootImage *destination)
{
    real scaleX = (real)destination->width / (real)source->width;
    real scaleY = (real)destination->height / (real)source->height;
    int x, y;
    uint32 *pixel = (uint32 *)destination->data;
    uint32 *sourcepixel = (uint32 *)source->data;

    int currentX = 0;
    int currentY = 0;

    for (y = 0; y < destination->height; ++y)
    {
        for (x = 0; x < destination->width; ++x)
        {
            int pX = (int)((real)x / scaleX);
            int pY = (int)((real)y / scaleY);
            int diffX = pX - currentX;
            int diffY = pY - currentY;

            sourcepixel += diffX;
            currentX = pX;
            sourcepixel += diffY * source->width;
            currentY = pY;

            shoot_copy32(pixel, sourcepixel, 1);
            ++pixel;
        }
    }
}

static void shoot_image_flip_vertical(struct ShootImage *source, struct ShootImage *destination)
{
    verify(source->width == destination->width && source->height == destination->height, "cannot duplicate image onto differently sized image");

    uint32 *destinationData = destination->data + destination->pixel_count * BYTES_PER_PIXEL;
    destinationData -= source->width;
    uint32 *sourceData = source->data;

    int dy;
    for (dy = 0; dy < source->height; ++dy)
    {
        shoot_copy32(destinationData, sourceData, source->width);
        destinationData -= source->width;
        sourceData += source->width;
    }
}
static void shoot_image_flip_horizontal(struct ShootImage *source, struct ShootImage *destination)
{
    verify(source->width == destination->width && source->height == destination->height, "cannot duplicate image onto differently sized image");

    uint32 *destinationData = destination->data;
    uint32 *sourceData = source->data;

    int dy;
    for (dy = 0; dy < source->height; ++dy)
    {
        shoot_copy32_backwards(destinationData, sourceData, source->width);
        destinationData += source->width;
        sourceData += source->width;
    }
}