#ifndef SUBH_FEATURES_VELOCITY_H
#define SUBH_FEATURES_VELOCITY_H
#include <stdint.h>
/* Inputs are accepted MIDI values. Preserve the exact stock inverted CV for
 * zero depth and maximum velocity, including floating-point endpoint bits. */
static float velocity_vca(float inverted, uint16_t depth, uint16_t velocity)
{
    float gain;
    if(!depth || velocity==127) return inverted;
    gain=1.0f-((float)depth/127.0f)*(1.0f-(float)velocity/127.0f);
    return 1.0f-gain*(1.0f-inverted);
}
#endif
