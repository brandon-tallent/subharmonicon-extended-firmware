#include <assert.h>
#include "features_harmony.h"
int main(void)
{
    static const int16_t steps[2][7]={{2,2,1,2,2,2,1},{2,1,2,2,1,3,1}};
    unsigned scale,d,f,inv,voicing,key;
    for(scale=0;scale<14;++scale) {
        int tone=0;
        for(d=0;d<7;++d) {
            assert(harmony_scales[scale][d]==tone);
            tone+=steps[scale/7][(scale%7+d)%7];
            for(f=1;f<4;++f) {
                int step=d+2*f;
                int interval=harmony_scales[scale][step%7]+12*(step/7)-harmony_scales[scale][d];
                assert(harmony_qualities[harmony_degree_quality[scale][d]][f-1]==interval);
            }
        }
        for(key=0;key<12;++key) for(d=0;d<128;++d) {
            int16_t degree=harmony_degree((float)d,scale,key);
            struct HarmonyChord c;harmony_build(&c,degree,scale,key,0,0);
            assert(c.root>=-1 && c.root<=128);
            for(inv=0;inv<4;++inv) for(voicing=0;voicing<4;++voicing) {
                struct HarmonyChord v;harmony_build(&v,degree,scale,key,inv,voicing);
                for(f=0;f<4;++f) {
                    int pc=(v.note[f]-v.root)%12;if(pc<0) pc+=12;
                    assert(pc==(f ? harmony_qualities[v.quality][f-1] : 0));
                }
            }
        }
    }
    assert(harmony_degree(61,0,0)==0);
    assert(harmony_stable_degree(61.04f,0,0,0,1)==0);
    assert(harmony_stable_degree(61.09f,0,0,0,1)==1);
    assert(harmony_stable_degree(60.96f,0,0,1,1)==1);
    assert(harmony_stable_degree(60.91f,0,0,1,1)==0);
    assert(harmony_degree(59,0,0)==-1);
    assert(harmony_degree(72,0,0)==7);
    {
        struct HarmonyChord c;
        harmony_build(&c,0,0,0,1,1); /* Cmaj7 drop2 first inversion, function order. */
        assert(c.note[0]==72 && c.note[1]==64 && c.note[2]==67 && c.note[3]==59);
        harmony_build(&c,4,7,0,0,0); /* Harmonic-minor V is dominant. */
        assert(c.quality==4 && c.note[0]==67 && c.note[1]==71 && c.note[2]==74 && c.note[3]==77);
    }
    return 0;
}
