#ifndef SUBH_FEATURES_CONFIG_H
#define SUBH_FEATURES_CONFIG_H
#include <stdint.h>
#define FEATURE_CONFIG_MAGIC 0x5348U
#define FEATURE_CONFIG_VERSION 10U
#define FEATURE_CONFIG_WORDS 10U
struct FeatureConfig { uint16_t spread; uint16_t span; uint16_t glide; uint16_t glide_time; uint16_t velocity_depth; uint16_t mode; uint16_t rhythm; uint16_t noise; uint16_t midi_vcf; };
/* Version 10 packs the MIDI split switch beside noise in the final word.
 * Version 9 remains readable so an existing panel save keeps its settings. */
static uint16_t feature_config_decode(const volatile uint16_t *words,
                                      volatile struct FeatureConfig *config)
{
    config->spread=0;config->span=0;config->glide=0;
    config->glide_time=0;config->velocity_depth=0;config->mode=0;config->rhythm=1;config->noise=0;config->midi_vcf=0;
    if(words[0]!=FEATURE_CONFIG_MAGIC || (words[1]!=9 && words[1]!=FEATURE_CONFIG_VERSION) ||
       words[2]>127 || words[3]>2 || words[4]>1 || words[5]>16383 || words[6]>127 || words[7]>3 || words[8]>1 || words[9]>(words[1]==9 ? 1 : 3)) return 0;
    config->spread=words[2];config->span=words[3];config->glide=words[4];
    config->glide_time=words[5];config->velocity_depth=words[6];config->mode=words[7];config->rhythm=words[8];config->noise=words[9]&1;
    if(words[1]==FEATURE_CONFIG_VERSION) config->midi_vcf=(words[9]>>1)&1;
    return 1;
}
static void feature_config_encode(volatile uint16_t *words,
                                  const volatile struct FeatureConfig *config)
{
    words[0]=FEATURE_CONFIG_MAGIC; words[1]=FEATURE_CONFIG_VERSION;
    words[2]=config->spread; words[3]=config->span;
    words[4]=config->glide; words[5]=config->glide_time;
    words[6]=config->velocity_depth;words[7]=config->mode;words[8]=config->rhythm;words[9]=config->noise|(config->midi_vcf<<1);
}
/* Panel sessions publish only the settings edited there, in one image save.
 * Values/mask are internal, already validated when the live edit was applied. */
static void feature_config_apply_edits(volatile struct FeatureConfig *config,
                                       uint16_t dirty, const volatile uint16_t *values)
{
    if(dirty&1) config->spread=values[0];
    if(dirty&2) config->span=values[1];
    if(dirty&4) config->glide=values[2];
    if(dirty&8) config->glide_time=values[3];
    if(dirty&16) config->velocity_depth=values[4];
    if(dirty&32) config->mode=values[5];
    if(dirty&128) config->noise=values[7];
    if(dirty&64) config->rhythm=values[6];
    if(dirty&256) config->midi_vcf=values[8];
}
static uint16_t feature_config_value_valid(uint16_t id, uint16_t value)
{
    return (id==5 && value<=1) || (id==4 && value<=2) || (id==6 && value<=16383) || ((id==3 || id==7) && value<=127) || (id==8 && value<=3) || ((id==9 || id==10 || id==11) && value<=1);
}
#endif
