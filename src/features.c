/* firmware mod features.
 * C28x addresses and sizes are in 16-bit words; uint32_t accesses are atomic.
 */
#include <stdint.h>

#define REG16(a) (*(volatile uint16_t *)(a))
#define REG32(a) (*(volatile uint32_t *)(a))
#define ENGINE 0xe380UL
#define CONTROLS 0xd580UL
#define TIMER2 0x0c10UL
#define ECAP5 0x5300UL
#define GPIO 0x7c00UL
#define GPDATA 0x7f00UL
#define CLOCK_HZ 100000000.0f
#define MIN_REARM_CYCLES 100UL
#define Q_OFF 5U

extern void feature_output(volatile uint16_t *, float);
extern float feature_parent_cv(uint16_t, float, float);
extern void feature_stock_refresh(void);
extern void feature_clear_notes(void);
extern void feature_gate_on(void);
extern void feature_gate_off(void);
extern void feature_close_gates(void);
extern uint32_t feature_micros(void);
extern void feature_save_config(void);
extern void feature_enable_timer_irq(void);
extern void feature_disable_timer_irq(void);
extern uint16_t feature_fast_load, feature_fast_run, feature_fast_size;

/* These hot pitch routines share the already reserved LS3 execution bank
 * with reset scheduling. The real linker bounds the complete copied section. */
#pragma CODE_SECTION(feature_tick, ".feature_fast")
#pragma CODE_SECTION(feature_harmony_targets, ".feature_fast")
#pragma CODE_SECTION(pitch_control_interval, ".feature_fast")
#pragma CODE_SECTION(harmony_degree, ".feature_fast")
#pragma CODE_SECTION(harmony_stable_degree, ".feature_fast")
#pragma CODE_SECTION(pitch_target, ".feature_fast")
#pragma CODE_SECTION(natural_log, ".feature_fast")
#pragma CODE_SECTION(reset_next, ".feature_fast")
#pragma CODE_SECTION(reset_plan, ".feature_fast")
#pragma CODE_SECTION(reset_commit, ".feature_fast")
#pragma FUNC_CANNOT_INLINE(reset_irq_lead)
#pragma FUNC_CANNOT_INLINE(morph_reset_period)
#include "features_schedule.h"
#include "features_morph.h"
#include "features_voices.h"
#include "features_controls.h"
#include "features_panel.h"
#include "features_config.h"
#include "features_glide.h"
#include "features_velocity.h"
#pragma DATA_SECTION(feature_velocity_depth, ".feature_velocity_state")
#pragma DATA_SECTION(feature_velocity_last, ".feature_velocity_state")
volatile uint16_t feature_velocity_depth, feature_velocity_last;
struct FeatureSettings {
    uint32_t last_activity;
    uint16_t active, draining, page, fine, dirty;
    uint16_t edited[8], recall[7];
    struct PanelChord chord;
    uint16_t captured, saved[2];

};
#pragma DATA_SECTION(feature_settings, ".feature_settings_state")
volatile struct FeatureSettings feature_settings;
#pragma DATA_SECTION(feature_morph_position, ".feature_settings_state")
volatile uint16_t feature_morph_position[2];
struct MorphGrid { uint32_t last, margin[2]; uint16_t known; };
#pragma DATA_SECTION(feature_morph_grid, ".feature_settings_state")
volatile struct MorphGrid feature_morph_grid;
extern void feature_setting_led(volatile uint16_t *, uint16_t);
extern void feature_refresh_panel_references(void);
extern uint16_t feature_setting_button_edge(volatile uint16_t *);


#include "features_rhythm.h"

#include "features_harmony.h"
struct FeatureHarmony {
    uint16_t enabled,spread,valid;
    float root;
    int16_t degree,melody_degree;
    uint16_t parameter[4],bin_valid[4];
    struct HarmonyChord chord;
};
#pragma DATA_SECTION(feature_harmony, ".feature_harmony_state")
volatile struct FeatureHarmony feature_harmony;

struct FeatureGlide {
    struct GlideVoice voice[6];
    float alpha;
    uint16_t enabled, time, snap_mask, active_mask, tied_mask;
};
#pragma DATA_SECTION(feature_glide, ".feature_glide_state")
volatile struct FeatureGlide feature_glide;

struct FeatureState {
    uint16_t enabled; /* First word is read by the stock capture hook. */
    uint16_t allocation; /* Second word is read by MIDI hooks. */
    uint16_t dirty;
    uint16_t quantize;
    uint16_t fault;
    float note[6];
    struct VoiceAllocation voices;
    uint16_t midi_gate;
    uint16_t clamped_mask;
    uint16_t pitch_controls;
    uint16_t knob_span;
    struct PitchControl control[6];
    float published_base[6];
    float published_detune[6];
    uint16_t published_mask;
    struct FeatureConfig persisted;
    uint16_t config_pending;
    uint16_t config_invalid;
    struct ResetSchedule reset;
    uint32_t early_interrupts;
    uint32_t saved_mux;
    uint32_t saved_gmux;
    uint32_t saved_dir;
    uint32_t irq_lead;
};
volatile struct FeatureState feature_state;
/* Engine active with only GPIO3 owned; stock owns the other five pitches. */
#pragma DATA_SECTION(feature_noise_only, ".feature_settings_state")
volatile uint16_t feature_noise_only;
struct FeatureClock { uint32_t boundary; uint16_t valid; };
#pragma DATA_SECTION(feature_clock, ".feature_settings_state")
static volatile struct FeatureClock feature_clock;

/* Called with interrupts masked at Timer 1 entry, only after independent
 * reset ownership has started eCAP5 and copied the RAM code. Align the free
 * counter to Timer 1's current hardware epoch: now_us adds its sub-tick part.
 * Retaining fractional cycles prevents rounding drift; unsigned subtraction
 * handles the 42.95-second eCAP wrap. No catch-up control callbacks are run. */
#pragma CODE_SECTION(feature_scheduler_advance, ".feature_fast")
void feature_scheduler_advance(void)
{
    uint32_t before=REG32(0x0c08UL),counter=REG32(ECAP5),after=REG32(0x0c08UL);
    uint32_t boundary,elapsed;
    if(after>before) counter=REG32(ECAP5); /* Timer reloaded during sampling. */
    boundary=counter-(REG32(0x0c0aUL)-after);
    if(!feature_clock.valid) {
        feature_clock.boundary=boundary;feature_clock.valid=1;
        REG32(0xc0c2UL)+=REG32(0xc0c0UL);
        return;
    }
    elapsed=boundary-feature_clock.boundary;
    /* A pending interrupt can be serviced twice within one hardware epoch.
     * Sampling skew must not reinterpret that boundary as a counter wrap. */
    if((int32_t)elapsed<=0) return;
    elapsed/=100UL;
    feature_clock.boundary+=elapsed*100UL;
    REG32(0xc0c2UL)+=elapsed;
}


/* Assembly ADC ingress handshake: ready timestamp, in-flight flag, skips.
 * A reset ISR can postpone scan completion past the next 40-kHz request.
 * Never queue another bank until the previous result has been consumed and
 * its newly selected mux bank has had a full 25-us interval to settle. */
#pragma DATA_SECTION(feature_adc_scan, ".feature_scan_state")
#pragma DATA_ALIGN(feature_adc_scan, 2)
volatile uint16_t feature_adc_scan[4];

static const uint16_t binding[6] = {0x0ca4,0x0c54,0x0c5e,0x0cae,0x0c7c,0x0c86};
static const uint16_t panel_offset[6] = {0x1ac,0x212,0x256,0x234,0x278,0x2de};
static const uint16_t midi_address[6] = {0xc2ab,0xc2b1,0xc2b2,0xc2ac,0xc2b3,0xc2b4};
static const uint16_t assign_offset[6] = {0xd2b,0xd2d,0xd2e,0xd2c,0xd2f,0xd30};
static const uint16_t sub_index[6] = {0,0,1,0,2,3};
static const uint16_t gain_offset[4] = {0x0b62,0x0b66,0x0b6a,0x0b6e};
#include "features_math.h"

#pragma CODE_SECTION(feature_now, ".feature_fast")
static uint32_t feature_now(void) { return REG32(ECAP5); }

/* A stopped peripheral must not lock the CPU with interrupts masked. */
#pragma CODE_SECTION(feature_wait_until, ".feature_fast")
#include "features_clock.h"

#pragma CODE_SECTION(feature_arm_timer, ".feature_fast")
static void feature_arm_timer(uint32_t next)
{
    int32_t remaining=(int32_t)(next-feature_now())-(int32_t)feature_state.irq_lead;
    uint32_t delay=remaining>(int32_t)MIN_REARM_CYCLES ? (uint32_t)remaining : MIN_REARM_CYCLES;
    REG16(TIMER2+4)=0x8010; /* TSS + clear pending TIF */
    REG32(TIMER2+2)=delay-1;
    REG16(TIMER2+4)=0xc020; /* TIE + TRB + TIF; run */
}

/* Accepted rhythm-clock ingress; phase is 24 PPQN, 96 ticks per bar. */
void feature_morph_clock_reset(void)
{
    uint16_t irq=__disable_interrupts();
    feature_state.reset.grid_valid=0;feature_morph_grid.known=0;
    __restore_interrupts(irq);
}

void feature_morph_clock(uint16_t phase, uint16_t running)
{
    uint32_t now, elapsed;
    uint16_t irq;
    if(!running || !feature_state.enabled || !feature_state.reset.morph) {
        feature_morph_clock_reset();return;
    }
    if(phase%6) return;
    irq=__disable_interrupts();now=feature_now();
    elapsed=now-feature_morph_grid.last;
    /* The estimator admits 20..3000 BPM and tolerates scheduler jitter.
     * A paused/missing source must not publish an arbitrarily long grid. */
    if(feature_morph_grid.known && elapsed>=400000UL && elapsed<=80000000UL) {
        feature_state.reset.grid_cycles=elapsed;
        feature_state.reset.grid_epoch=now;
        feature_state.reset.grid_expires=now+2*elapsed;
        feature_state.reset.grid_valid=1;
    } else feature_state.reset.grid_valid=0;
    feature_morph_grid.last=now;feature_morph_grid.known=1;
    if(!(phase%96)) {
        morph_bar_anchor(&feature_state.reset,now+2000UL);
        feature_arm_timer(reset_next(&feature_state.reset));
    }
    __restore_interrupts(irq);
}

/* Timer 2 can nest the stock Timer 1 control ISR. It deliberately does not
 * enable nested interrupts itself. The eCAP5 counter supplies a continuous
 * reference so ISR latency never accumulates into oscillator period drift.
 * Start early and wait to the deadline; count lateness instead of concealing it.
 */
#pragma CODE_SECTION(feature_reset_isr, ".feature_fast")
__interrupt void feature_reset_isr(void)
{
    uint16_t batch;
    REG16(TIMER2+4)=0x8010;
    /* A short reload can underflow again before a delayed ISR starts. Clear
     * any second CPU request while stopped, before installing a new alarm. */
    __asm(" AND IFR,#0xdfff");
    for (batch=0; batch<8; ++batch) {
        uint32_t next, now;
        uint16_t events;
        now=feature_now();
        next=reset_plan(&feature_state.reset,now,&events);
        if ((int32_t)(next-now)>(int32_t)feature_state.irq_lead) {
            ++feature_state.early_interrupts;
            feature_arm_timer(next);
            return;
        }
        if (!feature_wait_until(next)) goto clock_fault;
        REG32(GPDATA+2)=events>>4;
        REG32(GPDATA+4)=events&15;
        reset_commit(&feature_state.reset,events,feature_now());
    }
    feature_arm_timer(reset_next(&feature_state.reset));
    return;
clock_fault:
    REG32(GPDATA+2)=15;
    feature_state.fault=1;
    /* Timer is stopped at entry. Let the control tick restore normal division. */
}

static void feature_set_enabled(uint16_t enabled);
static uint16_t feature_setting(uint16_t id, uint16_t value, uint16_t persist);
static void feature_settings_open(void);
static void feature_settings_edit(uint16_t id,uint16_t value);
static uint16_t feature_settings_gesture(void);

/* The allocator observes stock list insertion/removal at the accepted-message
 * boundary. Node identity survives sorting and distinguishes equal pitches. */
uint16_t feature_note_added(const volatile uint16_t *node)
{
    uint16_t irq,slot,mask;
    if(!feature_state.allocation) {
        /* The accepted node is already linked; equal sentinel next/prev
         * pointers identify the first held note of a fresh articulation. */
        irq=__disable_interrupts();
        if(REG32(0xc040UL)==REG32(0xc042UL)) {
            feature_glide.snap_mask|=9;
            feature_glide.tied_mask&=~9U;
        } else feature_glide.tied_mask|=9;
        __restore_interrupts(irq);
        return 0; /* Preserve stock note selection and gate idempotence. */
    }
    irq=__disable_interrupts();
    feature_velocity_last=node[5];
    if(feature_harmony.enabled) {
        feature_harmony.root=(float)node[4];
        if(feature_state.midi_gate) feature_glide.tied_mask=63;
    }
    if(feature_state.reset.morph) {
        /* MIDI plays the two carriers together. SUB2 modulation rates keep
         * their neutral anchor; SUB1 derives timing from its parent. */
        feature_state.note[0]=feature_state.note[3]=(float)node[4];
        mask=9;
    } else {
        slot=voices_assign(&feature_state.voices,feature_state.note,(uint32_t)node,
                           node[4],node[5],node[8]);
        mask=1U<<slot;
    }
    if(!feature_state.midi_gate) {
        feature_glide.snap_mask=63;
        feature_glide.tied_mask=0;
    } else feature_glide.tied_mask|=mask;
    feature_state.published_mask&=~mask;
    feature_state.dirty=1;
    __restore_interrupts(irq);
    if(!feature_state.midi_gate) {
        feature_state.midi_gate=1;
        feature_gate_on();
    }
    return 1;
}

/* Caller excludes pitch publication. Reset only the MIDI anchor; panel,
 * sequence and continuous offsets still participate in the next pitch sum. */
static void feature_neutral_notes(uint16_t mask)
{
    uint16_t i;
    for(i=0;i<6;++i) if(mask&(1U<<i)) feature_state.note[i]=60.0f;
    feature_glide.snap_mask|=mask;
    feature_glide.tied_mask&=~mask;
    feature_glide.active_mask&=~mask;
    feature_state.published_mask&=~mask;
    feature_state.dirty=1;
}

void feature_note_deleted(const volatile uint16_t *node)
{
    uint16_t irq,mask;
    if(!feature_state.allocation) return;
    irq=__disable_interrupts();
    mask=voices_release(&feature_state.voices,(uint32_t)node);
    if(mask && !feature_harmony.enabled && !feature_state.reset.morph)
        feature_neutral_notes(mask);
    __restore_interrupts(irq);
}

uint16_t feature_note_removed(void)
{
    if(!feature_state.allocation) {
        if(REG32(0xc040UL)!=0xc040UL) feature_glide.tied_mask|=9;
        return 0;
    }
    if(REG32(0xc040UL)==0xc040UL && feature_state.midi_gate) {
        feature_state.midi_gate=0;
        feature_gate_off();
    }
    return 1;
}

/* Publication follows stock note selection, so mono priority and Note Off
 * fallback use the selected record rather than the newly inserted node.
 * An empty list retains the last velocity through the complete release. */
void feature_velocity_selected(void)
{
    if(!feature_state.allocation && REG32(0xc040UL)!=0xc040UL)
        feature_velocity_last=REG16(0xc051UL);
}

float feature_vca_velocity(float inverted)
{
    feature_velocity_selected();
    if(REG16(ENGINE+0xafc) || REG16(0xc4dcUL)) return inverted;
    return velocity_vca(inverted,feature_velocity_depth,feature_velocity_last);
}

void feature_notes_reset(void)
{
    uint16_t irq=__disable_interrupts();
    feature_glide.snap_mask=63;
    feature_glide.tied_mask=0;
    feature_glide.active_mask=0;
    if(!feature_state.allocation) { __restore_interrupts(irq);return; }
    voices_clear(&feature_state.voices);
    if(!feature_harmony.enabled && !feature_state.reset.morph) feature_neutral_notes(63);
    feature_state.midi_gate=0;
    __restore_interrupts(irq);
    feature_close_gates(); /* Explicit All Notes/Sound Off overrides Held. */
}

static uint16_t feature_midi_interval(uint16_t slot)
{
    uint16_t raw=REG16(midi_address[slot]);
    /* Sub CCs are stored shifted left by seven. CC64 is centered tuning,
     * matching the parent 14-bit controls' exact neutral value 8192. */
    return (slot==0 || slot==3) ? raw : pitch_midi_7bit_unit(raw>>7);
}

static void feature_set_pitch_controls(uint16_t enabled)
{
    uint16_t i,irq=__disable_interrupts();
    if(enabled) for(i=0;i<6;++i)
        pitch_control_seed(&feature_state.control[i],REG16(CONTROLS+panel_offset[i]),feature_midi_interval(i));
    feature_state.pitch_controls=enabled;
    feature_state.published_mask=0;
    feature_state.dirty=1;
    __restore_interrupts(irq);
}

static float feature_glide_value(uint16_t slot, float target)
{
    uint16_t bit=1U<<slot;
    float value;
    glide_target(&feature_glide.voice[slot],target,
                 !feature_glide.enabled || !(feature_glide.tied_mask&bit) ||
                 (feature_glide.snap_mask&bit));
    value=glide_advance(&feature_glide.voice[slot],feature_glide.alpha);
    if(feature_glide.voice[slot].remaining!=0.0f) feature_glide.active_mask|=bit;
    else {
        feature_glide.active_mask&=~bit;
        feature_glide.tied_mask&=~bit;
    }
    feature_glide.snap_mask&=~bit;
    return value;
}

/* Final calibrated stock-parent CV, after both MIDI pitch-selection paths.
 * Parent capture/division and its measured-frequency slope drive stay stock.
 * No held-note event arms a slide for running-transport latched transpose. */
float feature_mono_glide(uint16_t slot, float target)
{
    uint16_t bit=1U<<slot;
    if(feature_state.enabled && !feature_noise_only) return target;
    if(!feature_glide.enabled || REG16(ENGINE+0xafc) || REG16(0xc4dcUL)) {
        glide_target(&feature_glide.voice[slot],target,1);
        feature_glide.snap_mask&=~bit;
        feature_glide.tied_mask&=~bit;
        feature_glide.active_mask&=~bit;
        return target; /* Exact disabled/service bypass. */
    }
    return feature_glide_value(slot,target);
}

/* Property controls reuse the existing panel/MIDI ownership arbitration.
 * Sequence assignments offset the property under the matching knob. */
static uint16_t feature_harmony_bin(uint16_t slot,float position,uint16_t count)
{
    uint16_t bin=feature_harmony.parameter[slot];
    if(position<0) position=0;
    if(position>=(float)count) position=(float)count-.001f;
    if(!feature_harmony.bin_valid[slot] || position>=(float)bin+1.04f || position<(float)bin-.04f)
        bin=(uint16_t)position;
    feature_harmony.parameter[slot]=bin;feature_harmony.bin_valid[slot]=1;
    return bin;
}
static void feature_harmony_targets(float *notes,const float *seq,float transpose)
{
    static const uint16_t slots[4]={1,2,4,5};
    static const uint16_t counts[4]={14,12,4,4};
    float interval[6],root,melody;
    uint16_t i,changed=!feature_harmony.valid;
    int16_t degree;
    struct HarmonyChord chord;
    for(i=0;i<6;++i)
        interval[i]=pitch_control_interval(&feature_state.control[i],REG16(CONTROLS+panel_offset[i]),
                                           feature_midi_interval(i),feature_state.knob_span,Q_OFF);
    for(i=0;i<4;++i) {
        uint16_t slot=slots[i],old=feature_harmony.parameter[i];
        float position=(float)feature_state.control[slot].value*((float)counts[i]/16384.0f);
        if(REG16(CONTROLS+assign_offset[slot]))
            position+=seq[slot<3 ? 0 : 1]*(float)counts[i]/(float)(2*REG16(CONTROLS+0xd2a)+1);
        if(feature_harmony_bin(i,position,counts[i])!=old) changed=1;
    }
    root=feature_harmony.root+interval[0]+transpose;
    melody=feature_harmony.root+interval[3]+transpose;
    if(REG16(CONTROLS+assign_offset[0])) root+=seq[0];
    if(REG16(CONTROLS+assign_offset[3])) melody+=seq[1];
    degree=harmony_stable_degree(root,feature_harmony.parameter[0],feature_harmony.parameter[1],
                                 feature_harmony.degree,!changed);
    feature_harmony.melody_degree=harmony_stable_degree(melody,feature_harmony.parameter[0],feature_harmony.parameter[1],
                                                       feature_harmony.melody_degree,!changed);
    if(changed || degree!=feature_harmony.degree) {
        harmony_build(&chord,degree,feature_harmony.parameter[0],feature_harmony.parameter[1],
                      feature_harmony.parameter[2],feature_harmony.parameter[3]);
        for(i=0;i<4;++i) feature_harmony.chord.note[i]=chord.note[i];
        feature_harmony.chord.root=chord.root;feature_harmony.chord.quality=chord.quality;
        feature_harmony.degree=degree;feature_harmony.valid=1;
    }
    /* Four suboscillators retain root/third/fifth/seventh identity. Main VCO1
     * supplies bass; main VCO2 is an independently sequenced scale melody. */
    notes[0]=(float)feature_harmony.chord.root-12.0f;
    notes[1]=(float)feature_harmony.chord.note[0];notes[2]=(float)feature_harmony.chord.note[1];
    notes[4]=(float)feature_harmony.chord.note[2];notes[5]=(float)feature_harmony.chord.note[3];
    notes[3]=(float)harmony_root_note(feature_harmony.melody_degree,feature_harmony.parameter[0],feature_harmony.parameter[1]);
    for(i=0;i<6;++i) {
        while(notes[i]<0) notes[i]+=12;
        while(notes[i]>127) notes[i]-=12;
    }
}

/* Fixed slope sized for the longest noise interval. Analog amplitude and
 * spectrum still require qualification; no pitched modulation is applied. */
static void feature_noise_output(void)
{
    if(!(feature_state.published_mask&32)) {
        float gain=*(volatile float *)(ENGINE+gain_offset[3]);
        float offset=*(volatile float *)(ENGINE+gain_offset[3]+2);
        feature_output((volatile uint16_t *)(ENGINE+binding[5]),
                       gain*7.881490f+offset); /* ln(100 MHz / 37767). */
        feature_state.published_mask|=32;
    }
}

/* Compute the shape outside the reset ISR. Publish a bounded eight-interval table
 * under the same interrupt exclusion used for mode changes. A moving control
 * can change the current pattern; settled patterns always sum to eight periods. */
#pragma FUNC_CANNOT_INLINE(feature_morph_output)
static void feature_morph_output(uint16_t voice, uint16_t position, uint32_t margin, uint32_t period, float gain, float offset)
{
    uint16_t pair=voice==4, k, irq, slot=0;
    uint32_t shape[8], longest;
    if(period<MORPH_MIN_PERIOD) period=MORPH_MIN_PERIOD;
    longest=morph_shape(shape,period,position);
    for(k=1;k<8;++k) if(shape[k]>shape[slot]) slot=k;
    /* Allow the full 7/4 timing expansion without extra ramp growth. */
    longest+=longest-(longest>>2)+margin;
    feature_morph_position[pair]=position;feature_morph_grid.margin[pair]=margin;
    irq=__disable_interrupts();
    feature_state.reset.period[pair*2]=period;
    feature_state.reset.grid_slot[pair]=slot;
    for(k=0;k<8;++k) feature_state.reset.shape[pair][k]=shape[k];
    __restore_interrupts(irq);
    feature_output((volatile uint16_t *)(ENGINE+binding[voice]),
                   gain*natural_log(CLOCK_HZ/(float)longest)+offset);
}

#pragma FUNC_CANNOT_INLINE(feature_morph_tick)
static void feature_morph_tick(uint16_t voice, uint16_t q)
{
    uint16_t parent=voice-1, bit=1U<<voice, sub=sub_index[voice];
    float base=feature_state.published_base[parent];
    float detune=feature_state.published_detune[parent]-12.0f;
    struct PitchTarget target;
    float hz, log_hz;
    uint16_t position;
    uint32_t margin=0;
    if(REG16(ENGINE+0xae2) && feature_state.reset.grid_valid &&
       (int32_t)(feature_state.reset.grid_expires-feature_now())>0)
        margin=feature_state.reset.grid_cycles>>2;
    else feature_state.reset.grid_valid=0;
    (void)pitch_control_interval(&feature_state.control[voice],
        REG16(CONTROLS+panel_offset[voice]),feature_midi_interval(voice),feature_state.knob_span,Q_OFF);
    {
        float shaped=(float)feature_state.control[voice].value;
        if(REG16(CONTROLS+assign_offset[voice])) {
            uint16_t bank=voice==4, step=REG16(ENGINE+0xaf4+bank);
            /* Timbre stays continuous regardless of pitch quantization or
             * sequence octave range. Noon is neutral; ends span half a bank. */
            shaped+=(*(volatile float *)(CONTROLS+0x4c0+8*bank+2*step)-0.5f)*16383.0f;
        }
        if(shaped<0.0f) shaped=0.0f;
        if(shaped>16383.0f) shaped=16383.0f;
        position=(uint16_t)shaped;
    }
    if((feature_state.published_mask&bit) &&
       feature_state.published_base[voice]==base && feature_state.published_detune[voice]==detune &&
       feature_morph_position[voice==4]==position && feature_morph_grid.margin[voice==4]==margin &&
       !((feature_glide.active_mask|feature_glide.snap_mask)&bit)) return;
    feature_state.published_base[voice]=base;feature_state.published_detune[voice]=detune;
    feature_state.published_mask|=bit;
    pitch_target(&target,base,detune,q);
    hz=pitch_frequency(&target,q,(const float *)0x094246UL);
    log_hz=feature_glide_value(voice,natural_log(hz));
    if(feature_glide.voice[voice].remaining!=0.0f) hz=frequency_from_log(log_hz);
    feature_morph_output(voice,position,margin,(uint32_t)(CLOCK_HZ/hz+0.5f),
        *(volatile float *)(ENGINE+gain_offset[sub]),
        *(volatile float *)(ENGINE+gain_offset[sub]+2));
}

void feature_tick(void)
{
    uint16_t i,q,changed;
    float continuous=0.0f,transpose=0.0f,seq[2],harmony[6];
    if (!feature_state.enabled) return;
    if (feature_state.fault) return; /* Foreground owns held-list cleanup. */
    if(feature_noise_only) { feature_noise_output();return; }
    q=REG16(ENGINE+0x0ae0);
    changed=feature_state.quantize!=q;
    if (!feature_state.dirty && !changed && !feature_state.pitch_controls &&
        !feature_glide.active_mask && !feature_glide.snap_mask) return;
    if(changed) feature_state.published_mask=0;
    feature_state.quantize=q;
    feature_state.dirty=0;
    feature_state.clamped_mask=0;
    seq[0]=0;seq[1]=0;
    if(feature_state.pitch_controls) {
        uint16_t note=REG16(0xc2b8UL);
        transpose=note ? (float)note-60.0f : 0.0f;
        continuous=*(volatile float *)(ENGINE+0xab6)+
                   (*(volatile float *)(ENGINE+0xad6)-0.5f)*
                   *(volatile float *)(ENGINE+0xad8);
        for(i=0;i<2;++i) {
            uint16_t step=REG16(ENGINE+0xaf4+i);
            seq[i]=pitch_sequence_interval(*(volatile float *)(CONTROLS+0x4c0+8*i+2*step),
                                           REG16(CONTROLS+0xd2a),feature_harmony.enabled ? Q_OFF : q);
        }
    }
    if(feature_harmony.enabled) feature_harmony_targets(harmony,seq,transpose);
    for(i=0;i<6;++i) {
        float base=feature_state.note[i],detune=continuous;
        uint16_t bit=1U<<i,same;
        if(i==5 && feature_state.reset.noise) { feature_noise_output();continue; }
        if(feature_state.reset.morph && (bit&18)) {
            feature_morph_tick(i,q);continue;
        }
        if(feature_harmony.enabled) {
            /* Quantize harmony once. C is invariant under the stock white-note
             * mapper; carry the selected chord pitch as its continuous term.
             * This preserves the panel ET/JI family without re-quantizing chords. */
            base=60.0f;detune+=harmony[i]-60.0f;
            detune+=(float)((int16_t)(2*i)-5)*(0.06f/127.0f)*(float)feature_harmony.spread;
        } else if(feature_state.pitch_controls) {
            base+=pitch_control_interval(&feature_state.control[i],REG16(CONTROLS+panel_offset[i]),
                                         feature_midi_interval(i),feature_state.knob_span,q)+transpose;
            if(REG16(CONTROLS+assign_offset[i])) base+=seq[i<3 ? 0 : 1];
        }
        {
            struct PitchTarget target;
            pitch_target(&target,base,detune,q);
            base=target.base; detune=target.detune;
            if(target.clamped) feature_state.clamped_mask|=bit;
        }
        same=(feature_state.published_mask&bit) && base==feature_state.published_base[i] &&
             detune==feature_state.published_detune[i];
        if(same && !(feature_glide.active_mask&bit) && !(feature_glide.snap_mask&bit)) continue;
        feature_state.published_base[i]=base;
        feature_state.published_detune[i]=detune;
        feature_state.published_mask|=bit;
        if(i==0 || i==3) {
            float cv=same ? feature_glide.voice[i].target : feature_parent_cv(i==3,base,detune);
            feature_output((volatile uint16_t *)(ENGINE+binding[i]),feature_glide_value(i,cv));
        } else {
            uint16_t sub=sub_index[i];
            struct PitchTarget target;
            float hz,log_hz;
            target.base=base; target.detune=detune;
            hz=pitch_frequency(&target,q,(const float *)0x094246UL);
            log_hz=feature_glide_value(i,natural_log(hz));
            if(feature_glide.voice[i].remaining!=0.0f) hz=frequency_from_log(log_hz);
            float gain=*(volatile float *)(ENGINE+gain_offset[sub]);
            float offset=*(volatile float *)(ENGINE+gain_offset[sub]+2);
            feature_state.reset.period[sub]=(uint32_t)(CLOCK_HZ/hz+0.5f);
            feature_output((volatile uint16_t *)(ENGINE+binding[i]),gain*log_hz+offset);
        }
    }
    feature_state.irq_lead=reset_irq_lead(&feature_state.reset);
}

/* Mode cleanup closes the gates and clears MIDI ownership. Preserve the
 * user's EG Held selection by requesting a fresh stock gate-on afterward;
 * otherwise its LED keeps blinking while both envelopes remain closed.
 * Explicit All Notes/Sound Off does not call this mode-transition helper. */
static void feature_restore_panel_held(void)
{
    uint16_t irq=__disable_interrupts();
    if(REG16(CONTROLS+0xd27) && !REG16(ENGINE+0xafc) && !REG16(0xc4dcUL)) {
        REG16(ENGINE+0xadf)=0;
        REG16(ENGINE+0xade)=1;
    }
    __restore_interrupts(irq);
}

static void feature_set_enabled(uint16_t enabled)
{
    uint16_t i;
    uint16_t irq;
    if(!enabled) { feature_harmony.enabled=0;feature_state.reset.morph=0;feature_morph_clock_reset(); }
    if(!enabled && feature_state.allocation) {
        feature_clear_notes();
        feature_state.allocation=0;
    }
    irq=__disable_interrupts();
    if (enabled==feature_state.enabled) { __restore_interrupts(irq); return; }
    if (!enabled) {
        feature_disable_timer_irq(); REG16(TIMER2+4)=0x8010;
        REG16(0xc0c8UL)&=~0x2000U;
        REG16(0xc0c9UL)&=~0x2000U;
        REG32(GPDATA+2)=15;
        __asm(" EALLOW");
        REG32(GPIO+6)=(REG32(GPIO+6)&~255UL)|feature_state.saved_mux;
        REG32(GPIO+0x20)=(REG32(GPIO+0x20)&~255UL)|feature_state.saved_gmux;
        REG32(GPIO+0xa)=(REG32(GPIO+0xa)&~15UL)|feature_state.saved_dir;
        __asm(" EDIS");
        feature_state.enabled=0;feature_clock.valid=0;
        feature_glide.snap_mask=63; feature_glide.tied_mask=0; feature_glide.active_mask=0;
        feature_state.reset.low_mask=0;
        for(i=0;i<2;++i) {
            uint32_t subdivision=REG32(0xf820UL+2UL*i);
            REG16(subdivision+0x15)=0; REG16(subdivision+0x16)=0;
        }
        for (i=0;i<6;++i) REG16(ENGINE+binding[i]+8)=0;
        __restore_interrupts(irq);
        feature_stock_refresh();
        feature_restore_panel_held();
        return;
    }
    /* Reject diagnostic ownership conflicts; no remux under its pulse train. */
    if (REG16(0xf815UL)!=0 || REG16(0xf812UL)!=0 || REG16(ENGINE+0xafc)!=0) { __restore_interrupts(irq); return; }
    /* ECCTL1/2 are EALLOW-protected. Verify that the timestamp counter moves
     * before taking ownership of any reset pin or enabling Timer 2. */
    __asm(" EALLOW");
    REG32(0x5d328UL)|=0x10;
    REG16(ECAP5+0x14)=0;
    REG16(ECAP5+0x15)=0;
    REG16(ECAP5+0x16)=0;
    REG32(ECAP5)=0;
    for (i=0;i<4;++i) feature_adc_scan[i]=0;
    REG16(ECAP5+0x15)=0x10;
    __asm(" EDIS");
    if (!feature_wait_until(feature_now()+100UL)) {
        feature_state.fault=1;
        __restore_interrupts(irq);
        return;
    }
    feature_state.fault=0;
    feature_state.saved_mux=REG32(GPIO+6)&255UL;
    feature_state.saved_gmux=REG32(GPIO+0x20)&255UL;
    feature_state.saved_dir=REG32(GPIO+0xa)&15UL;
    REG32(GPDATA+2)=15;
    __asm(" EALLOW");
    REG32(GPIO+0xa)|=feature_noise_only ? 8UL : 15UL;
    REG32(GPIO+6)&=~(feature_noise_only ? 192UL : 255UL);
    REG32(GPIO+0x20)&=~(feature_noise_only ? 192UL : 255UL);
    REG32(0x0d1cUL)=(uint32_t)&feature_reset_isr;
    __asm(" EDIS");
    REG16(TIMER2+6)=0; REG16(TIMER2+7)=0;
    REG16(TIMER2+4)=0x8010;
    __asm(" AND IFR,#0xdfff");
    feature_clock.valid=0;
    feature_state.reset.disabled_mask=feature_noise_only ? 7 : 0;
    feature_state.reset.random=0x6d2b79f5UL;
    feature_state.reset.period[3]=NOISE_MIN_CYCLES;
    feature_state.enabled=1; feature_state.dirty=1; feature_state.published_mask=0;
    feature_glide.snap_mask=63; feature_glide.active_mask=0; feature_glide.tied_mask=0;
    feature_tick();
    feature_state.irq_lead=reset_irq_lead(&feature_state.reset);
    feature_state.reset.low_mask=0;
    for(i=0;i<4;++i) feature_state.reset.fall[i]=feature_now()+100000UL+(uint32_t)i*5000UL;
    REG16(0xc0c8UL)|=0x2000U;
    REG16(0xc0c9UL)|=0x2000U;
    feature_enable_timer_irq(); feature_arm_timer(reset_next(&feature_state.reset));
    __restore_interrupts(irq);
    feature_restore_panel_held();
}

static void feature_set_allocation(uint16_t enabled)
{
    if(enabled==feature_state.allocation) return;
    feature_clear_notes();
    feature_close_gates();
    if(enabled) feature_set_enabled(1);
    if(enabled && !feature_state.enabled) return;
    voices_clear(&feature_state.voices);
    feature_state.midi_gate=0;
    if(enabled) feature_set_pitch_controls(1);
    feature_state.allocation=enabled;
    feature_restore_panel_held();
}

static uint16_t feature_panel_idle(void)
{
    /* The mode transaction already closes both gates and clears held notes.
     * A latched gate or panel Held mode must not silently block the chord. */
    return !REG16(ENGINE+0xae2) && !REG16(ENGINE+0xafc) && !REG16(0xc4dcUL);
}

/* Observe the stock edge before its immediate assignment toggle. Ordinary
 * presses are returned unchanged, including when every feature is disabled. */
uint16_t feature_assignment_edge(uint16_t which, uint16_t edge)
{
    if(edge && feature_panel_idle()) {
        if(which==0 || which==2) {
            uint16_t index=which==0;
            feature_settings.saved[index]=REG16(CONTROLS+(index ? 0xd2e : 0xd2d));
            feature_settings.captured|=1U<<index;
        }
    }
    return edge;
}

static void feature_restore_assignment_leds(void)
{
    static const uint16_t assignments[6]={0xd2b,0xd2d,0xd2e,0xd2c,0xd2f,0xd30};
    uint16_t i;
    for(i=0;i<6;++i)
        feature_setting_led((volatile uint16_t *)(CONTROLS+0xaca+0x22*i),
                            REG16(CONTROLS+assignments[i]));
}

/* Runs before foreground dequeue, when no MIDI payload is in flight. Heap
 * cleanup and mode changes never run from the timer interrupt. */
void feature_service(void)
{
    if(feature_state.config_pending) {
        uint16_t i;
        feature_state.config_pending=0;
        feature_set_enabled(0);
        feature_noise_only=0;feature_state.reset.noise=0;
        feature_set_pitch_controls(0);
        voices_clear(&feature_state.voices);
        for(i=0;i<6;++i) feature_state.note[i]=60.0f;
        feature_setting(4,feature_state.persisted.span,0);
        feature_setting(6,feature_state.persisted.glide_time,0);
        feature_setting(5,feature_state.persisted.glide,0);
        feature_velocity_last=127;
        feature_setting(7,feature_state.persisted.velocity_depth,0);
        feature_setting(3,feature_state.persisted.spread,0);
        feature_setting(9,feature_state.persisted.rhythm,0);
        feature_setting(8,feature_state.persisted.mode,0);
        feature_setting(10,feature_state.persisted.noise,0);
    }
    if(feature_state.fault && feature_state.enabled) feature_set_enabled(0);
    feature_settings_gesture();
}

/* Volatile bench access to the actual shared panel quantizer state, including
 * its snapshot and LED policy. This is not a separate per-feature quantizer. */
static void feature_set_quantize(uint16_t q)
{
    if(REG16(ENGINE+0xafc) || REG16(0xc4dcUL)) return;
    REG16(ENGINE+0xae0)=q;
    REG16(CONTROLS+0xd26)=q!=Q_OFF;
    REG16(0xc3dcUL)=q;
    /* The normal panel service renders the shared mode on every pass. */
    feature_state.dirty=1;
}

/* Version 9 occupies 0x55..0x5e of the 128-word control image. Only this
 * format is accepted; loading never writes the image. */
#define FEATURE_CONFIG_STAGED (0xc300UL+0xb8UL+0x55UL)
void feature_config_load(void)
{
    feature_state.config_invalid=!feature_config_decode((const volatile uint16_t *)FEATURE_CONFIG_STAGED,
                                                       &feature_state.persisted);
    feature_state.config_pending=1;
}

static uint16_t feature_setting(uint16_t id, uint16_t value, uint16_t persist)
{
    uint16_t i;
    if(!feature_config_value_valid(id,value)) return 0;
    if(id==3) {
        feature_harmony.spread=value;feature_state.dirty=1;
    } else if(id==4) {
        feature_state.knob_span=value==1 ? 12 : 24;
        for(i=0;i<6;++i) feature_state.control[i].bin_valid=0;
        feature_state.dirty=1;
    } else if(id==5) {
        feature_glide.enabled=value;
        feature_glide.snap_mask=63;
        feature_glide.tied_mask=0;
        feature_state.dirty=1;
    } else if(id==8) {
        uint16_t previous=feature_state.reset.morph ? 3 : (feature_harmony.enabled ? 2 : feature_state.allocation);
        if(value!=previous || (value!=0 || feature_state.reset.noise)!=feature_state.enabled) {
            feature_set_allocation(0);
            feature_set_enabled(0);
            feature_noise_only=!value && feature_state.reset.noise;
            feature_harmony.enabled=value==2;feature_harmony.valid=0;
            feature_state.reset.morph=value==3 ? 5 : 0;
            if(value==1) feature_neutral_notes(63); /* Engine is stopped here. */
            if(value==3) feature_state.note[2]=feature_state.note[5]=60.0f;
            if(value) feature_set_allocation(1);
            else if(feature_noise_only) feature_set_enabled(1);
            if(value && !feature_state.allocation) { feature_harmony.enabled=0;feature_state.reset.morph=0;return 0; }
            if(feature_noise_only && !feature_state.enabled) return 0;
            for(i=0;i<4;++i) feature_harmony.bin_valid[i]=0;
            feature_state.published_mask=0;feature_state.dirty=1;feature_glide.snap_mask=63;
        }
    } else if(id==10) {
        uint16_t irq;
        if(value!=feature_state.reset.noise || (value && !feature_state.enabled)) {
            if(!feature_state.allocation) {
                feature_set_enabled(0);
                feature_noise_only=value;
                feature_state.reset.noise=value;
                if(value) feature_set_enabled(1);
                if(value && !feature_state.enabled) {
                    feature_noise_only=0;feature_state.reset.noise=0;return 0;
                }
            } else {
                irq=__disable_interrupts();
                feature_state.reset.noise=value;
                feature_state.published_mask&=~32U;
                feature_glide.snap_mask|=32;
                feature_state.dirty=1;
                feature_tick();
                feature_state.irq_lead=reset_irq_lead(&feature_state.reset);
                __restore_interrupts(irq);
            }
        }
    } else if(id==9) {
        feature_rhythm_type=value;
    } else if(id==7) {
        feature_velocity_depth=value;
    } else {
        feature_glide.time=value;
        feature_glide.alpha=glide_coefficient(glide_time_us(value));
    }
    if(persist) {
        if(id==3) feature_state.persisted.spread=value;
        else if(id==4) feature_state.persisted.span=value;
        else if(id==5) feature_state.persisted.glide=value;
        else if(id==6) feature_state.persisted.glide_time=value;
        else if(id==7) feature_state.persisted.velocity_depth=value;
        else if(id==8) feature_state.persisted.mode=value;
        else if(id==9) feature_state.persisted.rhythm=value;
        else if(id==10) feature_state.persisted.noise=value;
        feature_config_encode((volatile uint16_t *)FEATURE_CONFIG_STAGED,&feature_state.persisted);
        feature_save_config();
    }
    return 1;
}

void feature_persistent_sysex(const volatile uint16_t *parser)
{
    uint16_t value;
    /* Unit addressing was checked by the stock command-0x23 dispatcher. */
    if(parser[0x1a]!=16) return;
    value=(parser[0x56]<<7)|parser[0x57];
    feature_setting(parser[0x55],value,1);
}

void feature_cc(uint16_t controller, uint16_t value)
{
    /* These channel messages have passed the stock source/channel filters. */
    if(controller>=80 && controller<=83) feature_rhythm_cc(controller-80,value);
    else if(controller==112) feature_setting(3,value,0);
    else if(controller==116) feature_setting(4,value==0 ? 0 : (value<64 ? 1 : 2),0);
    else if(controller==114) feature_setting(8,value,0);
    else if(controller==117) feature_setting(10,value>=64,0);
    else if(controller==115) feature_setting(9,value>=64,0);
    else if(controller==111) feature_setting(7,value,0);
    else if(controller==65) feature_setting(5,value>=64,0);
    else if(controller==5) feature_setting(6,(value<<7)|(feature_glide.time&127),0);
    else if(controller==37) feature_setting(6,(feature_glide.time&0x3f80)|value,0);
}

/* Command 0x35 remains a volatile development control surface. IDs 16..21
 * set six source notes as U14 semitones/128, constrained to MIDI 0..127.
 * ID 7 enables the feature engine; ID 8 selects Normal / Paraphonic / Harmony / Morph.
 * IDs 1..6 retain the diagnostic interface.
 */
uint16_t feature_sysex(const volatile uint16_t *parser)
{
    uint16_t id,value,unit;
    if(parser[0x1a]!=16) return 1;
    unit=parser[0x60];
    if(unit!=127 && unit!=REG16(0xc3bcUL)) return 1;
    id=parser[0x55]; value=(parser[0x56]<<7)|parser[0x57];
    if(id==7) { if(value<=1) feature_set_enabled(value); return 1; }
    if(id==8) { feature_setting(8,value,0); return 1; }
    if(id==9) { if(value<=1) feature_set_pitch_controls(value); return 1; }
    if(id==10) {
        if(value==12 || value==24) {
            uint16_t i;
            feature_state.knob_span=value;
            for(i=0;i<6;++i) feature_state.control[i].bin_valid=0;
            feature_state.dirty=1;
        }
        return 1;
    }
    if(id==11) { if(value<=5 && value!=2) feature_set_quantize(value); return 1; }
    if(id==12) { feature_setting(5,value,0); return 1; }
    if(id==13) { feature_setting(6,value,0); return 1; }
    if(id==14) { feature_setting(10,value,0); return 1; }
    if(id>=16 && id<=21) {
        if(value<=127*128) {
            feature_state.note[id-16]=(float)value*(1.0f/128.0f);
            feature_state.dirty=1;
        }
        return 1;
    }
    return feature_state.enabled ? 1 : 0;
}

void feature_init(void)
{
    uint16_t i;
    volatile uint16_t *words=(volatile uint16_t *)&feature_state;
    const uint16_t *load=&feature_fast_load;
    uint16_t *run=&feature_fast_run;
    for(i=0;i<(uint32_t)&feature_fast_size;++i) run[i]=load[i];
    for(i=0;i<sizeof(feature_state);++i) words[i]=0;
    for(i=0;i<4;++i) feature_adc_scan[i]=0;
    words=(volatile uint16_t *)&feature_glide;
    for(i=0;i<sizeof(feature_glide);++i) words[i]=0;
    words=(volatile uint16_t *)&feature_settings;
    for(i=0;i<sizeof(feature_settings);++i) words[i]=0;
    words=(volatile uint16_t *)&feature_harmony;
    for(i=0;i<sizeof(feature_harmony);++i) words[i]=0;
    feature_noise_only=0;
    feature_harmony.root=60.0f;
    feature_rhythm_init();
    feature_velocity_depth=0; feature_velocity_last=127;
    feature_glide.alpha=glide_coefficient(GLIDE_DEFAULT_US);
    feature_glide.snap_mask=63;
    for(i=0;i<6;++i) feature_state.note[i]=60.0f;
    feature_state.knob_span=24;
}

/* Settings owns panel buttons only while stopped. Continuous ADC and envelope
 * service keep running. Exit drains held keys so no gesture leaks to stock. */
static const uint16_t settings_buttons[20]={
    0x602,0x5e0,0x7de,0x7bc,0x79a,0x778, /* six page selectors */
    0x646,0x558,0x57a,0x624,0x5be,0x756, /* EG, NEXT, RESET, OCT, PLAY, QUANTIZE */
    0x536,0x514,0x4f2,0x4d0,0x6ce,0x6ac,0x68a,0x668
};
static const uint16_t settings_ids[7]={3,5,7,8,9,10,4};
static uint16_t feature_settings_gesture(void)
{
    uint16_t i,down=0,other=0;
    if(feature_settings.active) return 1;
    if(!feature_settings.captured && !feature_settings.chord.latched) return 0;
    for(i=0;i<20;++i) {
        volatile uint16_t *button=(volatile uint16_t *)(CONTROLS+settings_buttons[i]);
        if(button[6]!=button[5]) continue;
        if(i==1) down|=1;
        else if(i==2) down|=2;
        else other=1;
    }
    feature_settings.captured&=down;
    if(panel_chord_tick(&feature_settings.chord,feature_micros(),down,
                        feature_panel_idle() && !other,feature_settings.captured)) {
        REG16(CONTROLS+0xd2d)=feature_settings.saved[0];
        REG16(CONTROLS+0xd2e)=feature_settings.saved[1];
        REG16(0xc3d6UL)=feature_settings.saved[0];
        REG16(0xc3d7UL)=feature_settings.saved[1];
        feature_settings_open();
        return 1;
    }
    return 0;
}

static uint16_t feature_setting_value(uint16_t id)
{
    if(id==3) return feature_harmony.spread;
    if(id==4) return feature_state.knob_span==12 ? 1 : 2;
    if(id==5) return feature_glide.enabled;
    if(id==6) return feature_glide.time;
    if(id==7) return feature_velocity_depth;
    if(id==8) return feature_state.reset.morph ? 3 : (feature_harmony.enabled ? 2 : feature_state.allocation);
    if(id==9) return feature_rhythm_type;
    if(id==10) return feature_state.reset.noise;
    return 0;
}
static void feature_settings_open(void)
{
    uint16_t i;
    feature_settings.active=1; feature_settings.draining=2;
    feature_settings.page=0; feature_settings.fine=0; feature_settings.dirty=0;
    feature_settings.last_activity=feature_micros();
    for(i=0;i<7;++i) {
        uint16_t value=feature_setting_value(settings_ids[i]);
        feature_settings.recall[i]=value ? value : (i==2 ? 64 : (i==6 ? 2 : 1));
    }
}
static void feature_settings_edit(uint16_t id, uint16_t value)
{
    if(!feature_setting(id,value,0)) return;
    feature_settings.edited[id-3]=value;
    feature_settings.dirty|=1U<<(id-3);
}
static void feature_settings_close(void)
{
    feature_config_apply_edits(&feature_state.persisted,feature_settings.dirty,
                               feature_settings.edited);
    if(feature_settings.dirty) {
        feature_config_encode((volatile uint16_t *)FEATURE_CONFIG_STAGED,&feature_state.persisted);
        feature_save_config();
    }
    feature_settings.dirty=0; feature_settings.draining=1;
}
uint16_t feature_settings_gate(void)
{
    uint16_t i,id,value;
    uint32_t down=0,edge=0,now;
    if(!feature_settings.active) return REG16(0xf686UL); /* Original panel guard. */
    now=feature_micros();
    for(i=0;i<20;++i) {
        volatile uint16_t *button=(volatile uint16_t *)(CONTROLS+settings_buttons[i]);
        /* The stock poll updates the debounced cache as well as its latch. */
        if(feature_setting_button_edge(button)) edge|=1UL<<i;
        if(button[6]==button[5]) down|=1UL<<i;
        *(volatile uint32_t *)(button+0x16)=0;
        button[0x18]=0; /* Stock hold timers must re-arm after leaving settings. */
    }
    if(feature_settings.draining==2) { /* Entry: require all other keys released. */
        if(!down) feature_settings.draining=0;
        edge=0;
    }
    if(!feature_settings.draining && (!feature_panel_idle() || (uint32_t)(now-feature_settings.last_activity)>=30000000UL))
        feature_settings_close();
    if(feature_settings.draining==1) {
        if(!down) {
            feature_settings.active=0; feature_settings.draining=0;
            feature_settings.captured=0; feature_settings.chord.timing=0; feature_settings.chord.latched=0;
            feature_restore_assignment_leds();
            for(i=0;i<8;++i)
                feature_setting_led((volatile uint16_t *)(CONTROLS+0x800+0x22*i),
                                    (i&3)==REG16(ENGINE+0xaf4+(i>>2)));
            return REG16(0xf686UL); /* Stock redraws indicators on this pass. */
        }
    } else if(edge) {
        feature_settings.last_activity=now;
        if(edge&(1UL<<10)) feature_settings_close(); /* PLAY saves/exits, never starts. */
        else {
            for(i=0;i<6;++i) if(edge&(1UL<<i)) feature_settings.page=i;
            if(edge&(1UL<<11)) feature_settings.page=6;
            if(edge&(1UL<<9)) feature_settings.fine=!feature_settings.fine;
            id=settings_ids[feature_settings.page]; value=feature_setting_value(id);
            if(edge&(1UL<<6)) {
                if(feature_settings.page==3) value=value==3 ? 0 : value+1;
                else if(feature_settings.page==6) value=value==1 ? 2 : 1;
                else if(value) { feature_settings.recall[feature_settings.page]=value; value=0; }
                else value=feature_settings.recall[feature_settings.page];
                feature_settings_edit(id,value);
            }
            if(edge&((1UL<<7)|(1UL<<8))) {
                uint16_t step=feature_settings.fine ? 1 : 16,maximum=127;
                if(feature_settings.page==1) id=6;
                else if(feature_settings.page==3) { step=1;maximum=3; }
                else if(feature_settings.page==4 || feature_settings.page==5) { step=1;maximum=1; }
                else if(feature_settings.page==6) { step=1;maximum=2; }
                if(id==6) value=glide_panel_adjust(feature_glide.time,feature_settings.fine,(edge&(1UL<<7))!=0);
                else value=panel_adjust(feature_setting_value(id),step,maximum,(edge&(1UL<<7))!=0);
                if(id==4 && !value) value=1; /* Panel span has two choices, no default alias. */
                feature_settings_edit(id,value);
            }
        }
    }
    for(i=0;i<6;++i) {
        uint16_t on=feature_setting_value(settings_ids[i])!=0;
        if(i==feature_settings.page) on=(now%500000UL)<250000UL;
        feature_setting_led((volatile uint16_t *)(CONTROLS+0xaca+0x22*i),on);
    }
    for(i=0;i<4;++i)
        feature_setting_led((volatile uint16_t *)(CONTROLS+0xb96+0x22*i),
                            feature_settings.page==6 && (now%500000UL)<250000UL);
    id=feature_settings.page==1 ? 6 : settings_ids[feature_settings.page];
    value=feature_setting_value(id);
    if(id==8) value=value ? 1+32*(value-1) : 0;
    else if(id==9 || id==10) value*=127;
    else if(id==4) value=value==1 ? 32 : 127;
    for(i=0;i<8;++i)
        feature_setting_led((volatile uint16_t *)(CONTROLS+0x800+0x22*i),
                            id==6 ? i<glide_panel_level(value) : (i<4 && value>=(i*32+1)));
    feature_setting_led((volatile uint16_t *)(CONTROLS+0xa42),feature_setting_value(settings_ids[feature_settings.page])!=0);
    feature_setting_led((volatile uint16_t *)(CONTROLS+0xa64),feature_settings.fine);
    feature_setting_led((volatile uint16_t *)(CONTROLS+0xa86),!feature_settings.fine);
    feature_setting_led((volatile uint16_t *)(CONTROLS+0xaa8),0);
    feature_refresh_panel_references();
    return 1; /* Original epilogue; do not run musical button handlers. */
}
