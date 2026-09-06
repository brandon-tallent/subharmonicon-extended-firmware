#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "features_glide.h"

static void trajectory(float start,float target,float tau)
{
    struct GlideVoice v;
    float alpha=glide_coefficient(tau),previous=start;
    unsigned i,steps=(unsigned)(tau*12.0f/GLIDE_TICK_US);
    glide_target(&v,start,1);
    glide_target(&v,target,0);
    for(i=1;i<=steps;++i) {
        float actual=glide_advance(&v,alpha);
        double expected=target+(start-target)*exp(-(double)i*GLIDE_TICK_US/tau);
        assert(fabs(actual-expected)<0.0002); /* < .02 cent for MIDI pitch. */
        assert(actual>=fminf(start,target) && actual<=fmaxf(start,target));
        assert(target>start ? actual>=previous : actual<=previous);
        previous=actual;
    }
    assert(fabsf(previous-target)<fabsf(start-target)*0.00001f);
}

int main(void)
{
    unsigned value;
    float previous=0;
    struct GlideVoice v;
    assert(glide_time_us(0)==22000);
    trajectory(0,1,GLIDE_DEFAULT_US);
    for(value=1;value<=16383;++value) {
        float tau=glide_time_us(value),alpha=glide_coefficient(tau);
        assert(tau>=10000 && tau<=2000000 && tau>=previous);
        assert(fabs(alpha/(1-exp(-200.0/tau))-1)<0.000001);
        previous=tau;
    }
    trajectory(48,84,10000);
    trajectory(84,48,80000);
    trajectory(48,84,2000000);
    trajectory(.8f,.2f,80000); /* Parent calibrated CV moves downward. */
    glide_target(&v,60,1);
    assert(glide_advance(&v,.01f)==60); /* New articulation snaps. */
    glide_target(&v,72,0);
    {
        float before=glide_advance(&v,.01f);
        glide_target(&v,48,0); /* Steal/retarget starts at the current pitch. */
        assert(fabsf((v.target-v.remaining)-before)<0.00001f);
        glide_target(&v,61.25f,1); /* Disable snaps, preserving fractions. */
        assert(glide_advance(&v,.01f)==61.25f);
    }
    puts("PASS: 5-kHz RC glide timing, monotonicity, slow convergence, retarget continuity, and snaps");
}
