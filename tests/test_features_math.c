#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "features_math.h"
int main(void) {
    const float ratios[12]={1,16.0f/15,9.0f/8,6.0f/5,5.0f/4,4.0f/3,7.0f/5,3.0f/2,8.0f/5,5.0f/3,16.0f/9,15.0f/8};
    const unsigned modes[5]={0,1,3,4,5};
    unsigned m,n;
    for(m=0;m<5;++m) for(n=0;n<128;++n) {
        struct PitchTarget target;
        pitch_target(&target,60,(float)n-60,modes[m]);
        assert(target.base+target.detune==(float)n && !target.clamped);
        assert(fabsf(pitch_frequency(&target,modes[m],ratios)-note_frequency((float)n,(modes[m]==1 || modes[m]==4) ? 1 : 5,ratios))<.002f);
    }
    for(m=0;m<5;++m) {
        float previous=0;
        for(n=0;n<=127*128;++n) {
            float hz=note_frequency(n/128.0f,modes[m],ratios);
            assert(isfinite(hz) && hz>=8 && hz<=13000);
            assert(hz>=previous); previous=hz;
            if(modes[m]==5) {
                float exact=440.0f*exp2f(((float)n/128.0f-69.0f)/12.0f);
                assert(fabsf(1200.0f*log2f(hz/exact))<.002f);
            }
            assert(fabsf(natural_log(hz)-logf(hz))<0.00015f);
            assert(fabsf(1200.0f*log2f(frequency_from_log(natural_log(hz))/hz))<.01f);
        }
    }
    assert(fabsf(note_frequency(69,0,ratios)-440)<0.001f);
    assert(note_frequency(61,3,ratios)==note_frequency(60,3,ratios));
    assert(fabsf(note_frequency(60,1,ratios)-8.18f*32)<0.001f);
    assert(note_frequency(60.5f,5,ratios)>note_frequency(60,5,ratios));
    assert(note_frequency(60.5f,0,ratios)==note_frequency(60,0,ratios));
    {
        struct PitchTarget t;
        pitch_target(&t,45,6,0);
        assert(t.base+t.detune==51 && !t.clamped);
        pitch_target(&t,90,-20,0);
        assert(t.base+t.detune==70 && !t.clamped);
        pitch_target(&t,127,0,0);
        assert(t.base+t.detune==127 && !t.clamped);
        pitch_target(&t,0,0,0);
        assert(t.base+t.detune==0 && !t.clamped);
        pitch_target(&t,-24,0,5);
        assert(t.base+t.detune==0 && t.clamped);
        pitch_target(&t,151,0,5);
        assert(t.base+t.detune==127 && t.clamped);
        pitch_target(&t,61,.5f,3);
        assert(t.base==60 && t.detune==.5f); /* Bend survives eight-note quantization. */
        pitch_target(&t,60.25f,.5f,5);
        assert(t.base+t.detune==60.75f);
    }
    for(n=0;n<=127*128;++n) {
        struct PitchTarget t;
        float expected=440.0f*exp2f(((float)n/128.0f-69.0f)/12.0f);
        t.base=60; t.detune=(float)n/128.0f-60;
        assert(fabsf(1200.0f*log2f(pitch_frequency(&t,0,ratios)/expected))<.002f);
    }
    {
        struct PitchTarget t;
        float expected=sqrtf(note_frequency(60,1,ratios)*note_frequency(61,1,ratios));
        pitch_target(&t,61,.5f,4); /* Eight-note base folds; bend stays continuous. */
        assert(fabsf(1200.0f*log2f(pitch_frequency(&t,4,ratios)/expected))<.002f);
        t.detune=1;
        assert(fabsf(pitch_frequency(&t,4,ratios)-note_frequency(61,1,ratios))<.001f);
    }
    puts("PASS: all quantizer modes monotonic across the admitted band; stock JI anchor and log error bounded");
}
