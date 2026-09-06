; Firmware 1.1.1 ABI addresses, register offsets and constants.
ADCA_INTFLGCLR                                       .set 0x7404
ADCA_SOCFRC1                                         .set 0x740d
ADC_INT2_ENABLE_BIT                                  .set 0x2000
CONTROL_14BIT_TO_UNIT_F32_HI                         .set 0x3880
CONTROL_OUTPUT_OVERRIDE_OFFSET                       .set 0x8
CONTROL_STATE_RHYTHM_SNAPSHOT_OFFSET                 .set 0x80
CONTROL_STATE_SINGLETON                              .set 0xd580
DIAGNOSTIC_OUTPUT_OVERRIDE_ENABLE_OFFSET             .set 0x0
DIAGNOSTIC_RAW_TARGET_U14_OFFSET                     .set 0x2
DIAGNOSTIC_RESET_DRIVE_EPWM_MANUAL                   .set 0x2
DIAGNOSTIC_RESET_DRIVE_EPWM_TRAIN                    .set 0x4
DIAGNOSTIC_RESET_DRIVE_FORCE_LOW                     .set 0x1
DIAGNOSTIC_RESET_DRIVE_GPIO_MANUAL                   .set 0x3
DIAGNOSTIC_RESET_DRIVE_MODE_OFFSET                   .set 0x3
DIAGNOSTIC_RESET_DRIVE_STOCK                         .set 0x0
DIAGNOSTIC_RESET_OUTPUT_COUNT                        .set 0x4
DIAGNOSTIC_RESET_TRAIN_COUNTDOWN_OFFSET              .set 0x2
DIAGNOSTIC_RESET_TRAIN_OUTPUT_OFFSET                 .set 0x0
DIAGNOSTIC_RESET_TRAIN_PERIOD_TICKS_OFFSET           .set 0x1
DIAGNOSTIC_RESET_TRAIN_STATE_BASE                    .set 0xf82c
DIAGNOSTIC_SCRATCH_OFFSET                            .set 0x4
DIAGNOSTIC_SELECTED_SLOT_OFFSET                      .set 0x1
DIAGNOSTIC_SETTING_OUTPUT_OVERRIDE                   .set 0x1
DIAGNOSTIC_SETTING_RAW_TARGET_U14                    .set 0x3
DIAGNOSTIC_SETTING_RESET_DRIVE_MODE                  .set 0x4
DIAGNOSTIC_SETTING_RESET_PULSE_OUTPUT                .set 0x5
DIAGNOSTIC_SETTING_RESET_TRAIN_PERIOD_TICKS          .set 0x6
DIAGNOSTIC_SETTING_SELECTED_SLOT                     .set 0x2
DIAGNOSTIC_SLOT_COUNT                                .set 0x6
DIAGNOSTIC_STATE_PRIMARY_BASE                        .set 0xf812
DIAGNOSTIC_SYSEX_CAPTURE_BYTES                       .set 0x10
ENGINE_CONTROL_TICK_ACCESS_GATE_OFFSET               .set 0xb19
ENGINE_CONTROL_UPDATE_INHIBIT                        .set 0xafc
ENGINE_RHYTHM_ENGINE_OFFSET                          .set 0xae2
ENGINE_SEQ1_ENVELOPE_TRIGGER_ENABLE_OFFSET           .set 0xae1
ENGINE_TRANSPORT_RUNNING_OFFSET                      .set 0xae2
ENGINE_TRIGGER_OFF_PENDING_OFFSET                    .set 0xadf
ENGINE_TRIGGER_ON_PENDING_OFFSET                     .set 0xade
ENGINE_VCA_ENVELOPE_OFFSET                           .set 0xc04
ENGINE_VCF_ENVELOPE_OFFSET                           .set 0xbb4
ENGINE_VCO1_PITCH_OUTPUT_OFFSET                      .set 0xca4
ENGINE_VCO1_SUB1_OUTPUT_OFFSET                       .set 0xc54
ENGINE_VCO1_SUB2_OUTPUT_OFFSET                       .set 0xc5e
ENGINE_VCO2_PITCH_OUTPUT_OFFSET                      .set 0xcae
ENGINE_VCO2_SUB1_OUTPUT_OFFSET                       .set 0xc7c
ENGINE_VCO2_SUB2_OUTPUT_OFFSET                       .set 0xc86
ENVELOPE_GATE_REQUEST_OFFSET                         .set 0x8
EPWM1_A_GPIO                                         .set 0x0
EPWM1_BASE                                           .set 0x4000
EPWM1_B_GPIO                                         .set 0x1
EPWM2_A_GPIO                                         .set 0x2
EPWM2_BASE                                           .set 0x4100
EPWM2_B_GPIO                                         .set 0x3
EPWM_AQCSFRC_FORCE_BOTH_HIGH                         .set 0xa
EPWM_AQCSFRC_FORCE_BOTH_LOW                          .set 0x5
EPWM_AQCSFRC_FORCE_DISABLED                          .set 0x0
EPWM_AQSFRC_RELOAD_IMMEDIATE                         .set 0xc0
EPWM_OUTPUT_BOTH                                     .set 0x2
EPWM_O_AQCSFRC                                       .set 0x49
EPWM_O_AQSFRC                                        .set 0x47
FRONT_PANEL_MODE_DIRECT                              .set 0x1c
GPIO_0_EPWM1_A                                       .set 0x60001
GPIO_0_GPIO0                                         .set 0x60000
GPIO_1_EPWM1_B                                       .set 0x60201
GPIO_1_GPIO1                                         .set 0x60200
GPIO_2_EPWM2_A                                       .set 0x60401
GPIO_2_GPIO2                                         .set 0x60400
GPIO_3_EPWM2_B                                       .set 0x60601
GPIO_3_GPIO3                                         .set 0x60600
GPIO_DATA_CLEAR_OFFSET                               .set 0x4
GPIO_DATA_REGS_BASE                                  .set 0x7f00
GPIO_DATA_SET_OFFSET                                 .set 0x2
MIDI_CC_LEGATO_FOOTSWITCH                            .set 0x44
MIDI_DEVICE_ID_CONFIG_DP                             .set 0x30e
MIDI_DEVICE_ID_DIRECT                                .set 0x3c
MIDI_DISPATCH_CONTEXT_SINGLETON                      .set 0xc300
MIDI_NOTE_MANAGER_SINGLETON                          .set 0xc040
MIDI_SYSEX_BROADCAST_DEVICE_ID                       .set 0x7f
MIDI_SYSEX_CAPTURE_COUNT_OFFSET                      .set 0x1a
MIDI_SYSEX_PARAMETER_ID_OFFSET                       .set 0x55
MIDI_SYSEX_PARAMETER_VALUE_LSB_OFFSET                .set 0x57
MIDI_SYSEX_PARAMETER_VALUE_MSB_OFFSET                .set 0x56
MIDI_SYSEX_UNIT_ADDRESS_OFFSET                       .set 0x60
NVRAM_RHYTHM_FORMAT_MARKER                           .set 0x1a2b
NVRAM_RHYTHM_FORMAT_RECORD                           .set 0xc14
NVRAM_RHYTHM_STATE_RECORD                            .set 0xc16
NVRAM_SERVICE_SINGLETON                              .set 0xc006
PERIODIC_SCHEDULER_EPOCH_DP                          .set 0x303
PERIODIC_SCHEDULER_EPOCH_US_DIRECT                   .set 0x2
PERIODIC_SCHEDULER_SINGLETON                         .set 0xc0c0
PERIODIC_SCHEDULER_TICK_US_DIRECT                    .set 0x0
PERSISTENCE_LOAD_CALLBACK_OFFSET                     .set 0x12
PERSISTENCE_LOAD_CONTEXT_OFFSET                      .set 0x10
PIECTRL_REGS_DP                                      .set 0x33
RHYTHM_ASSIGNMENT_UI_BASE                            .set 0xee70
RHYTHM_ENGINE_ASSIGN1_OFFSET                         .set 0xe
RHYTHM_ENGINE_RUNNING_DIRECT                         .set 0x22
RHYTHM_ENGINE_STATE_DP                               .set 0x3b9
RHYTHM_OUTPUT_PULSE_MS                               .set 0x5
SUBDIVISION_STATE_TIMING_OBJECT_OFFSET               .set 0x10
SUBOSC_IDLE_COMPARE                                  .set 0x401
SUBOSC_RESET_PULSE_FALL_COMPARE                      .set 0x2d4
SYNTH_ENGINE_SINGLETON                               .set 0xe380
VCO1_SUBDIVISION_STATE_PTR_DIRECT                    .set 0x20
VCO2_SUBDIVISION_STATE_PTR_DIRECT                    .set 0x22
VCO_SUBDIVISION_STATE_PTR_DP                         .set 0x3e0
        .ref cla_task8_acknowledge
        .ref control_output_set_float
        .ref cpp_operator_delete
        .ref dequeue_midi_message_record
        .ref disable_ecap_event1_interrupt
        .ref drive_expander_signal_configured_active_level
        .ref drive_expander_signal_configured_inactive_level
        .ref emit_clock_and_sequencer_pulses
        .ref emit_trigger_output_pulse
        .ref enable_ecap_event1_interrupt
        .ref envelope_set_gate
        .ref epwm_pair_force_timebase_sync
        .ref epwm_pair_set_raw_compare
        .ref feature_cc_low_continue
        .ref feature_manager_reset_continue
        .ref feature_note_running_continue
        .ref feature_remove_finish
        .ref gpio_set_pin_config
        .ref gpio_set_pin_output_enable
        .ref initialize_control_state_defaults
        .ref initialize_control_state_from_persistence
        .ref map_pitch_sum_to_quantized_cv
        .ref midi_cc_accepted
        .ref midi_cc_dispatch_above_68
        .ref midi_held_note_insert
        .ref midi_held_note_remove_test
        .ref midi_note_dispatch_accepted
        .ref midi_note_reselect
        .ref midi_note_stopped_transport
        .ref periodic_scheduler_now_us
        .ref platform_startup
        .ref poll_digital_control_activation_edge
        .ref propagate_control_state_changes
        .ref read_discrete_control_value
        .ref refresh_all_panel_analog_control_references
        .ref release_both_envelopes
        .ref reset_midi_note_manager_state
        .ref rts_memcmp
        .ref sysex_finish_moog_command
        .ref sysex_set_parameter
        .ref tick_vco_suboscillator_reset_pulses
        .ref trigger_both_envelopes
        .ref trigger_timed_gpio_pulse_ms
        .ref update_pitch_control_outputs
