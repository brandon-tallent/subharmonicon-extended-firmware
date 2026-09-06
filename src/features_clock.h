#ifndef SUBH_FEATURES_CLOCK_H
#define SUBH_FEATURES_CLOCK_H
#include <stdint.h>
#define FEATURE_WAIT_LIMIT 4096U

/* The includer supplies feature_now(). Bound CPU work independently of the
 * peripheral clock: a stopped counter must never trap an interrupt handler.
 * Deadlines must be less than 2^31 ticks ahead, including across wraparound. */
static uint16_t feature_wait_until(uint32_t deadline)
{
    uint16_t attempts;
    for (attempts=0; attempts<FEATURE_WAIT_LIMIT; ++attempts)
        if ((int32_t)(feature_now()-deadline)>=0) return 1;
    return 0;
}
#endif
