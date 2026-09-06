#include <assert.h>
#include <string.h>
#include "features_schedule.h"

static void initialize(struct ResetSchedule *s, uint32_t start)
{
    unsigned i;
    memset(s,0,sizeof(*s));
    for (i=0;i<4;++i) {
        s->period[i]=100000;
        s->fall[i]=start+100*i;
    }
}

static void overlapping_pulses(uint32_t start)
{
    struct ResetSchedule s;
    unsigned i;
    initialize(&s,start);
    for (i=0;i<4+RESET_PULSE_CYCLES/100;++i) {
        uint32_t now=start+100*i;
        uint16_t expected=(i<4 ? 1U<<i : 0) | (i>=RESET_PULSE_CYCLES/100 ? 16U<<(i-RESET_PULSE_CYCLES/100) : 0);
        uint16_t actual;
        if (!expected) continue;
        assert(reset_next(&s)==now);
        assert(reset_plan(&s,now,&actual)==now);
        assert(actual==expected);
        reset_commit(&s,expected,now);
    }
    assert(s.low_mask==0);
    assert(s.resync_count==0);
    for(i=0;i<4;++i) assert(s.pulses[i]==1);
    assert(reset_next(&s)==start+100000);
}

int main(void)
{
    struct ResetSchedule s;
    unsigned i;
    uint16_t events;
    overlapping_pulses(10000);
    overlapping_pulses(UINT32_MAX-150);

    initialize(&s,1000);
    assert(reset_irq_lead(&s)==1500);
    s.period[3]=7972; /* MIDI 127: deliberate waiting <= 1/8 CPU budget. */
    assert(8*reset_irq_lead(&s)<=s.period[3]/8);
    assert(reset_irq_lead(&s)>0);
    initialize(&s,1000);
    for(i=0;i<4;++i) s.fall[i]=1000;
    assert(reset_plan(&s,999,&events)==1000);
    assert(events==15);
    reset_commit(&s,15,1000);
    assert(s.low_mask==15);
    assert(reset_next(&s)==1000+RESET_PULSE_CYCLES);
    assert(reset_plan(&s,1000+RESET_PULSE_CYCLES,&events)==1000+RESET_PULSE_CYCLES);
    assert(events==240);
    reset_commit(&s,240,1000+RESET_PULSE_CYCLES);
    assert(s.low_mask==0);

    initialize(&s,1000);
    assert(reset_plan(&s,201000,&events)==201000);
    reset_commit(&s,events,201000);
    assert(s.resync_count==4);
    assert(reset_next(&s)==201300);
    assert(reset_plan(&s,201300,&events)==201300);
    reset_commit(&s,events,201300);
    assert(reset_next(&s)==301000);
    initialize(&s,1000);
    reset_commit(&s,1,1010);
    assert(s.rise[0]==1000+RESET_PULSE_CYCLES);
    s.fall[3]=s.rise[0];
    s.fall[1]=s.fall[2]=100000;
    assert(reset_plan(&s,s.rise[0],&events)==s.rise[0]);
    assert(events==(16|8));
    return 0;
}
