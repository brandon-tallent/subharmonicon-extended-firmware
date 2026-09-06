#ifndef SUBH_FEATURES_HARMONY_H
#define SUBH_FEATURES_HARMONY_H
#include <stdint.h>

static const int16_t harmony_scales[14][7] = {
  { 0, 2, 4, 5, 7, 9, 11 }, { 0, 2, 3, 5, 7, 9, 10 },
  { 0, 1, 3, 5, 7, 8, 10 }, { 0, 2, 4, 6, 7, 9, 11 },
  { 0, 2, 4, 5, 7, 9, 10 }, { 0, 2, 3, 5, 7, 8, 10 },
  { 0, 1, 3, 5, 6, 8, 10 }, { 0, 2, 3, 5, 7, 8, 11 },
  { 0, 1, 3, 5, 6, 9, 10 }, { 0, 2, 4, 5, 8, 9, 11 },
  { 0, 2, 3, 6, 7, 9, 10 }, { 0, 1, 4, 5, 7, 8, 10 },
  { 0, 3, 4, 6, 7, 9, 11 }, { 0, 1, 3, 4, 6, 8, 9 }
};
static const int16_t harmony_qualities[8][3] = {
  { 3, 7, 11 }, { 3, 6, 9 }, { 3, 6, 10 }, { 3, 7, 10 },
  { 4, 7, 10 }, { 4, 7, 11 }, { 4, 8, 11 }, { 4, 8, 10 }
};
static const int16_t harmony_degree_quality[14][7] = {
  { 5, 3, 3, 5, 4, 3, 2 }, { 3, 3, 5, 4, 3, 2, 5 },
  { 3, 5, 4, 3, 2, 5, 3 }, { 5, 4, 3, 2, 5, 3, 3 },
  { 4, 3, 2, 5, 3, 3, 5 }, { 3, 2, 5, 3, 3, 5, 4 },
  { 2, 5, 3, 3, 5, 4, 3 }, { 0, 2, 6, 3, 4, 5, 1 },
  { 2, 6, 3, 4, 5, 1, 0 }, { 6, 3, 4, 5, 1, 0, 2 },
  { 3, 4, 5, 1, 0, 2, 6 }, { 4, 5, 1, 0, 2, 6, 3 },
  { 5, 1, 0, 2, 6, 3, 4 }, { 1, 0, 2, 6, 3, 4, 5 }
};
static const int16_t harmony_voicings[4][4] = {
  { 0, 0, 0, 0 }, { 0, 0, -12, 0 },
  { 0, -12, 0, 0 }, { 0, 12, 0, 12 }
};
static int16_t harmony_octave(int16_t degree)
{
    int16_t octave=(int16_t)((float)degree*(1.0f/7.0f));
    return degree<octave*7 ? octave-1 : octave;
}
static int16_t harmony_degree(float note,uint16_t scale,uint16_t key)
{
    float relative=note-60.0f-(float)key;
    int16_t octave=(int16_t)(relative/12.0f),best=0,i;
    float pc,distance=100.0f;
    if(relative<(float)(octave*12)) --octave;
    pc=relative-(float)(octave*12);
    for(i=0;i<=7;++i) {
        float delta=pc-(float)(i==7 ? 12 : harmony_scales[scale][i]);
        if(delta<0) delta=-delta;
        if(delta<distance) { distance=delta;best=i; } /* Midpoint ties go lower. */
    }
    return octave*7+best;
}
static int16_t harmony_root_note(int16_t degree,uint16_t scale,uint16_t key)
{
    int16_t octave=harmony_octave(degree),d=degree-octave*7;
    return 60+(int16_t)key+harmony_scales[scale][d]+12*octave;
}
static int16_t harmony_stable_degree(float note,uint16_t scale,uint16_t key,
                                     int16_t previous,uint16_t valid)
{
    int16_t next=harmony_degree(note,scale,key);
    if(valid && next!=previous) {
        /* Shift the input threshold, not the midpoint from a potentially
         * distant previous note. A skipped ADC sample must not change which
         * side of the destination boundary a fast sweep lands on. */
        return harmony_degree(note+(next>previous ? -.08f : .08f),scale,key);
    }
    return next;
}
struct HarmonyChord { int16_t note[4],root;uint16_t quality; };
static void harmony_build(struct HarmonyChord *c,int16_t degree,uint16_t scale,
                          uint16_t key,uint16_t inversion,uint16_t voicing)
{
    int16_t octave=harmony_octave(degree),d=degree-octave*7;
    uint16_t function;
    c->root=harmony_root_note(degree,scale,key);
    c->quality=harmony_degree_quality[scale][d];
    for(function=0;function<4;++function) {
        uint16_t position=(function+4-inversion)%4;
        c->note[function]=c->root+(function ? harmony_qualities[c->quality][function-1] : 0)+
                          (function<inversion ? 12 : 0)+harmony_voicings[voicing][position];
    }
}
#endif
