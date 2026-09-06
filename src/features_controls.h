#ifndef SUBH_FEATURES_CONTROLS_H
#define SUBH_FEATURES_CONTROLS_H
#include <stdint.h>

/* All six intervals use the same last-movement rule. Changes arriving in one
 * control pass resolve to MIDI. Small panel noise cannot seize MIDI ownership. */
struct PitchControl {
    uint16_t panel;
    uint16_t midi;
    uint16_t value;
    uint16_t bin;
    uint16_t bin_valid;
    uint16_t midi_owner;
};
/* Exact neutral at CC64, endpoints at CC0/127. The upper half has 63 steps,
 * the lower 64; preserve this asymmetry when expanding to 14-bit control. */
static uint16_t pitch_midi_7bit_unit(uint16_t cc)
{
    if(cc<=64) return cc<<7;
    return 8192U+(cc-64U)*130U+(cc==127);
}
static float pitch_centered_interval(uint16_t value, uint16_t radius, uint16_t deadband)
{
    int16_t offset=(int16_t)value-8192;
    if(offset<-(int16_t)deadband)
        return (float)(offset+(int16_t)deadband)*((float)radius/(8192.0f-(float)deadband));
    if(offset>(int16_t)deadband)
        return (float)(offset-(int16_t)deadband)*((float)radius/(8191.0f-(float)deadband));
    return 0.0f;
}
static void pitch_control_seed(volatile struct PitchControl *c, uint16_t panel, uint16_t midi)
{
    c->panel=panel; c->midi=midi; c->value=panel; c->bin_valid=0; c->midi_owner=0;
}
static float pitch_control_interval(volatile struct PitchControl *c,
                                    uint16_t panel, uint16_t midi,
                                    uint16_t span, uint16_t quantize)
{
    int16_t movement=(int16_t)panel-(int16_t)c->panel;
    float scaled;
    uint16_t bin;
    if(movement>=16 || movement<=-16) { c->panel=panel; c->value=panel; c->midi_owner=0; }
    /* Once the panel owns pitch, follow its settled sample too. The movement
     * threshold protects MIDI ownership; it must not strand the final knob
     * value at whichever sample happened to cross it during a fast sweep. */
    if(!c->midi_owner) c->value=panel;
    if(midi!=c->midi) { c->midi=midi; c->value=midi; c->midi_owner=1; }
    if(quantize==5) {
        c->bin_valid=0;
        return pitch_centered_interval(c->value,span,c->midi_owner ? 0 : 128);
    }
    scaled=pitch_centered_interval(c->value,span,c->midi_owner ? 0 : 128)+(float)span+0.5f;
    if(!c->bin_valid) bin=(uint16_t)scaled;
    else if(scaled>=(float)c->bin+1.08f) bin=(uint16_t)(scaled-.08f);
    else if(scaled<(float)c->bin-.08f) bin=(uint16_t)(scaled+.08f);
    else bin=c->bin;
    if(bin>2*span) bin=2*span;
    c->bin=bin; c->bin_valid=1;
    return (float)bin-(float)span;
}
static float pitch_sequence_interval(float unit, uint16_t span, uint16_t quantize)
{
    float value=-(float)span+(float)(2*span+1)*unit;
    return quantize==5 ? value : (float)(int16_t)value;
}
#endif
