#include <assert.h>
#include "features_voices.h"
int main(void)
{
    struct VoiceAllocation v={0};
    float pitch[6]={60,60,60,60,60,60};
    unsigned i;
    /* MIDI zero is a real note, including the initial seed. */
    assert(voices_assign(&v,pitch,100,0,80,0)==0);
    for(i=0;i<6;++i) assert(pitch[i]==0);
    for(i=1;i<6;++i) assert(voices_assign(&v,pitch,100+i,60+i,90,0)==i);
    assert(voices_assign(&v,pitch,106,72,110,0)==0);
    voices_release(&v,100); /* Old stolen instance cannot release its replacement. */
    assert(v.owner[0]==106 && pitch[0]==72);
    voices_release(&v,103);
    assert(pitch[3]==63); /* Note-off retains oscillator pitch. */
    assert(voices_assign(&v,pitch,107,72,120,0)==3); /* Scan for free slot first. */
    voices_release(&v,106); /* Equal pitch, distinct held instances. */
    assert(v.owner[3]==107 && !v.owner[0]);
    assert(voices_assign(&v,pitch,100,65,40,0)==0); /* Heap address can be reused. */
    voices_clear(&v);
    for(i=0;i<6;++i) assert(!v.owner[i]);
    assert(v.cursor==0 && v.initialized && pitch[0]==65);
    assert(voices_assign(&v,pitch,200,50,70,0)==0);
    assert(pitch[3]==72); /* Reset retains established pitch memories. */
    return 0;
}
