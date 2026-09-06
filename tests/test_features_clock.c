#include <assert.h>
#include <stdint.h>

static uint32_t counter, step, reads;
static uint32_t feature_now(void)
{
    uint32_t value=counter;
    counter+=step;
    ++reads;
    return value;
}
#include "features_clock.h"

int main(void)
{
    counter=100; step=0; reads=0;
    assert(!feature_wait_until(200));
    assert(reads==FEATURE_WAIT_LIMIT);
    counter=100; step=10; reads=0;
    assert(feature_wait_until(200));
    assert(reads==11);
    counter=UINT32_MAX-49; step=10; reads=0;
    assert(feature_wait_until(50));
    assert(reads==11);
    counter=200; step=0; reads=0;
    assert(feature_wait_until(200));
    assert(reads==1);
    counter=201; step=0; reads=0;
    assert(feature_wait_until(200));
    assert(reads==1);
    return 0;
}
