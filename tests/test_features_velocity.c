#include <assert.h>
#include <math.h>
#include <stdint.h>
#include "features_velocity.h"
int main(void)
{
    unsigned d,v,e;
    for(d=0;d<=127;++d) for(e=0;e<=100;++e) {
        float inverted=(float)e/100.0f,previous=1.0f;
        assert(velocity_vca(inverted,0,1)==inverted);
        assert(velocity_vca(inverted,d,127)==inverted);
        assert(velocity_vca(1.0f,d,1)==1.0f); /* Closed remains closed. */
        for(v=1;v<=127;++v) {
            float out=velocity_vca(inverted,d,v);
            double level=(1.0-(double)inverted)*(1.0-(double)d/127*(1.0-(double)v/127));
            assert(out>=inverted-1e-7f && out<=1.0f);
            assert(out<=previous+1e-7f); /* Higher velocity opens the VCA. */
            assert(fabs((1.0-(double)out)-level)<2e-7);
            previous=out;
        }
    }
    return 0;
}
