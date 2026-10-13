        .global feature_startup_hook
        .include "src/stock_abi.inc"
; Authored fixed-address replacements for firmware 1.1.1. Word addresses.
        .sect ".patch_084c98"
        .retain
        LCR feature_mono_vco1_output_hook
        .sect ".patch_084ca3"
        .retain
        LCR feature_mono_vco2_output_hook
        .sect ".patch_085c44"
        .retain
        LCR feature_vca_velocity_hook
        .sect ".patch_085caf"
        .retain
        LCR feature_pitch_hook
        .sect ".patch_085d82"
        .retain
        LB feature_rhythm_clock_dispatch
        .sect ".patch_085d9a"
        .retain
        LCR feature_rhythm_clock_output
        .sect ".patch_08647f"
        .retain
        LCR feature_rhythm_restore_routes
        .sect ".patch_086523"
        .retain
        .word 0x9a17 ; 24-PPQN clock rate index (MOVB AL immediate)
        .sect ".patch_08679e"
        .retain
        LCR feature_step_led_guard
        .sect ".patch_087024"
        .retain
        LCR feature_settings_guard_hook
        NOP
        .sect ".patch_087428"
        .retain
        LCR feature_sub1_button1_hook
        .sect ".patch_08744a"
        .retain
        LCR feature_sub2_button1_hook
        .sect ".patch_08763d"
        .retain
        .word 0x1a40 ; tempo span 200.0f, high immediate word
        .sect ".patch_087640"
        .retain
        .word 0xe808 ; tempo span 200.0f, zero low immediate (MOVXI)
        .sect ".patch_087646"
        .retain
        .word 0x8800 ; tempo minimum 40.0f, high immediate word
        .sect ".patch_087659"
        .retain
        .word 0x9a17 ; 24-PPQN clock rate index (MOVB AL immediate)
        .sect ".patch_08765f"
        .retain
        .word 0x9a17 ; 24-PPQN clock rate index (MOVB AL immediate)
        .sect ".patch_08766c"
        .retain
        LCR feature_rhythm_knob0_hook
        .sect ".patch_08768e"
        .retain
        LCR feature_rhythm_knob1_hook
        .sect ".patch_0876a9"
        .retain
        LCR feature_rhythm_knob2_hook
        .sect ".patch_0876c3"
        .retain
        LCR feature_rhythm_knob3_hook
        .sect ".patch_087aa6"
        .retain
        .word 0x9a20 ; 32-bin unsigned selector endpoint (MOVB AL immediate)
        .sect ".patch_087aae"
        .retain
        .word 0x9a20 ; 32-bin unsigned selector endpoint (MOVB AL immediate)
        .sect ".patch_087ab6"
        .retain
        .word 0x9a20 ; 32-bin unsigned selector endpoint (MOVB AL immediate)
        .sect ".patch_087abe"
        .retain
        .word 0x9a20 ; 32-bin unsigned selector endpoint (MOVB AL immediate)
        .sect ".patch_087b0f"
        .retain
        .word 0xfff0 ; signed selector lower endpoint -16, immediate word
        .sect ".patch_087b10"
        .retain
        .word 0x9b10 ; signed selector upper endpoint +16 (MOVB AH immediate)
        .sect ".patch_087b18"
        .retain
        .word 0xfff0 ; signed selector lower endpoint -16, immediate word
        .sect ".patch_087b19"
        .retain
        .word 0x9b10 ; signed selector upper endpoint +16 (MOVB AH immediate)
        .sect ".patch_087b21"
        .retain
        .word 0xfff0 ; signed selector lower endpoint -16, immediate word
        .sect ".patch_087b22"
        .retain
        .word 0x9b10 ; signed selector upper endpoint +16 (MOVB AH immediate)
        .sect ".patch_087b29"
        .retain
        .word 0xfff0 ; signed selector lower endpoint -16, immediate word
        .sect ".patch_087b2a"
        .retain
        .word 0x9b10 ; signed selector upper endpoint +16 (MOVB AH immediate)
        .sect ".patch_0892ac"
        .retain
        LCR feature_parameter_hook
        .sect ".patch_089316"
        .retain
        LCR feature_sysex_hook
        B sysex_finish_moog_command,UNC
        .word 0xffff,0xffff,0xffff
        .sect ".patch_089ff4"
        .retain
        LCR feature_rhythm_routes_changed
        .sect ".patch_08a3d3"
        .retain
        LCR feature_config_defaults_hook
        .sect ".patch_08b0bc"
        .retain
        LB feature_manager_reset_hook
        .sect ".patch_08b1a4"
        .retain
        LCR feature_note_delete_hook
        .sect ".patch_08b1a6"
        .retain
        LB feature_remove_continue_hook
        .sect ".patch_08b15b"
        .retain
        LCR feature_midi_gate_off
        .sect ".patch_08b17d"
        .retain
        LCR feature_midi_gate_on
        .sect ".patch_08b1bd"
        .retain
        LCR feature_note_removed_hook
        .sect ".patch_08b27d"
        .retain
        LCR feature_note_added_hook
        .sect ".patch_08b2c3"
        .retain
        LB feature_note_route_hook
        NOP
        NOP
        .sect ".patch_08b29b"
        .retain
        LB feature_note_channel_hook
        NOP
        .sect ".patch_08b2bd"
        .retain
        LCR feature_note_off_hook
        .sect ".patch_08b348"
        .retain
        LCR feature_all_notes_off_hook
        .sect ".patch_089752"
        .retain
        LCR feature_midi_stop_hook
        .sect ".patch_08bfb5"
        .retain
        .word 0x9a17 ; 24-PPQN clock rate index (MOVB AL immediate)
        .sect ".patch_08d0d5"
        .retain
        LB feature_cc_hook
        .sect ".patch_08e0ea"
        .retain
        LCR feature_capture_hook
        .sect ".patch_08e169"
        .retain
        LCR feature_capture_hook
        .sect ".patch_0903b3"
        .retain
        LCR feature_startup_hook
        .sect ".patch_0903d1"
        .retain
        LCR feature_config_load_hook
        .sect ".patch_0904b0"
        .retain
        LCR feature_foreground_hook
        .sect ".patch_090bd8"
        .retain
        LB feature_rhythm_reset_hook
        .sect ".patch_090bda"
        .retain
        LB feature_rhythm_tick_hook
        .sect ".patch_09110f"
        .retain
        ; F28004x ADC flag-clear race: keep conversion interrupt pulses live.
        OR @AL,#(ADC_INT2_ENABLE_BIT | 0x4000) ; INT2CONT, TI SPRZ439
        .sect ".patch_0919f6"
        .retain
        LCR feature_adc_clear_interrupt_hook
        .sect ".patch_091b01"
        .retain
        LCR feature_adc_scan_gate_hook
        SB cla_task8_acknowledge,TC
        .sect ".patch_092005"
        .retain
        LCR diagnostic_enable_capture_event1_interrupt
        .sect ".patch_092ac5"
        .retain
        LCR feature_scheduler_epoch_hook
        NOP
        .sect ".patch_0943df"
        .retain
        .word 0x001f ; selector maximum 31 in initialization data
        .sect ".patch_0946ee"
        .retain
        .word 0x0000,0x0000,0x2000,0x0000,0x0000              ; disabled, slot 0, midscale raw target
        .sect ".patch_0946f6"
        .retain
        .word 0x0003,0x0000,0x0001,0x0000                     ; VCO2 SUB2, train stopped, next tick
        .sect ".patch_0946fd"
        .retain
        .word 0x0000,0x0000,0x0000,0x0000                     ; reserved diagnostic state
        .sect ".patch_09477e"
        .retain
; Command 0x35 is diagnostic-only in this image.  Require the complete fixed
; 16-byte captured frame, accept only this unit or broadcast, reconstruct the
; MSB-first U14 value, and admit only the implemented volatile settings.
; No path below writes persistence or reaches update/erase service commands.
diagnostic_sysex_command:
        MOVB XAR0,#MIDI_SYSEX_CAPTURE_COUNT_OFFSET
        MOV AL,*+XAR2[AR0]
        CMPB AL,#DIAGNOSTIC_SYSEX_CAPTURE_BYTES
        B diagnostic_sysex_done,NEQ
        MOVB XAR0,#MIDI_SYSEX_UNIT_ADDRESS_OFFSET
        MOVZ AR6,*+XAR2[AR0]
        MOV AL,@AR6
        CMPB AL,#MIDI_SYSEX_BROADCAST_DEVICE_ID
        SB diagnostic_sysex_target_accepted,EQ
        MOVW DP,#MIDI_DEVICE_ID_CONFIG_DP
        MOVU ACC,@MIDI_DEVICE_ID_DIRECT
        CMPL ACC,@XAR6
        B diagnostic_sysex_done,NEQ
diagnostic_sysex_target_accepted:
        MOVB XAR1,#MIDI_SYSEX_PARAMETER_VALUE_MSB_OFFSET
        MOV ACC,*+XAR2[AR1]<<#0x7
        MOVB XAR0,#MIDI_SYSEX_PARAMETER_VALUE_LSB_OFFSET
        MOVB XAR1,#MIDI_SYSEX_PARAMETER_ID_OFFSET
        OR AL,*+XAR2[AR0]
        MOV AH,@AL
        MOV AL,*+XAR2[AR1]
        CMPB AL,#DIAGNOSTIC_SETTING_OUTPUT_OVERRIDE
        SB diagnostic_set_output_override,EQ
        CMPB AL,#DIAGNOSTIC_SETTING_SELECTED_SLOT
        SB diagnostic_set_selected_slot,EQ
        CMPB AL,#DIAGNOSTIC_SETTING_RAW_TARGET_U14
        SB diagnostic_set_raw_target_u14,EQ
        CMPB AL,#DIAGNOSTIC_SETTING_RESET_DRIVE_MODE
        SB diagnostic_set_reset_drive_mode,EQ
        CMPB AL,#DIAGNOSTIC_SETTING_RESET_PULSE_OUTPUT
        SB diagnostic_trigger_reset_pulse_output,EQ
        CMPB AL,#DIAGNOSTIC_SETTING_RESET_TRAIN_PERIOD_TICKS
        SB diagnostic_set_reset_train_period_ticks,EQ
        B diagnostic_sysex_done,UNC

diagnostic_set_output_override:
        CMPB AH,#0x01
        B diagnostic_sysex_done,HI
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV *+XAR7[DIAGNOSTIC_SCRATCH_OFFSET],AH
        LCR diagnostic_clear_selected_output_override
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_SCRATCH_OFFSET]
        MOV *+XAR7[DIAGNOSTIC_OUTPUT_OVERRIDE_ENABLE_OFFSET],AL
        LCR diagnostic_apply_selected_output_override
        B diagnostic_sysex_done,UNC

diagnostic_set_selected_slot:
        CMPB AH,#(DIAGNOSTIC_SLOT_COUNT-1)
        B diagnostic_sysex_done,HI
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV *+XAR7[DIAGNOSTIC_SCRATCH_OFFSET],AH
        LCR diagnostic_clear_selected_output_override
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_SCRATCH_OFFSET]
        MOV *+XAR7[DIAGNOSTIC_SELECTED_SLOT_OFFSET],AL
        LCR diagnostic_apply_selected_output_override
        B diagnostic_sysex_done,UNC

diagnostic_set_raw_target_u14:
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV *+XAR7[DIAGNOSTIC_RAW_TARGET_U14_OFFSET],AH
        LCR diagnostic_apply_selected_output_override
        B diagnostic_sysex_done,UNC

; Zero stops the autonomous train.  Nonzero values are periods in 10-kHz
; control ticks; restart the countdown at one so a new rate takes effect on
; the next VCO2 control pass rather than after the previous period expires.
diagnostic_set_reset_train_period_ticks:
        MOVL XAR7,#DIAGNOSTIC_RESET_TRAIN_STATE_BASE
        MOV *+XAR7[DIAGNOSTIC_RESET_TRAIN_PERIOD_TICKS_OFFSET],AH
        MOV *+XAR7[DIAGNOSTIC_RESET_TRAIN_COUNTDOWN_OFFSET],#0x01
        B diagnostic_sysex_done,UNC

; All diagnostic drive modes mask the VCO edge-capture interrupts.  Modes two
; and four use the stock paired ePWM compare/sync mechanism for one-shot and
; autonomous pulses respectively.  Mode three remuxes GPIO0..3 to CPU GPIO.
; Mode zero restores the complete stock ePWM/capture path.
diagnostic_set_reset_drive_mode:
        CMPB AH,#DIAGNOSTIC_RESET_DRIVE_EPWM_TRAIN
        B diagnostic_sysex_done,HI
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV *+XAR7[DIAGNOSTIC_RESET_DRIVE_MODE_OFFSET],AH
        CMPB AH,#DIAGNOSTIC_RESET_DRIVE_STOCK
        SB diagnostic_restore_stock_reset_drive,EQ
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR4,@VCO1_SUBDIVISION_STATE_PTR_DIRECT
        LCR disable_ecap_event1_interrupt
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR4,@VCO2_SUBDIVISION_STATE_PTR_DIRECT
        LCR disable_ecap_event1_interrupt
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_RESET_DRIVE_MODE_OFFSET]
        CMPB AL,#DIAGNOSTIC_RESET_DRIVE_GPIO_MANUAL
        SB diagnostic_enter_gpio_reset_drive,EQ
        LCR diagnostic_route_reset_outputs_to_epwm
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_RESET_DRIVE_MODE_OFFSET]
        CMPB AL,#DIAGNOSTIC_RESET_DRIVE_FORCE_LOW
        SB diagnostic_force_reset_outputs_low,EQ
        MOVB AL,#EPWM_AQCSFRC_FORCE_BOTH_HIGH
        SB diagnostic_apply_reset_output_force,UNC
diagnostic_force_reset_outputs_low:
        MOVB AL,#EPWM_AQCSFRC_FORCE_BOTH_LOW
diagnostic_apply_reset_output_force:
        LCR diagnostic_force_suboscillator_reset_outputs
        B diagnostic_sysex_done,UNC
diagnostic_restore_stock_reset_drive:
        LCR diagnostic_route_reset_outputs_to_epwm
        MOVB AL,#EPWM_AQCSFRC_FORCE_DISABLED
        LCR diagnostic_force_suboscillator_reset_outputs
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR4,@VCO1_SUBDIVISION_STATE_PTR_DIRECT
        LCR enable_ecap_event1_interrupt
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR4,@VCO2_SUBDIVISION_STATE_PTR_DIRECT
        LCR enable_ecap_event1_interrupt
diagnostic_sysex_done:
        LRETR

; In either manual mode, output indexes 0..3 address VCO1 SUB1, VCO1 SUB2,
; VCO2 SUB1, and VCO2 SUB2.  GPIO mode emits one isolated stock-width pulse.
; ePWM mode stages a pulse on the selected channel and synchronizes its pair.
diagnostic_trigger_reset_pulse_output:
        CMPB AH,#(DIAGNOSTIC_RESET_OUTPUT_COUNT-1)
        B diagnostic_sysex_done,HI
        MOVL XAR7,#DIAGNOSTIC_RESET_TRAIN_STATE_BASE
        MOV *+XAR7[DIAGNOSTIC_RESET_TRAIN_OUTPUT_OFFSET],AH
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_RESET_DRIVE_MODE_OFFSET]
        CMPB AL,#DIAGNOSTIC_RESET_DRIVE_GPIO_MANUAL
        SB diagnostic_trigger_gpio_reset_pulse,EQ
        CMPB AL,#DIAGNOSTIC_RESET_DRIVE_EPWM_MANUAL
        B diagnostic_sysex_done,NEQ
        LCR diagnostic_trigger_selected_epwm_reset_pulse
        B diagnostic_sysex_done,UNC

; Emit one pulse on the reset output retained by setting five.  Output indexes
; 0..3 map to VCO1 SUB1, VCO1 SUB2, VCO2 SUB1, and VCO2 SUB2.
diagnostic_trigger_selected_epwm_reset_pulse:
        ; XAR2 is callee-saved and holds the engine in the 10-kHz caller.
        MOVL *SP++,XAR2
        MOVL XAR7,#DIAGNOSTIC_RESET_TRAIN_STATE_BASE
        MOV AH,*+XAR7[DIAGNOSTIC_RESET_TRAIN_OUTPUT_OFFSET]
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV *+XAR7[DIAGNOSTIC_SCRATCH_OFFSET],AH
        MOV AL,*+XAR7[DIAGNOSTIC_SCRATCH_OFFSET]
        CMPB AL,#0x01
        SB diagnostic_pulse_vco1_output,LOS
        SUBB AL,#0x02
        MOVZ AR5,@AL
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR2,@VCO2_SUBDIVISION_STATE_PTR_DIRECT
        SB diagnostic_pulse_output_ready,UNC
diagnostic_pulse_vco1_output:
        MOVZ AR5,@AL
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR2,@VCO1_SUBDIVISION_STATE_PTR_DIRECT
diagnostic_pulse_output_ready:
        MOVB XAR0,#SUBDIVISION_STATE_TIMING_OBJECT_OFFSET
        MOVL XAR4,*+XAR2[AR0]
        MOVL XAR6,#SUBOSC_RESET_PULSE_FALL_COMPARE
        MOVL ACC,@XAR6
        LCR epwm_pair_set_raw_compare
        MOVB XAR0,#SUBDIVISION_STATE_TIMING_OBJECT_OFFSET
        MOVL XAR4,*+XAR2[AR0]
        LCR epwm_pair_force_timebase_sync
        MOVB XAR0,#SUBDIVISION_STATE_TIMING_OBJECT_OFFSET
        MOVL XAR4,*+XAR2[AR0]
        MOVL XAR6,#SUBOSC_IDLE_COMPARE
        MOVB XAR5,#EPWM_OUTPUT_BOTH
        MOVL ACC,@XAR6
        LCR epwm_pair_set_raw_compare
        MOVB AL,#EPWM_AQCSFRC_FORCE_DISABLED
        LCR diagnostic_force_suboscillator_reset_outputs
        MOVL XAR2,*--SP
        LRETR

; Remux all four reset outputs to ordinary CPU GPIO.  Preload the output latch
; high and force the still-visible ePWM pins high before changing the mux, so
; entering diagnostic GPIO ownership cannot create an accidental low edge.
diagnostic_enter_gpio_reset_drive:
        MOVB AL,#EPWM_AQCSFRC_FORCE_BOTH_HIGH
        LCR diagnostic_force_suboscillator_reset_outputs
        MOVL XAR5,#GPIO_DATA_REGS_BASE
        MOVB ACC,#0x0f
        MOVL *+XAR5[GPIO_DATA_SET_OFFSET],ACC
        MOVB ACC,#EPWM1_A_GPIO
        MOVB XAR4,#0x01
        LCR gpio_set_pin_output_enable
        MOVB ACC,#EPWM1_B_GPIO
        MOVB XAR4,#0x01
        LCR gpio_set_pin_output_enable
        MOVB ACC,#EPWM2_A_GPIO
        MOVB XAR4,#0x01
        LCR gpio_set_pin_output_enable
        MOVB ACC,#EPWM2_B_GPIO
        MOVB XAR4,#0x01
        LCR gpio_set_pin_output_enable
        MOVL XAR4,#GPIO_0_GPIO0
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        MOVL XAR4,#GPIO_1_GPIO1
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        MOVL XAR4,#GPIO_2_GPIO2
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        MOVL XAR4,#GPIO_3_GPIO3
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        B diagnostic_sysex_done,UNC

; Restore the four stock ePWM pin functions while both the GPIO latch and the
; hidden ePWM action-qualifier outputs are high.  The caller chooses whether
; to leave ePWM forced or release it back to the subdivision engine.
diagnostic_route_reset_outputs_to_epwm:
        MOVL XAR5,#GPIO_DATA_REGS_BASE
        MOVB ACC,#0x0f
        MOVL *+XAR5[GPIO_DATA_SET_OFFSET],ACC
        MOVB AL,#EPWM_AQCSFRC_FORCE_BOTH_HIGH
        LCR diagnostic_force_suboscillator_reset_outputs
        MOVL XAR4,#GPIO_0_EPWM1_A
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        MOVL XAR4,#GPIO_1_EPWM1_B
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        MOVL XAR4,#GPIO_2_EPWM2_A
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        MOVL XAR4,#GPIO_3_EPWM2_B
        MOVL ACC,@XAR4
        LCR gpio_set_pin_config
        LRETR

; Issue one active-low pulse on exactly one of GPIO0..3.  Interrupt masking
; bounds the low interval to roughly 300 CPU cycles (3 us at 100 MHz), matching
; the stock reset-pulse width without involving either shared ePWM timebase.
diagnostic_trigger_gpio_reset_pulse:
        MOV T,@AH
        MOVB ACC,#0x01
        LSLL ACC,T
        MOVL XAR6,@ACC
        MOVL XAR5,#GPIO_DATA_REGS_BASE
        SETC INTM
        MOVL *+XAR5[GPIO_DATA_CLEAR_OFFSET],XAR6
        RPT #0xff || NOP
        RPT #0x2b || NOP
        MOVL *+XAR5[GPIO_DATA_SET_OFFSET],XAR6
        CLRC INTM
        B diagnostic_sysex_done,UNC

; The 10-kHz control loop normally re-arms each VCO capture after every edge.
; Keep that common wrapper inert while a diagnostic drive mode owns the pins.
; In autonomous ePWM-train mode, clock one countdown only on the VCO2 call so
; the wrapper runs exactly once per control tick, then use the proven one-shot
; path for the retained reset-output index.
diagnostic_enable_capture_event1_interrupt:
        MOVL XAR6,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR6[DIAGNOSTIC_RESET_DRIVE_MODE_OFFSET]
        SB diagnostic_capture_rearm_stock,EQ
        CMPB AL,#DIAGNOSTIC_RESET_DRIVE_EPWM_TRAIN
        SB diagnostic_capture_rearm_done,NEQ
        MOVW DP,#VCO_SUBDIVISION_STATE_PTR_DP
        MOVL XAR6,@VCO2_SUBDIVISION_STATE_PTR_DIRECT
        MOVL ACC,@XAR4
        CMPL ACC,@XAR6
        SB diagnostic_capture_rearm_done,NEQ
        MOVL XAR6,#DIAGNOSTIC_RESET_TRAIN_STATE_BASE
        MOV AL,*+XAR6[DIAGNOSTIC_RESET_TRAIN_PERIOD_TICKS_OFFSET]
        SB diagnostic_capture_rearm_done,EQ
        MOV AL,*+XAR6[DIAGNOSTIC_RESET_TRAIN_COUNTDOWN_OFFSET]
        ADDB AL,#-0x01
        MOV *+XAR6[DIAGNOSTIC_RESET_TRAIN_COUNTDOWN_OFFSET],AL
        SB diagnostic_capture_rearm_done,NEQ
        MOV AL,*+XAR6[DIAGNOSTIC_RESET_TRAIN_PERIOD_TICKS_OFFSET]
        MOV *+XAR6[DIAGNOSTIC_RESET_TRAIN_COUNTDOWN_OFFSET],AL
        LCR diagnostic_trigger_selected_epwm_reset_pulse
        SB diagnostic_capture_rearm_done,UNC
diagnostic_capture_rearm_stock:
        LCR enable_ecap_event1_interrupt
diagnostic_capture_rearm_done:
        LRETR

; AQSFRC selects immediate loading; AQCSFRC then forces both A/B pins of the
; VCO1 and VCO2 reset-PWM pairs low/high, or releases the force when AL is zero.
diagnostic_force_suboscillator_reset_outputs:
        MOVZ AR6,@AL
        MOVL XAR4,#EPWM1_BASE
        MOVB XAR0,#EPWM_O_AQSFRC
        OR *+XAR4[AR0],#EPWM_AQSFRC_RELOAD_IMMEDIATE
        MOVB XAR0,#EPWM_O_AQCSFRC
        MOV *+XAR4[AR0],AR6
        MOVL XAR4,#EPWM2_BASE
        MOVB XAR0,#EPWM_O_AQSFRC
        OR *+XAR4[AR0],#EPWM_AQSFRC_RELOAD_IMMEDIATE
        MOVB XAR0,#EPWM_O_AQCSFRC
        MOV *+XAR4[AR0],AR6
        LRETR

; Clear only the selected output's generic override latch.  The next stock
; 10-kHz control pass republishes its normal value.
diagnostic_clear_selected_output_override:
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_SELECTED_SLOT_OFFSET]
        CMPB AL,#0x00
        SB diagnostic_clear_vco1,EQ
        CMPB AL,#0x01
        SB diagnostic_clear_vco1_sub1,EQ
        CMPB AL,#0x02
        SB diagnostic_clear_vco1_sub2,EQ
        CMPB AL,#0x03
        SB diagnostic_clear_vco2,EQ
        CMPB AL,#0x04
        SB diagnostic_clear_vco2_sub1,EQ
        MOVL XAR0,#ENGINE_VCO2_SUB2_OUTPUT_OFFSET+CONTROL_OUTPUT_OVERRIDE_OFFSET
        SB diagnostic_clear_output_ready,UNC
diagnostic_clear_vco1:
        MOVL XAR0,#ENGINE_VCO1_PITCH_OUTPUT_OFFSET+CONTROL_OUTPUT_OVERRIDE_OFFSET
        SB diagnostic_clear_output_ready,UNC
diagnostic_clear_vco1_sub1:
        MOVL XAR0,#ENGINE_VCO1_SUB1_OUTPUT_OFFSET+CONTROL_OUTPUT_OVERRIDE_OFFSET
        SB diagnostic_clear_output_ready,UNC
diagnostic_clear_vco1_sub2:
        MOVL XAR0,#ENGINE_VCO1_SUB2_OUTPUT_OFFSET+CONTROL_OUTPUT_OVERRIDE_OFFSET
        SB diagnostic_clear_output_ready,UNC
diagnostic_clear_vco2:
        MOVL XAR0,#ENGINE_VCO2_PITCH_OUTPUT_OFFSET+CONTROL_OUTPUT_OVERRIDE_OFFSET
        SB diagnostic_clear_output_ready,UNC
diagnostic_clear_vco2_sub1:
        MOVL XAR0,#ENGINE_VCO2_SUB1_OUTPUT_OFFSET+CONTROL_OUTPUT_OVERRIDE_OFFSET
diagnostic_clear_output_ready:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        MOV *+XAR4[AR0],#0x0000
        LRETR

; Convert the selected U14 value to [0,1) and install it through the same
; bounded generic control-output leaf used by the stock panel diagnostics.
diagnostic_apply_selected_output_override:
        MOVL XAR7,#DIAGNOSTIC_STATE_PRIMARY_BASE
        MOV AL,*+XAR7[DIAGNOSTIC_OUTPUT_OVERRIDE_ENABLE_OFFSET]
        SB diagnostic_apply_output_done,EQ
        MOV AL,*+XAR7[DIAGNOSTIC_SELECTED_SLOT_OFFSET]
        CMPB AL,#0x00
        SB diagnostic_apply_vco1,EQ
        CMPB AL,#0x01
        SB diagnostic_apply_vco1_sub1,EQ
        CMPB AL,#0x02
        SB diagnostic_apply_vco1_sub2,EQ
        CMPB AL,#0x03
        SB diagnostic_apply_vco2,EQ
        CMPB AL,#0x04
        SB diagnostic_apply_vco2_sub1,EQ
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCO2_SUB2_OUTPUT_OFFSET
        SB diagnostic_apply_output_ready,UNC
diagnostic_apply_vco1:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCO1_PITCH_OUTPUT_OFFSET
        SB diagnostic_apply_output_ready,UNC
diagnostic_apply_vco1_sub1:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCO1_SUB1_OUTPUT_OFFSET
        SB diagnostic_apply_output_ready,UNC
diagnostic_apply_vco1_sub2:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCO1_SUB2_OUTPUT_OFFSET
        SB diagnostic_apply_output_ready,UNC
diagnostic_apply_vco2:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCO2_PITCH_OUTPUT_OFFSET
        SB diagnostic_apply_output_ready,UNC
diagnostic_apply_vco2_sub1:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCO2_SUB1_OUTPUT_OFFSET
diagnostic_apply_output_ready:
        UI16TOF32 R0H,*+XAR7[DIAGNOSTIC_RAW_TARGET_U14_OFFSET]
        NOP
        MPYF32 R0H,#CONTROL_14BIT_TO_UNIT_F32_HI,R0H
        NOP
        NOP
        MOVB AL,#0x01
        LCR control_output_set_float
diagnostic_apply_output_done:
        LRETR
diagnostic_extension_code_end:
        ; Pad the diagnostic extension to a fixed 0x200-word span.  The stock
        ; image has erased flash throughout this range.
        .loop 0x0076
        .word 0xffff
        .endloop
        .include "src/features_bridge.asm"
        .include "src/features_rhythm_bridge.asm"
