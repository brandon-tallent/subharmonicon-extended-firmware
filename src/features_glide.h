#ifndef SUBH_FEATURES_GLIDE_H
#define SUBH_FEATURES_GLIDE_H
#include <stdint.h>

/* Pitch publication alternates with control acquisition: 200 us, not 100 us.
 * A paused control service pauses the trajectory; there is no catch-up jump.
 * Parameter ingress owns range validation. The default models the TB-303's
 * 100-kohm DAC impedance charging C35 (0.22 uF): nominal tau = 22 ms.
*/
#define GLIDE_TICK_US 200.0f
#define GLIDE_DEFAULT_US 22000.0f
struct GlideVoice { float target; float remaining; };

static float glide_time_us(uint16_t value)
{
    float unit;
    if(!value) return GLIDE_DEFAULT_US;
    unit=(float)value*(1.0f/16383.0f);
    return 10000.0f+1990000.0f*unit*unit;
}

/* Compute only on a time change. dt/tau <= .02; fourth-order expansion
 * avoids cancellation in 1-exp(-x) and needs no general-purpose exp(). */
static float glide_coefficient(float tau_us)
{
    float x=GLIDE_TICK_US/tau_us;
    return x*(1.0f-x*(0.5f-x*(1.0f/6.0f-x*(1.0f/24.0f))));
}

static void glide_target(volatile struct GlideVoice *v, float target, uint16_t snap)
{
    if(snap) v->remaining=0.0f;
    else v->remaining+=target-v->target;
    v->target=target;
}

static float glide_advance(volatile struct GlideVoice *v, float alpha)
{
    float current;
    /* Decay the remaining displacement. Adding tiny increments directly to
     * a large absolute MIDI pitch can stall a slow glide before its target. */
    v->remaining-=v->remaining*alpha;
    current=v->target-v->remaining;
    if(current==v->target) v->remaining=0.0f;
    return current;
}
#endif
