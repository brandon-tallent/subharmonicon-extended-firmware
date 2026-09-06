#ifndef SUBH_FEATURES_RHYTHM_H
#define SUBH_FEATURES_RHYTHM_H
#include <stdint.h>
extern volatile uint16_t feature_rhythm_type;
void feature_rhythm_init(void);
void feature_rhythm_reset(void);
void feature_morph_clock(uint16_t phase, uint16_t running);
void feature_morph_clock_reset(void);
void feature_rhythm_cc(uint16_t lane, uint16_t value);
#endif
