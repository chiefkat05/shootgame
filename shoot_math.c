#include "shoot.h"

static real shoot_math_sqrt()
{
    return 0.0;
}

static real shoot_math_pow(real number, real exponent)
{
    return 0.0;
}

static int32 shoot_math_round_to_int(real number)
{
    return (int32)(number + 0.5);
}

static int32 shoot_math_random()
{
    return 0;
}

static real shoot_math_lerp(real source, real destination, real alpha)
{
    return (source * (1.0f - alpha)) + (destination * alpha);
}

static void shoot_math_letterbox(real width, real height, real box_width, real box_height, real *out_x, real *out_y, real *out_width, real *out_height)
{
    real WidthDiff = width / box_width;
    real HeightDiff = height / box_height;
    real widthAspect = WidthDiff / MIN(WidthDiff, HeightDiff);
    real heightAspect = HeightDiff / MIN(WidthDiff, HeightDiff);

    *out_width = (real)width / widthAspect;
    *out_height = (real)height / heightAspect;
    *out_x = width / 2 - (*out_width / 2.0f);
    *out_y = height / 2 - (*out_height / 2.0f);
}