#include "shoot.h"

static void shoot_input_set(struct ShootInputState *state, enum ShootInputCode input, int16 value)
{
    state->currentState[input] = value;
}
static void shoot_input_update(struct ShootInputState *state)
{
    int i;
    for (i = 0; i < total_input_count; ++i)
    {
        state->lastState[i] = state->currentState[i];
    }
}
static int16 shoot_input_get(struct ShootInputState *state, enum ShootInputCode input)
{
    return state->currentState[input];
}
static bool32 shoot_input_just_released(struct ShootInputState *state, enum ShootInputCode input)
{
    if (state->currentState[input] != state->lastState[input] && state->currentState[input] == RELEASED)
    {
        return TRUE;
    }
    return FALSE;
}
static bool32 shoot_input_just_pressed(struct ShootInputState *state, enum ShootInputCode input)
{
    if (state->currentState[input] != state->lastState[input] && state->currentState[input] == PRESSED)
    {
        return TRUE;
    }
    return FALSE;
}
