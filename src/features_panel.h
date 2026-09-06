#ifndef SUBH_FEATURES_PANEL_H
#define SUBH_FEATURES_PANEL_H
#include <stdint.h>
struct PanelChord {
    uint32_t started;
    uint16_t timing;
    uint16_t latched;
};
/* Ordinary assignment presses still act immediately. Only a complete, idle
 * one-second chord is consumed; the caller restores its saved assignments. */
static uint16_t panel_chord_tick(volatile struct PanelChord *c, uint32_t now,
                                 uint16_t down, uint16_t eligible, uint16_t captured)
{
    if(!down) { c->timing=0; c->latched=0; return 0; }
    if(!eligible) { c->timing=0; c->latched=1; return 0; }
    if(down!=3 || captured!=3) { c->timing=0; return 0; }
    if(c->latched) return 0;
    if(!c->timing) { c->started=now; c->timing=1; return 0; }
    if((uint32_t)(now-c->started)<1000000UL) return 0;
    c->latched=1;
    return 1;
}
static uint16_t panel_adjust(uint16_t value, uint16_t step, uint16_t maximum, uint16_t up)
{
    if(up) return maximum-value<step ? maximum : value+step;
    return value<step ? 0 : value-step;
}
#endif
