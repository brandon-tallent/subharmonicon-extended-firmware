#include <assert.h>
#include "features_config.h"
int main(void)
{
    uint16_t words[FEATURE_CONFIG_WORDS]={0},image[128];
    struct FeatureConfig c={0};unsigned i;
    assert(!feature_config_decode(words,&c) && !c.spread);
    c.spread=1;c.span=2;c.glide=1;c.glide_time=16383;c.velocity_depth=127;
    feature_config_encode(words,&c);
    assert(feature_config_decode(words,&c) && c.spread==1 && c.span==2 && c.glide==1 && c.glide_time==16383 && c.velocity_depth==127);
    for(i=0;i<=8;++i) {
        words[1]=i;assert(!feature_config_decode(words,&c));
        assert(!c.spread && !c.span && !c.glide && !c.glide_time && !c.velocity_depth && !c.mode && c.rhythm==1);
    }
    words[1]=9;words[9]=1;
    assert(feature_config_decode(words,&c) && c.noise==1 && !c.midi_vcf);
    words[1]=FEATURE_CONFIG_VERSION;
    {
        const uint16_t invalid[7]={128,3,2,16384,128,4,2};
        for(i=0;i<7;++i) {
            uint16_t saved=words[i+2];words[i+2]=invalid[i];
            assert(!feature_config_decode(words,&c));words[i+2]=saved;
        }
    }
    words[0]=0;assert(!feature_config_decode(words,&c));words[0]=FEATURE_CONFIG_MAGIC;
    assert(feature_config_decode(words,&c));
    words[9]=4;assert(!feature_config_decode(words,&c));words[9]=1;
    assert(feature_config_decode(words,&c));
    for(i=0;i<128;++i) image[i]=(uint16_t)(0x1200+i);
    feature_config_encode(image+0x55,&c);
    for(i=0;i<128;++i) if(i<0x55 || i>=0x55+FEATURE_CONFIG_WORDS) assert(image[i]==0x1200+i);
    assert(feature_config_decode(image+0x55,&c) && c.spread==1);
    assert(feature_config_value_valid(3,0) && feature_config_value_valid(3,1));
    assert(!feature_config_value_valid(3,128) && !feature_config_value_valid(4,3));
    assert(!feature_config_value_valid(2,1) && feature_config_value_valid(5,1));
    assert(feature_config_value_valid(7,0) && feature_config_value_valid(7,127));
    assert(!feature_config_value_valid(7,128));
    assert(feature_config_value_valid(8,1) && feature_config_value_valid(9,1) && !feature_config_value_valid(9,2));
    {
        uint16_t edits[9]={0,1,0,1234,127,0,1,0,1};
        c.spread=1;c.span=2;c.glide=1;c.glide_time=42;c.velocity_depth=12;
        feature_config_apply_edits(&c,16,edits);
        assert(c.spread==1 && c.span==2 && c.glide==1 && c.glide_time==42 && c.velocity_depth==127);
        feature_config_apply_edits(&c,0,edits);assert(c.spread==1);
        feature_config_apply_edits(&c,31,edits);
        assert(!c.spread && c.span==1 && !c.glide && c.glide_time==1234);
    }
    c.spread=1;c.mode=2;c.rhythm=1;feature_config_encode(words,&c);
    assert(feature_config_decode(words,&c) && c.mode==2 && c.rhythm==1);
    words[2]=0;assert(feature_config_decode(words,&c));
    words[2]=1;words[7]=1;assert(feature_config_decode(words,&c) && c.mode==1);
    words[7]=0;assert(feature_config_decode(words,&c) && c.mode==0);
    assert(feature_config_value_valid(8,2) && !feature_config_value_valid(8,4));
    { uint16_t split_edit[9]={0};split_edit[8]=1;feature_config_apply_edits(&c,256,split_edit); }
    feature_config_encode(words,&c);
    assert(feature_config_decode(words,&c) && c.midi_vcf==1);
    assert(feature_config_value_valid(11,1) && !feature_config_value_valid(11,2));
    return 0;
}
