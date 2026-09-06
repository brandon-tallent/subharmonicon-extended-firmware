#ifndef SUBH_FEATURES_MATH_H
#define SUBH_FEATURES_MATH_H
#include <stdint.h>
#define Q_OFF 5U
static const uint16_t white_key[12] = {0,0,2,2,4,5,5,7,7,9,9,11};
#include "features_frequency_table.h"
static float fractional_semitone_ratio(float fraction)
{
    float x=fraction*0.0577622650467f;
    return 1.0f+x*(1.0f+x*(0.5f+x*(0.166666666667f+x*0.041666666667f)));
}
static float note_frequency(float note, uint16_t quantize, const float *ratios)
{
    uint16_t n=(uint16_t)note;
    float fraction=note-(float)n;
    if (quantize!=Q_OFF) fraction=0.0f;
    if (quantize==3 || quantize==4) n=(n/12)*12+white_key[n%12];
    if (quantize==1 || quantize==4) {
        /* The stock musical JI table has twelve ratios relative to C. */
        return note_hz[(n/12)*12]*(8.18f/8.17579891564f)*ratios[n%12];
    }
    return note_hz[n]*fractional_semitone_ratio(fraction);
}

struct PitchTarget { float base; float detune; uint16_t clamped; };
static void pitch_target(struct PitchTarget *target, float base, float detune, uint16_t quantize)
{
    target->clamped=0;
    /* Octave shifts preserve both pitch class and the complete pitch sum. */
    while(base<48.0f) { base+=12.0f; detune-=12.0f; }
    while(base>84.0f) { base-=12.0f; detune+=12.0f; }
    if(quantize!=Q_OFF) base=(float)(uint16_t)base;
    if(quantize==3 || quantize==4) {
        uint16_t n=(uint16_t)base;
        base=(float)((n/12)*12+white_key[n%12]);
    }
    /* Admit the complete MIDI/calibration domain. The former C3..C6 limit
     * was a feasibility-test guard, not the sequencer's musical range. */
    if(base+detune<0.0f) { detune=-base; target->clamped=1; }
    if(base+detune>127.0f) { detune=127.0f-base; target->clamped=1; }
    target->base=base; target->detune=detune;
}

static float natural_log(float x)
{
    union { float f; uint32_t u; } bits;
    int16_t exponent;
    float y,y2;
    bits.f=x;
    exponent=(int16_t)(bits.u>>23)-127;
    bits.u=(bits.u&0x007fffffUL)|0x3f800000UL;
    y=(bits.f-1.0f)/(bits.f+1.0f); y2=y*y;
    return (float)exponent*0.69314718056f+2.0f*y*(1.0f+y2*(0.33333333333f+
           y2*(0.2f+y2*(1.0f/7.0f+y2*(1.0f/9.0f+y2/11.0f)))));
}

static float frequency_from_log(float log_hz)
{
    float position=(log_hz-2.10117843869f)*17.3123404907f; /* ln(MIDI 0), 12/ln(2) */
    /* Roundoff at the lower endpoint and JI's slightly shifted upper pitch
     * must never turn into an out-of-bounds frequency-table index. */
    if(position<0.0f) position=0.0f;
    if(position>128.0f) position=128.0f;
    return note_frequency(position,Q_OFF,(const float *)0);
}

/* Stock parent CVs interpolate the chosen temperament table after adding
 * fine/bend. Match that musical position for subs; interpolation is logarithmic
 * in frequency, corresponding to a linear pitch-control voltage. */
static float pitch_frequency(const struct PitchTarget *target, uint16_t quantize, const float *ratios)
{
    float position=target->base+target->detune;
    if(quantize==1 || quantize==4) {
        uint16_t n=(uint16_t)position;
        float fraction=position-(float)n;
        float lo=note_frequency((float)n,1,ratios);
        float hi=note_frequency((float)(n+1),1,ratios);
        float x=fraction*natural_log(hi/lo);
        return lo*(1.0f+x*(1.0f+x*(0.5f+x*(0.166666666667f+x*0.041666666667f))));
    }
    return note_frequency(position,Q_OFF,ratios);
}

#endif
