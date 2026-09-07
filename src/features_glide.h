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

/* Panel presets approximate 10, 22, 50, 100, 200, 500, 1000, 2000 ms.
 * Keep the existing wire/storage law: zero remains the exact 22-ms default.
 * Resolve that alias before panel arithmetic, and never decrement into it. */
static const uint16_t glide_panel_presets[8]={1,1272,2323,3484,5062,8130,11555,16383};

static inline uint16_t glide_panel_adjust(uint16_t value, uint16_t fine, uint16_t up)
{
    uint16_t i;
    if(!value) value=glide_panel_presets[1];
    if(fine) {
        if(up) return value>16383-128 ? 16383 : value+128;
        return value<=128 ? 1 : value-128;
    }
    if(up) {
        for(i=0;i<8;++i) if(glide_panel_presets[i]>value) return glide_panel_presets[i];
        return 16383;
    }
    for(i=8;i>0;--i) if(glide_panel_presets[i-1]<value) return glide_panel_presets[i-1];
    return 1;
}

/* Nearest preset on the curved time control; at least one LED even at minimum.
 * The complete bar spans SEQ1 then SEQ2. EG separately reports enable state. */
static inline uint16_t glide_panel_level(uint16_t value)
{
    uint16_t i;
    if(!value) value=glide_panel_presets[1];
    for(i=1;i<8;++i)
        if(value<(glide_panel_presets[i-1]+glide_panel_presets[i])/2) return i;
    return 8;
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
