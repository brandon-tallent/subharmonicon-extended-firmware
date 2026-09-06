#ifndef SUBH_FEATURES_MORPH_H
#define SUBH_FEATURES_MORPH_H
#include <stdint.h>
#define MORPH_MIN_PERIOD 8000UL
#define MORPH_MIN_INTERVAL 2000UL
/* Eight interpolated timing shapes, each totaling eight nominal periods.
 * Zero weights still receive a 20-us interval, preserving the reset pulse
 * and a real ramp interval. Integer rounding is absorbed by the longest slot.
 * Brake/accelerate use Nihilo's geometric idea, compressing its 0.85
 * restitution into eight slots with a 0.85^2 interval ratio. The curves are
 * generated here as normalized weights, not copied scheduling code.
 * All weights sum to 256. Shape calculations run outside the reset ISR. */
static uint32_t morph_shape(uint32_t shape[8], uint32_t period, uint16_t position)
{
    static const uint16_t weights[8][8]={
        {32,32,32,32,32,32,32,32}, /* even */
        {64,0,64,0,64,0,64,0},     /* alternating */
        {8,11,15,21,29,40,55,77},   /* brake: geometrically widening */
        {77,55,40,29,21,15,11,8},   /* accelerate: geometric bounce */
        {112,0,0,16,112,0,0,16},   /* double burst */
        {80,0,0,80,0,0,80,16},     /* triple burst */
        {4,76,8,12,96,4,48,8},     /* irregular */
        {256,0,0,0,0,0,0,0}        /* single long ramp, seven short resets */
    };
    float scan=(float)position*(7.0f/16383.0f), fraction, budget;
    uint16_t frame=(uint16_t)scan, i, longest=0;
    uint32_t sum=0;
    if(frame==7) {frame=6;fraction=1.0f;}
    else fraction=scan-(float)frame;
    budget=(float)(8UL*(period-MORPH_MIN_INTERVAL))*(1.0f/256.0f);
    for(i=0;i<8;++i) {
        float weight=(float)weights[frame][i]+fraction*
                     ((float)weights[frame+1][i]-(float)weights[frame][i]);
        shape[i]=MORPH_MIN_INTERVAL+(uint32_t)(budget*weight);
        sum+=shape[i];
        if(shape[i]>shape[longest]) longest=i;
    }
    shape[longest]+=8UL*period-sum;
    /* At long periods, float rounding can make the correction negative.
     * Re-find the maximum after correcting a formerly tied longest slot. */
    for(i=0;i<8;++i) if(shape[i]>shape[longest]) longest=i;
    return shape[longest];
}
#endif
