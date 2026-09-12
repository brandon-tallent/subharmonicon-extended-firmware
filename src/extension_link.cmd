/* Sparse extension: no stock code or data sections. */
MEMORY
{
 PAGE 0:
  P084c98: origin=0x084c98, length=0x2
  P084ca3: origin=0x084ca3, length=0x2
  P085c44: origin=0x085c44, length=0x2
  P085caf: origin=0x085caf, length=0x2
  P085d82: origin=0x085d82, length=0x2
  P085d9a: origin=0x085d9a, length=0x2
  P08647f: origin=0x08647f, length=0x2
  P086523: origin=0x086523, length=0x1
  P08679e: origin=0x08679e, length=0x2
  P087024: origin=0x087024, length=0x3
  P087428: origin=0x087428, length=0x2
  P08744a: origin=0x08744a, length=0x2
  P08763d: origin=0x08763d, length=0x1
  P087640: origin=0x087640, length=0x1
  P087646: origin=0x087646, length=0x1
  P087659: origin=0x087659, length=0x1
  P08765f: origin=0x08765f, length=0x1
  P08766c: origin=0x08766c, length=0x2
  P08768e: origin=0x08768e, length=0x2
  P0876a9: origin=0x0876a9, length=0x2
  P0876c3: origin=0x0876c3, length=0x2
  P087aa6: origin=0x087aa6, length=0x1
  P087aae: origin=0x087aae, length=0x1
  P087ab6: origin=0x087ab6, length=0x1
  P087abe: origin=0x087abe, length=0x1
  P087b0f: origin=0x087b0f, length=0x1
  P087b10: origin=0x087b10, length=0x1
  P087b18: origin=0x087b18, length=0x1
  P087b19: origin=0x087b19, length=0x1
  P087b21: origin=0x087b21, length=0x1
  P087b22: origin=0x087b22, length=0x1
  P087b29: origin=0x087b29, length=0x1
  P087b2a: origin=0x087b2a, length=0x1
  P0892ac: origin=0x0892ac, length=0x2
  P089316: origin=0x089316, length=0x7
  P089ff4: origin=0x089ff4, length=0x2
  P08a3d3: origin=0x08a3d3, length=0x2
  P08b0bc: origin=0x08b0bc, length=0x2
  P08b1a4: origin=0x08b1a4, length=0x2
  P08b1a6: origin=0x08b1a6, length=0x2
  P08b1bd: origin=0x08b1bd, length=0x2
  P08b27d: origin=0x08b27d, length=0x2
  P08b2c3: origin=0x08b2c3, length=0x4
  P08b348: origin=0x08b348, length=0x2
  P08bfb5: origin=0x08bfb5, length=0x1
  P08d0d5: origin=0x08d0d5, length=0x2
  P08e0ea: origin=0x08e0ea, length=0x2
  P08e169: origin=0x08e169, length=0x2
  P0903b3: origin=0x0903b3, length=0x2
  P0903d1: origin=0x0903d1, length=0x2
  P0904b0: origin=0x0904b0, length=0x2
  P090bd8: origin=0x090bd8, length=0x2
  P090bda: origin=0x090bda, length=0x2
  P09110f: origin=0x09110f, length=0x2
  P0919f6: origin=0x0919f6, length=0x2
  P091b01: origin=0x091b01, length=0x3
  P092005: origin=0x092005, length=0x2
  P092ac5: origin=0x092ac5, length=0x3
  P0943df: origin=0x0943df, length=0x1
  P0946ee: origin=0x0946ee, length=0x5
  P0946f6: origin=0x0946f6, length=0x4
  P0946fd: origin=0x0946fd, length=0x4
  P09477e: origin=0x09477e, length=0x200
  FEATURES: origin=0x09497e, length=0xb672
  FEATURE_FAST_RUN: origin=0x009800, length=0x800
 PAGE 1:
  FEATURE_RAM: origin=0x010200, length=0x1000
}
cla_task8_acknowledge=0x91b69;
control_output_set_float=0x91d7b;
cpp_operator_delete=0x92fdc;
dequeue_midi_message_record=0x90726;
disable_ecap_event1_interrupt=0x8f9ba;
drive_expander_signal_configured_active_level=0x8e93e;
drive_expander_signal_configured_inactive_level=0x8e99c;
emit_clock_and_sequencer_pulses=0x85d94;
emit_trigger_output_pulse=0x85d90;
enable_ecap_event1_interrupt=0x8f9b3;
envelope_set_gate=0x8fe2b;
epwm_pair_force_timebase_sync=0x8c57f;
epwm_pair_set_raw_compare=0x8c551;
feature_cc_low_continue=0x8d0d7;
feature_manager_reset_continue=0x8b0be;
feature_note_running_continue=0x8b2c7;
feature_remove_finish=0x8b1b7;
gpio_set_pin_config=0x90900;
gpio_set_pin_output_enable=0x909b1;
initialize_control_state_defaults=0x89e43;
initialize_control_state_from_persistence=0x8a3e9;
map_pitch_sum_to_quantized_cv=0x84730;
midi_cc_accepted=0x8d228;
midi_cc_dispatch_above_68=0x8d10d;
midi_held_note_insert=0x8b1f6;
midi_held_note_remove_test=0x8b1aa;
midi_note_dispatch_accepted=0x8b2d3;
midi_note_reselect=0x8b131;
midi_note_stopped_transport=0x8b2cb;
periodic_scheduler_now_us=0x8fd99;
platform_startup=0x92efd;
poll_digital_control_activation_edge=0x8ea44;
propagate_control_state_changes=0x8a0e6;
read_discrete_control_value=0x8cf74;
refresh_all_panel_analog_control_references=0x86b78;
release_both_envelopes=0x85762;
reset_midi_note_manager_state=0x8b0bc;
rts_memcmp=0x92f3b;
sysex_finish_moog_command=0x893a4;
sysex_set_parameter=0x8a389;
tick_vco_suboscillator_reset_pulses=0x91fc7;
trigger_both_envelopes=0x8577b;
trigger_timed_gpio_pulse_ms=0x8dee6;
update_pitch_control_outputs=0x847be;
forced_long_zero_u16=0;
SECTIONS
{
 .patch_084c98: > P084c98, PAGE=0
 .patch_084ca3: > P084ca3, PAGE=0
 .patch_085c44: > P085c44, PAGE=0
 .patch_085caf: > P085caf, PAGE=0
 .patch_085d82: > P085d82, PAGE=0
 .patch_085d9a: > P085d9a, PAGE=0
 .patch_08647f: > P08647f, PAGE=0
 .patch_086523: > P086523, PAGE=0
 .patch_08679e: > P08679e, PAGE=0
 .patch_087024: > P087024, PAGE=0
 .patch_087428: > P087428, PAGE=0
 .patch_08744a: > P08744a, PAGE=0
 .patch_08763d: > P08763d, PAGE=0
 .patch_087640: > P087640, PAGE=0
 .patch_087646: > P087646, PAGE=0
 .patch_087659: > P087659, PAGE=0
 .patch_08765f: > P08765f, PAGE=0
 .patch_08766c: > P08766c, PAGE=0
 .patch_08768e: > P08768e, PAGE=0
 .patch_0876a9: > P0876a9, PAGE=0
 .patch_0876c3: > P0876c3, PAGE=0
 .patch_087aa6: > P087aa6, PAGE=0
 .patch_087aae: > P087aae, PAGE=0
 .patch_087ab6: > P087ab6, PAGE=0
 .patch_087abe: > P087abe, PAGE=0
 .patch_087b0f: > P087b0f, PAGE=0
 .patch_087b10: > P087b10, PAGE=0
 .patch_087b18: > P087b18, PAGE=0
 .patch_087b19: > P087b19, PAGE=0
 .patch_087b21: > P087b21, PAGE=0
 .patch_087b22: > P087b22, PAGE=0
 .patch_087b29: > P087b29, PAGE=0
 .patch_087b2a: > P087b2a, PAGE=0
 .patch_0892ac: > P0892ac, PAGE=0
 .patch_089316: > P089316, PAGE=0
 .patch_089ff4: > P089ff4, PAGE=0
 .patch_08a3d3: > P08a3d3, PAGE=0
 .patch_08b0bc: > P08b0bc, PAGE=0
 .patch_08b1a4: > P08b1a4, PAGE=0
 .patch_08b1a6: > P08b1a6, PAGE=0
 .patch_08b1bd: > P08b1bd, PAGE=0
 .patch_08b27d: > P08b27d, PAGE=0
 .patch_08b2c3: > P08b2c3, PAGE=0
 .patch_08b348: > P08b348, PAGE=0
 .patch_08bfb5: > P08bfb5, PAGE=0
 .patch_08d0d5: > P08d0d5, PAGE=0
 .patch_08e0ea: > P08e0ea, PAGE=0
 .patch_08e169: > P08e169, PAGE=0
 .patch_0903b3: > P0903b3, PAGE=0
 .patch_0903d1: > P0903d1, PAGE=0
 .patch_0904b0: > P0904b0, PAGE=0
 .patch_090bd8: > P090bd8, PAGE=0
 .patch_090bda: > P090bda, PAGE=0
 .patch_09110f: > P09110f, PAGE=0
 .patch_0919f6: > P0919f6, PAGE=0
 .patch_091b01: > P091b01, PAGE=0
 .patch_092005: > P092005, PAGE=0
 .patch_092ac5: > P092ac5, PAGE=0
 .patch_0943df: > P0943df, PAGE=0
 .patch_0946ee: > P0946ee, PAGE=0
 .patch_0946f6: > P0946f6, PAGE=0
 .patch_0946fd: > P0946fd, PAGE=0
 .patch_09477e: > P09477e, PAGE=0
 .feature_bridge: > FEATURES, PAGE=0
 .text: > FEATURES, PAGE=0
 .econst: > FEATURES, PAGE=0
 .switch: > FEATURES, PAGE=0
 .feature_fast: LOAD=0x09497e, RUN=FEATURE_FAST_RUN, PAGE=0,
  LOAD_START(_feature_fast_load), RUN_START(_feature_fast_run),
  LOAD_SIZE(_feature_fast_size)
 .ebss: > FEATURE_RAM, PAGE=1
 .bss: > FEATURE_RAM, PAGE=1
 .feature_scan_state: > FEATURE_RAM, PAGE=1
 .feature_harmony_state: > FEATURE_RAM, PAGE=1
 .feature_glide_state: > FEATURE_RAM, PAGE=1
 .feature_settings_state: > FEATURE_RAM, PAGE=1
 .feature_velocity_state: > FEATURE_RAM, PAGE=1
}
