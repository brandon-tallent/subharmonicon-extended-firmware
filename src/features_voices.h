#ifndef SUBH_FEATURES_VOICES_H
#define SUBH_FEATURES_VOICES_H
#include <stdint.h>

/* References into the stock held-note list, never a second held-note list.
 * A stolen node remains held, but no longer owns an oscillator. */
struct VoiceAllocation {
    uint32_t owner[6];
    uint16_t velocity[6];
    uint16_t source[6];
    uint16_t cursor;
};

static void voices_clear(volatile struct VoiceAllocation *v)
{
    uint16_t i;
    for(i=0;i<6;++i) v->owner[i]=0;
    v->cursor=0;
}

static uint16_t voices_assign(volatile struct VoiceAllocation *v,
                             volatile float *pitch, uint32_t owner,
                             uint16_t note, uint16_t velocity, uint16_t source)
{
    uint16_t i,slot=v->cursor;
    for(i=0;i<6;++i) {
        slot=v->cursor+i;
        if(slot>=6) slot-=6;
        if(!v->owner[slot]) break;
    }
    if(i==6) slot=v->cursor;
    v->owner[slot]=owner;
    v->velocity[slot]=velocity;
    v->source[slot]=source;
    pitch[slot]=(float)note;
    v->cursor=slot==5 ? 0 : slot+1;
    return slot;
}

/* Return only slots still owned by this exact held-note instance. */
static uint16_t voices_release(volatile struct VoiceAllocation *v, uint32_t owner)
{
    uint16_t i,mask=0;
    for(i=0;i<6;++i) if(v->owner[i]==owner) {
        v->owner[i]=0;
        mask|=1U<<i;
    }
    return mask;
}
#endif
