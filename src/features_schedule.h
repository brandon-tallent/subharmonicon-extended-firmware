#ifndef SUBH_FEATURES_SCHEDULE_H
#define SUBH_FEATURES_SCHEDULE_H
#include <stdint.h>
#define RESET_PULSE_CYCLES 1000UL
#define RESET_MIN_PULSE_CYCLES 300UL
#define NOISE_MIN_CYCLES 5000UL
#define NOISE_RANGE_MASK 32767UL

struct ResetSchedule {
    uint32_t period[4], fall[4], rise[4], pulses[4];
    uint32_t late_cycles_max, resync_count;
    uint16_t low_mask;
    uint16_t disabled_mask, noise;
    uint32_t random;
    uint16_t morph; /* SUB1 reset mask: 0 or 5. */
    uint32_t shape[2][8];
    uint16_t grid_valid, grid_slot[2];
    uint32_t grid_epoch, grid_cycles, grid_expires;
};

/* SUB2 phase frequency-modulates its paired SUB1 reset intervals in Morph.
 * Sample the scheduled SUB2 phase at the SUB1 edge: first half-cycle contracts
 * to 1/4, second half expands to 7/4. This is interval modulation, not an
 * extra reset on every SUB2 edge. A noise replacement is not a periodic source.
 * The 20-us floor preserves at least 10 us of ramp after the normal pulse. */
static uint32_t morph_reset_period(const volatile struct ResetSchedule *s,
                                    uint16_t sub, uint32_t now)
{
    uint32_t interval=s->shape[sub>>1][s->pulses[sub]&7];
    uint16_t mod=sub+1;
    if(!(mod==3 && s->noise)) {
        if((int32_t)(s->fall[mod]-now)>(int32_t)(s->period[mod]>>1)) interval>>=2;
        else interval+=interval-(interval>>2);
    }
    if(interval<2000UL) interval=2000UL;
    /* Pull only the reset after the longest ramp toward a nearby sixteenth.
     * Outside the quarter-grid capture window, retain the free interval. */
    if(s->grid_valid && (s->pulses[sub]&7)==s->grid_slot[sub>>1] &&
       (int32_t)(s->grid_expires-now)>0) {
        uint32_t planned=s->fall[sub]+interval;
        int32_t relative=(int32_t)(planned-s->grid_epoch);
        if(relative>=0) {
            uint32_t line=(uint32_t)((float)relative/(float)s->grid_cycles+0.5f);
            int32_t delta=(int32_t)(s->grid_epoch+line*s->grid_cycles-planned);
            int32_t radius=(int32_t)(s->grid_cycles>>2);
            if(delta>=-radius && delta<=radius && (int32_t)interval+delta>=2000)
                interval=(uint32_t)((int32_t)interval+delta);
        }
    }
    return interval;
}

/* A bar restart changes future deadlines, never GPIO levels. Pending reset
 * pulses must release before the aligned assertion; noise remains free. */
static inline void morph_bar_anchor(volatile struct ResetSchedule *s, uint32_t when)
{
    uint16_t i;
    for(i=0;i<4;++i) {
        uint32_t next=when;
        if(i==3 && s->noise) continue;
        if((s->low_mask&(1U<<i)) &&
           (int32_t)(next-s->rise[i])<(int32_t)RESET_MIN_PULSE_CYCLES)
            next=s->rise[i]+RESET_MIN_PULSE_CYCLES;
        s->fall[i]=next;s->pulses[i]=0;
    }
}

/* At most eight edges per shortest period: reserving <= period/64 per edge
 * bounds deliberate waiting to 1/8 of CPU time, before ISR bookkeeping.
 * Keep the previously qualified 15-us lead for slower pitches. This does
 * not establish a measured worst-case ISR cost at the new high-note limit. */
static uint32_t reset_irq_lead(const volatile struct ResetSchedule *s)
{
    uint16_t i;
    uint32_t lead=1500;
    for(i=0;i<4;++i) {
        if(s->disabled_mask & (1U<<i)) continue;
        uint32_t candidate=s->period[i]>>6;
        if(s->morph&(1U<<i)) candidate=2000UL>>6;
        if(i==3 && s->noise) candidate=NOISE_MIN_CYCLES>>6;
        if(candidate<lead) lead=candidate;
    }
    return lead;
}

/* All future events are less than 2^31 ticks away. Signed differences preserve
 * their order when the continuous timestamp counter wraps. */
static uint32_t reset_next(const volatile struct ResetSchedule *s)
{
    uint16_t i;
    uint32_t next=(s->low_mask&8) ? s->rise[3] : s->fall[3];
    for (i=0; i<4; ++i) {
        if(s->disabled_mask & (1U<<i)) continue;
        uint32_t edge=(s->low_mask&(1U<<i)) ? s->rise[i] : s->fall[i];
        if ((int32_t)(edge-next)<0) next=edge;
    }
    return next;
}

/* Low nibble asserts resets; high nibble releases them. Determine masks before
 * waiting to the deadline, keeping bookkeeping out of the timed GPIO edge. */
static uint32_t reset_plan(const volatile struct ResetSchedule *s, uint32_t now,
                           uint16_t *events_out)
{
    uint16_t i, events=0;
    uint32_t next=now;
    for (i=0; i<4; ++i) {
        uint16_t edge;
        uint32_t candidate;
        if(s->disabled_mask & (1U<<i)) continue;
        /* Release every held reset before considering its next assertion. */
        edge=(s->low_mask&(1U<<i)) ? 16U<<i : 1U<<i;
        candidate=(edge&15) ? s->fall[i] : s->rise[i];
        if ((int32_t)(candidate-now)<0) candidate=now;
        if (!events || (int32_t)(candidate-next)<0) {
            next=candidate; events=edge;
        } else if(candidate==next) events|=edge;
    }
    *events_out=events;
    return next;
}

static void reset_commit(volatile struct ResetSchedule *s, uint16_t events,
                         uint32_t actual)
{
    uint16_t i;
    s->low_mask &= ~(events>>4);
    for (i=0; i<4; ++i) {
        /* An overdue release still leaves a real ramp before reassertion. */
        if((events&(16U<<i)) &&
           (int32_t)(s->fall[i]-actual)<(int32_t)RESET_MIN_PULSE_CYCLES) {
            ++s->resync_count;
            s->fall[i]=actual+RESET_MIN_PULSE_CYCLES;
        }
        if (events & (1U<<i)) {
            uint32_t period=s->period[i];
            uint32_t late=actual-s->fall[i];
            if(s->morph&(1U<<i)) period=morph_reset_period(s,i,actual);
            /* Fresh full-range randomness each cycle, independent of note,
             * parent capture and sequencer. Seeded nonzero before enabling.
             * 50..377.67 us bounds ramp growth and interrupt density. */
            if(i==3 && s->noise) {
                uint32_t x=s->random;
                x^=x<<13;x^=x>>17;x^=x<<5;
                s->random=x;
                period=NOISE_MIN_CYCLES+(x&NOISE_RANGE_MASK);
            }
            if (late>s->late_cycles_max) s->late_cycles_max=late;
            /* Both edges follow the absolute schedule. Do not transfer an
             * assertion's software latency into the audible release edge.
             * Ten microseconds allows bookkeeping between edges; an overdue
             * assertion still receives at least the proven 3-us reset width. */
            s->rise[i]=s->fall[i]+RESET_PULSE_CYCLES;
            if ((int32_t)(s->rise[i]-actual)<(int32_t)RESET_MIN_PULSE_CYCLES)
                s->rise[i]=actual+RESET_MIN_PULSE_CYCLES;
            s->fall[i]+=period;
            if ((int32_t)(actual-s->fall[i])>=0) {
                ++s->resync_count;
                s->fall[i]=actual+period;
            }
            s->low_mask|=1U<<i;
            ++s->pulses[i];
        }
    }
}
#endif
