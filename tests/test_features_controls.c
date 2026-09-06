#include <assert.h>
#include <math.h>
#include "features_controls.h"
int main(void)
{
    struct PitchControl c;
    unsigned raw;
    float previous=-25;
    pitch_control_seed(&c,0,8192);
    assert(pitch_control_interval(&c,0,8192,24,0)==-24);
    assert(pitch_control_interval(&c,16383,8192,24,0)==24);
    assert(pitch_control_interval(&c,16383,8193,24,0)==0);
    assert(pitch_control_interval(&c,16376,8193,24,0)==0); /* Noise does not seize MIDI ownership. */
    assert(pitch_control_interval(&c,0,8193,24,0)==-24);
    assert(pitch_control_interval(&c,8192,16383,24,0)==24); /* Simultaneous movement resolves to MIDI. */
    pitch_control_seed(&c,8192,8192);
    assert(pitch_control_interval(&c,8475,8192,24,0)==0);
    assert(pitch_control_interval(&c,8500,8192,24,0)==0); /* Inside Schmitt boundary. */
    assert(pitch_control_interval(&c,8520,8192,24,0)==1);
    assert(pitch_control_interval(&c,8475,8192,24,0)==1);
    assert(pitch_control_interval(&c,8440,8192,24,0)==0);
    assert(pitch_control_interval(&c,8192,8192,24,5)==0);
    assert(pitch_control_interval(&c,0,8192,12,5)==-12);
    assert(pitch_control_interval(&c,16383,8192,12,5)==12);
    for(raw=0;raw<=16383;++raw) {
        float value=pitch_centered_interval(raw,24,128);
        assert(value>=-24 && value<=24 && value>=previous);
        previous=value;
    }
    for(raw=8064;raw<=8320;++raw)
        assert(pitch_centered_interval(raw,24,128)==0);
    assert(pitch_centered_interval(8063,24,128)<0);
    assert(pitch_centered_interval(8321,24,128)>0);
    assert(pitch_centered_interval(8193,24,0)>0); /* MIDI has no panel deadband. */
    assert(pitch_midi_7bit_unit(0)==0);
    assert(pitch_midi_7bit_unit(64)==8192);
    assert(pitch_midi_7bit_unit(127)==16383);
    for(raw=1;raw<=127;++raw)
        assert(pitch_midi_7bit_unit(raw)>pitch_midi_7bit_unit(raw-1));
    assert(pitch_sequence_interval(0,12,0)==-12);
    assert(pitch_sequence_interval(.5f,12,0)==0);
    assert(pitch_sequence_interval(.5f,12,5)==.5f);
    assert(pitch_sequence_interval(.1f,12,0)==-9); /* Stock truncates signed offset toward zero. */
    return 0;
}
