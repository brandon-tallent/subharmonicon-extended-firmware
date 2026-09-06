/* Four pattern lanes. All addresses below are offsets in the stock rhythm
 * object, in C28x words. The feature panel ingress supplies indices 0..31 and
 * assignment bits 0..3. No independent scheduler or interrupt is introduced.
 */
#include <stdint.h>
#include "features_rhythm.h"
#include "features_rhythm_patterns.h"

/* Independent of pitch mode. MIDI owns a pot until it moves 16 ADC counts;
 * the existing bipolar CV contribution is added by stock code afterwards. */
#pragma DATA_SECTION(feature_rhythm_type, ".feature_settings_state")
volatile uint16_t feature_rhythm_type;
struct RhythmControls { uint16_t owner[4],value[4],raw[4],counter[4]; };
#pragma DATA_SECTION(rhythm_controls, ".feature_settings_state")
static volatile struct RhythmControls rhythm_controls;
static uint16_t rhythm_raw(uint16_t lane)
{
    return *(volatile uint16_t *)(0xd580UL+0x104UL+0x22UL*lane+0x20UL);
}
void feature_rhythm_reset(void)
{
    uint16_t i;
    feature_morph_clock_reset();
    for(i=0;i<4;++i) rhythm_controls.counter[i]=0;
}
void feature_rhythm_init(void)
{
    uint16_t i;
    feature_rhythm_type=1;
    for(i=0;i<4;++i) {
        rhythm_controls.owner[i]=0;rhythm_controls.value[i]=0;rhythm_controls.raw[i]=0;
    }
    feature_rhythm_reset();
}
void feature_rhythm_cc(uint16_t lane,uint16_t value)
{
    rhythm_controls.raw[lane]=rhythm_raw(lane);
    rhythm_controls.value[lane]=31-(value>>2);
    rhythm_controls.owner[lane]=1;
}
uint16_t feature_rhythm_knob(uint16_t lane,uint16_t physical)
{
    int16_t movement=(int16_t)rhythm_raw(lane)-(int16_t)rhythm_controls.raw[lane];
    if(movement>=16 || movement<=-16) rhythm_controls.owner[lane]=0;
    return rhythm_controls.owner[lane] ? rhythm_controls.value[lane] : physical;
}

/* The stock knob/CV converter descends clockwise. The pattern bank begins
 * with straight note values and then moves through various phrases. 
 * Internal and MIDI timing use 24 PPQN, with sixteenth-grid pattern hits.
 */
static void rhythm_advance(volatile uint16_t *r, uint16_t lane,
                           uint16_t event, uint16_t manual)
{
    r[0x15+lane]=event;
    if(!event) return;
    if(r[3]) r[0x12+lane]=r[4];
    else if(manual || !r[1+lane])
        r[0x12+lane]=(r[0x12+lane]+1)&3;
    r[1+lane]=0;
}

uint16_t feature_rhythm_tick(volatile uint16_t *r, uint16_t manual)
{
    uint16_t event[4],i,phase=r[0x0a],seq1=0,seq2=0,vca=0,vcf=0;
    /* Stock source selector: physical CLOCK bypasses the 24-PPQN driver.
     * One edge is one pattern sixteenth, or one Stock divider count. */
    uint16_t physical=*(volatile uint16_t *)0xc4deUL==2;
    uint16_t stride=physical ? 6 : 1;
    if(physical && !manual) {
        /* On source handoff, consume the next unplayed sixteenth. */
        phase=((phase+5)/6)*6;
        if(phase>=RHYTHM_PHRASE_TICKS) phase=0;
    }
    /* Clear publication even on stopped ticks: a stale articulation must
     * never leak through the normal clock dispatch after STOP.
     */
    r[0x0b]=0;r[0x0c]=0;r[0x0d]=0;r[0x14]=0;r[0x15]=0;r[0x16]=0;
    if(!manual) feature_morph_clock(phase,r[0]);
    if(!r[0] && !manual) return 0;
    if(manual) {
        /* NEXT is bounded even with silent or unassigned patterns. It
         * auditions pitch positions without moving the bar or firing EGs.
         */
        rhythm_advance(r,0,1,1);rhythm_advance(r,1,1,1);
        r[0x14]=1;
        return 1;
    }
    if(feature_rhythm_type) {
        for(i=0;i<4;++i)
            event[i]=(rhythm_patterns[31-r[6+i]][phase>>4]>>(phase&15))&1;
        /* R1/R2 route pitch; R3/R4 independently route VCA/VCF. */
        for(i=0;i<2;++i) {
            if(event[i] && (r[0x0e + i]&1)) seq1=1;
            if(event[i] && (r[0x0e + i]&2)) seq2=1;
            if(event[2+i] && (r[0x10+i]&1)) vca=1;
            if(event[2+i] && (r[0x10+i]&2)) vcf=1;
        }
    } else if(physical || (phase%24)==0) {
        /* Stock periods 1..16 quarters internally, or physical input edges.
         * All four rhythms route both sequences.
         * CC113 uses the original exclusive-one (not parity XOR) rule. */
        for(i=0;i<4;++i) {
            uint16_t counter=rhythm_controls.counter[i];
            if(!counter) {
                if(r[0x0e + i]&1) ++seq1;
                if(r[0x0e + i]&2) ++seq2;
            }
            rhythm_controls.counter[i]=counter>=(r[6+i]>>1) ? 0 : counter+1;
        }
        seq1=r[5]==1 ? seq1==1 : seq1!=0;
        seq2=r[5]==1 ? seq2==1 : seq2!=0;
        vca=seq1;vcf=seq1;
    }
    rhythm_advance(r,0,seq1,0);rhythm_advance(r,1,seq2,0);
    r[0x14]=seq1|seq2;r[0x0b]=vca;r[0x0c]=vcf;
    /* Physical CLOCK OUT follows every accepted edge. No interpolation. */
    r[0x0d]=physical || (phase%6)==0;
    phase+=stride;
    r[0x0a]=phase>=RHYTHM_PHRASE_TICKS ? 0 : phase;
    return seq1|seq2;
}
