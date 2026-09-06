#include <assert.h>
#include "features_panel.h"
int main(void)
{
    struct PanelChord c={0};
    assert(!panel_chord_tick(&c,0,1,1,1));
    assert(!panel_chord_tick(&c,100,3,1,3));
    assert(!panel_chord_tick(&c,1000099,3,1,3));
    assert(panel_chord_tick(&c,1000100,3,1,3));
    assert(!panel_chord_tick(&c,3000100,3,1,3));
    assert(!panel_chord_tick(&c,3000101,1,1,1));
    assert(!panel_chord_tick(&c,3000102,3,1,3)); /* Must release both to rearm. */
    assert(!panel_chord_tick(&c,3000103,0,1,0));
    assert(!panel_chord_tick(&c,0xfffffff0UL,3,1,3));
    assert(panel_chord_tick(&c,(uint32_t)(0xfffffff0UL+1000000UL),3,1,3));
    assert(!panel_chord_tick(&c,0,0,1,0));
    assert(!panel_chord_tick(&c,100,3,0,3)); /* No arming during playback/gate. */
    assert(!panel_chord_tick(&c,200,3,1,3));
    assert(!panel_chord_tick(&c,2000000,3,1,3));
    assert(!panel_chord_tick(&c,2000001,0,1,0));
    assert(!panel_chord_tick(&c,3000000,3,1,1)); /* Both original states must be captured. */
    assert(panel_adjust(120,16,127,1)==127);
    assert(panel_adjust(7,16,127,0)==0);
    assert(panel_adjust(64,1,127,1)==65);
    assert(panel_adjust(64,1,127,0)==63);
    assert(panel_adjust(0,128,16383,1)==128);
    assert(panel_adjust(16380,128,16383,1)==16383);
    return 0;
}
