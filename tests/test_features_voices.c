#include <assert.h>
#include "features_voices.h"
int main(void)
{
    struct VoiceAllocation v={0};
    float pitch[6]={60,60,60,60,60,60};
    unsigned i;
    /* MIDI zero is a real note; unallocated sources stay neutral. */
    assert(voices_assign(&v,pitch,100,0,80,0)==0);
    for(i=0;i<6;++i) assert(pitch[i]==(i ? 60 : 0));
    for(i=1;i<6;++i) assert(voices_assign(&v,pitch,100+i,60+i,90,0)==i);
    assert(voices_assign(&v,pitch,106,72,110,0)==0);
    assert(voices_release(&v,100)==0); /* Stolen instance cannot release its replacement. */
    assert(v.owner[0]==106 && pitch[0]==72);
    assert(voices_release(&v,103)==8);
    assert(pitch[3]==63); /* Caller applies mode-specific pitch policy to the released mask. */
    assert(voices_assign(&v,pitch,107,72,120,0)==3); /* Scan for free slot first. */
    assert(voices_release(&v,106)==1); /* Equal pitch, distinct held instances. */
    assert(v.owner[3]==107 && !v.owner[0]);
    assert(voices_assign(&v,pitch,100,65,40,0)==0); /* Heap address can be reused. */
    voices_clear(&v);
    for(i=0;i<6;++i) assert(!v.owner[i]);
    assert(v.cursor==0 && pitch[0]==65);
    assert(voices_assign(&v,pitch,200,50,70,0)==0);
    assert(pitch[3]==72); /* Clearing ownership does not seed unrelated sources. */
    return 0;
}
