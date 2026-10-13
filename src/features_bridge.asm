; Fixed-image hooks and the explicit C/stock ABI boundary.
        .sect ".feature_bridge"
        .retain
        .ref _feature_init, _feature_tick, _feature_sysex, _feature_state
        .global _feature_output, _feature_parent_cv, _feature_stock_refresh
        .global _feature_enable_timer_irq, _feature_disable_timer_irq
feature_startup_hook:
        LCR platform_startup
        LCR _feature_init
        LRETR
        .ref _feature_noise_only
feature_pitch_hook:
        ; Independent voices own all six pitch outputs and reset periods.
        ; Stock calculations would only reach overridden bindings, but still
        ; consume the control ISR's budget (especially with SEQ assignments).
        MOVL XAR7,#_feature_state
        MOV AH,*+XAR7[0]
        SB feature_pitch_stock,EQ
        MOVL XAR7,#_feature_noise_only
        MOV AH,*+XAR7[0]
        SB feature_pitch_stock,NEQ
        MOVL XAR0,#ENGINE_CONTROL_UPDATE_INHIBIT
        MOV AH,*+XAR4[AR0]
        SB feature_pitch_stock,NEQ
        LB _feature_tick
feature_pitch_stock:
        LCR update_pitch_control_outputs
        LCR _feature_tick
        LRETR
        .ref _feature_mono_glide
feature_mono_vco1_output_hook:
        MOVB AL,#0
        SB feature_mono_output_hook,UNC
feature_mono_vco2_output_hook:
        MOVB AL,#3
feature_mono_output_hook:
        MOVL *SP++,XAR4
        LCR _feature_mono_glide
        MOVL XAR4,*--SP
        MOVB AL,#0
        LCR control_output_set_float
        LRETR
feature_capture_hook:
        MOVL XAR7,#_feature_state
        MOV AL,*+XAR7[0]
        SB feature_capture_stock,EQ
        MOVL XAR7,#_feature_noise_only
        MOV AL,*+XAR7[0]
        SB feature_capture_return,EQ
feature_capture_stock:
        LCR tick_vco_suboscillator_reset_pulses
feature_capture_return:
        LRETR
feature_sysex_hook:
        MOVL XAR4,@XAR2
        LCR _feature_sysex
        CMPB AL,#0
        SB feature_sysex_return,NEQ
        LCR diagnostic_sysex_command
feature_sysex_return:
        LRETR
_feature_output:
        MOVB AL,#1
        LCR control_output_set_float
        LRETR
; C ABI: AL=parent index, R0H=quantized base, R1H=continuous fine/bend.
; The stock mapper folds the base for eight-note scales before adding R1H.
_feature_parent_cv:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        ZERO R2H
        ZERO R3H
        LCR map_pitch_sum_to_quantized_cv
        LRETR
_feature_stock_refresh:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        MOVB AL,#1
        LCR update_pitch_control_outputs
        LRETR
_feature_enable_timer_irq:
        OR IER,#0x2000
        LRETR
_feature_disable_timer_irq:
        AND IER,#0xdfff
        LRETR

        .ref _feature_note_added, _feature_note_deleted
        .ref _feature_note_removed, _feature_notes_reset
        .ref _feature_velocity_selected
        .ref _feature_midi_vcf_note, _feature_midi_vcf
        .global _feature_clear_notes, _feature_gate_on, _feature_gate_off
; Source admission is complete, but the stock channel filter has not run.
; An adjacent-channel VCF note is consumed without changing pitch or the
; stock held-note manager. Other messages resume the exact stock filter.
feature_note_channel_hook:
        MOVL *SP++,XAR4
        MOVL XAR4,@XAR2
        LCR _feature_midi_vcf_note
        MOVL XAR4,*--SP
        CMPB AL,#0
        SB feature_note_channel_stock,EQ
        LB midi_note_dispatch_accepted
feature_note_channel_stock:
        MOVW DP,#MIDI_INPUT_CONFIG_DP
        MOVB XAR0,#MIDI_MESSAGE_STATUS_OFFSET
        LB feature_note_filter_continue
; The C allocation flag is the second word; AH still carries MIDI velocity
; at the routing seam. Do not use an ACC-wide load here.
feature_note_route_hook:
        MOVL XAR7,#_feature_state
        MOV AL,*+XAR7[1]
        SB feature_note_route_allocated,NEQ
        MOVW DP,#RHYTHM_ENGINE_STATE_DP
        MOV AL,@RHYTHM_ENGINE_RUNNING_DIRECT
        SB feature_note_route_stopped,EQ
        LB feature_note_running_continue
feature_note_route_stopped:
        LB midi_note_stopped_transport
feature_note_route_allocated:
        MOVL XAR4,@XAR3
        MOV AL,@AR6
        MOVB XAR5,#0
        LCR midi_held_note_insert
        LB midi_note_dispatch_accepted

feature_note_added_hook:
        MOVL *SP++,XAR4
        MOVL XAR4,@XAR3
        LCR _feature_note_added
        MOVL XAR4,*--SP
        CMPB AL,#0
        SB feature_note_callback_done,NEQ
        LCR midi_note_reselect
        LCR _feature_velocity_selected
feature_note_callback_done:
        LRETR
feature_note_removed_hook:
        MOVL *SP++,XAR4
        LCR _feature_note_removed
        MOVL XAR4,*--SP
        CMPB AL,#0
        SB feature_note_callback_done,NEQ
        LCR midi_note_reselect
        LCR _feature_velocity_selected
        LRETR
feature_note_delete_hook:
        MOVL *SP++,XAR4
        LCR _feature_note_deleted
        MOVL XAR4,*--SP
        LB cpp_operator_delete
; Continue after the first removal only in allocator mode. This preserves
; the stock frame and final token cleanup without writing into its stack.
feature_remove_continue_hook:
        MOVB XAR1,#1
        MOVL XAR7,#_feature_state
        MOV AL,*+XAR7[1]
        SB feature_remove_one,NEQ
        LB midi_held_note_remove_test
feature_remove_one:
        LB feature_remove_finish
feature_manager_reset_hook:
        MOVL *SP++,XAR4
        LCR _feature_notes_reset
        MOVL XAR4,*--SP
        MOVL *SP++,XAR1
        ADDB SP,#6
        LB feature_manager_reset_continue
; A matching Note Off owns the running-transport transpose latch even though
; stock has no held-note node for it. Preserve the replaced XAR4 setup.
feature_note_off_hook:
        MOVW DP,#MIDI_CONTROL_SOURCE_DP
        MOV AL,@MIDI_CONTROL_SOURCE_NOTE_DIRECT
        CMP AL,@AR6
        SB feature_note_off_preserve_latch,NEQ
        MOV @MIDI_CONTROL_SOURCE_NOTE_DIRECT,#0
feature_note_off_preserve_latch:
        MOVL XAR4,@XAR3
        MOV AL,@AR6
        LRETR
; Transport Stop terminates the lifetime of its transpose source. Preserve the
; replaced DP setup for the remaining stock transport-input checks.
feature_midi_stop_hook:
        MOVW DP,#MIDI_CONTROL_SOURCE_DP
        MOV @MIDI_CONTROL_SOURCE_NOTE_DIRECT,#0
        MOVW DP,#MIDI_INPUT_CONFIG_DP
        LRETR
; Keep the transport-time direct-note latch consistent with the held-note
; manager cleared by MIDI All Sound Off / All Notes Off.
feature_all_notes_off_hook:
        MOVW DP,#MIDI_CONTROL_SOURCE_DP
        MOV @MIDI_CONTROL_SOURCE_NOTE_DIRECT,#0
        LCR _feature_close_gates
        LRETR
_feature_clear_notes:
        MOVL XAR4,#MIDI_NOTE_MANAGER_SINGLETON
        LCR reset_midi_note_manager_state
        LRETR
_feature_gate_on:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        LCR feature_midi_gate_on
        LRETR
_feature_gate_off:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        LCR feature_midi_gate_off
        LRETR

feature_midi_gate_on:
        MOVL XAR0,#_feature_midi_vcf
        MOV AL,*+XAR0[0]
        SB feature_midi_gate_on_coupled,EQ
        MOVW DP,#MIDI_INPUT_CONFIG_DP
        MOV AL,@MIDI_INPUT_CHANNEL_DIRECT
        CMPB AL,#MIDI_INPUT_CHANNEL_OMNI
        SB feature_midi_vca_on,NEQ
feature_midi_gate_on_coupled:
        LCR trigger_both_envelopes
        LRETR
feature_midi_vca_on:
        MOVL XAR0,#ENGINE_CONTROL_UPDATE_INHIBIT
        MOV AL,*+XAR4[AR0]
        SB feature_midi_vca_on_done,NEQ
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCA_ENVELOPE_OFFSET
        MOVB AL,#1
        LCR envelope_set_gate
feature_midi_vca_on_done:
        LRETR
feature_midi_gate_off:
        MOVL XAR0,#_feature_midi_vcf
        MOV AL,*+XAR0[0]
        SB feature_midi_gate_off_coupled,EQ
        MOVW DP,#MIDI_INPUT_CONFIG_DP
        MOV AL,@MIDI_INPUT_CHANNEL_DIRECT
        CMPB AL,#MIDI_INPUT_CHANNEL_OMNI
        SB feature_midi_vca_off,NEQ
feature_midi_gate_off_coupled:
        LCR release_both_envelopes
        LRETR
feature_midi_vca_off:
        MOVW DP,#CONTROL_ENVELOPE_HELD_MODE_DP
        MOV AL,@CONTROL_ENVELOPE_HELD_MODE_DIRECT
        MOVL XAR0,#ENGINE_CONTROL_UPDATE_INHIBIT
        OR AL,*+XAR4[AR0]
        SB feature_midi_vca_off_done,NEQ
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCA_ENVELOPE_OFFSET
        MOVB AL,#0
        LCR envelope_set_gate
feature_midi_vca_off_done:
        LRETR
        .global _feature_vcf_gate_on, _feature_vcf_gate_off
_feature_vcf_gate_on:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        MOVL XAR0,#ENGINE_CONTROL_UPDATE_INHIBIT
        MOV AL,*+XAR4[AR0]
        SB feature_vcf_gate_done,NEQ
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCF_ENVELOPE_OFFSET
        MOVB AL,#1
        LCR envelope_set_gate
feature_vcf_gate_done:
        LRETR
_feature_vcf_gate_off:
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON
        MOVW DP,#CONTROL_ENVELOPE_HELD_MODE_DP
        MOV AL,@CONTROL_ENVELOPE_HELD_MODE_DIRECT
        MOVL XAR0,#ENGINE_CONTROL_UPDATE_INHIBIT
        OR AL,*+XAR4[AR0]
        SB feature_vcf_gate_off_done,NEQ
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCF_ENVELOPE_OFFSET
        MOVB AL,#0
        LCR envelope_set_gate
feature_vcf_gate_off_done:
        LRETR

        .ref _feature_service, _feature_assignment_edge
        .global _feature_micros
feature_foreground_hook:
        MOVL *SP++,XAR4
        LCR _feature_service
        MOVL XAR4,*--SP
        LCR dequeue_midi_message_record
        LRETR
feature_sub1_button1_hook:
        LCR poll_digital_control_activation_edge
        MOV AH,@AL
        MOVB AL,#2
        LCR _feature_assignment_edge
        LRETR
feature_sub2_button1_hook:
        LCR poll_digital_control_activation_edge
        MOV AH,@AL
        MOVB AL,#0
        LCR _feature_assignment_edge
        LRETR
_feature_micros:
        MOVL XAR4,#PERIODIC_SCHEDULER_SINGLETON
        LCR periodic_scheduler_now_us
        LRETR
        .ref _feature_config_load, _feature_persistent_sysex, _feature_cc
        .global _feature_save_config
feature_parameter_hook:
        CMPB AL,#3
        SB feature_parameter_extension,HIS
        LB sysex_set_parameter
feature_parameter_extension:
        MOVL XAR4,@XAR2
        LCR _feature_persistent_sysex
        LRETR
feature_config_load_hook:
        LCR initialize_control_state_from_persistence
        MOVL *SP++,XAR4
        LCR _feature_config_load
        MOVL XAR4,*--SP
        LRETR
feature_config_defaults_hook:
        LCR initialize_control_state_defaults
        MOVL *SP++,XAR4
        LCR _feature_config_load
        MOVL XAR4,*--SP
        LRETR
_feature_save_config:
        MOVL XAR4,#MIDI_DISPATCH_CONTEXT_SINGLETON
        MOVB AL,#0
        LCR propagate_control_state_changes
        LRETR
feature_cc_hook:
        CMPB AL,#117
        SB feature_cc_extension,EQ
        CMPB AL,#80
        SB feature_cc_outside_routing,LO
        CMPB AL,#89
        SB feature_cc_extension,LOS
feature_cc_outside_routing:
        CMPB AL,#114
        SB feature_cc_extension,EQ
        CMPB AL,#115
        SB feature_cc_extension,EQ
        CMPB AL,#111
        SB feature_cc_extension,EQ
        CMPB AL,#65
        SB feature_cc_extension,EQ
        CMPB AL,#5
        SB feature_cc_extension,EQ
        CMPB AL,#37
        SB feature_cc_extension,EQ
        CMPB AL,#112
        SB feature_cc_extension,EQ
        CMPB AL,#116
        SB feature_cc_extension,EQ
        CMPB AL,#MIDI_CC_LEGATO_FOOTSWITCH
        SB feature_cc_above_68,GT
        LB feature_cc_low_continue
feature_cc_above_68:
        LB midi_cc_dispatch_above_68
feature_cc_extension:
        LCR _feature_cc
        LB midi_cc_accepted

; A mode boundary closes software gates even when panel Held is selected.
; Ordinary Note Off continues to use the stock Held-aware release helper.
        .global _feature_close_gates
_feature_close_gates:
        MOVL *SP++,XAR1
        MOVL XAR1,#SYNTH_ENGINE_SINGLETON
        MOVL XAR0,#ENGINE_CONTROL_TICK_ACCESS_GATE_OFFSET
        MOV *+XAR1[AR0],#0
        ; Cancel queued panel/feature gate transitions too. An All Notes/Sound
        ; Off arriving before a restored Held request is consumed must win.
        MOVL XAR0,#ENGINE_TRIGGER_ON_PENDING_OFFSET
        MOV *+XAR1[AR0],#0
        MOVL XAR0,#ENGINE_TRIGGER_OFF_PENDING_OFFSET
        MOV *+XAR1[AR0],#0
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCA_ENVELOPE_OFFSET
        MOVB AL,#0
        LCR envelope_set_gate
        MOVL XAR4,#SYNTH_ENGINE_SINGLETON+ENGINE_VCF_ENVELOPE_OFFSET
        MOVB AL,#0
        LCR envelope_set_gate
        MOVL XAR0,#ENGINE_CONTROL_TICK_ACCESS_GATE_OFFSET
        MOVB *+XAR1[AR0],#1,UNC
        MOVL XAR1,*--SP
        LRETR

; TI SPRZ439: ADC interrupt flag clearing can collide with the next EOC.
; AL=ADCINT2 mask, AH=PIE group-10 ack. Preserve AH and all address registers.
; INT2CONT is also enabled at ADC initialization so a lost scan cannot latch
; the twelve multiplexed panel pots permanently. Stock image stays unchanged.
feature_adc_clear_interrupt_hook:
        MOV *(0:ADCA_INTFLGCLR),@AL
        MOV @AL,*(0:0x7405) ; ADCINTOVF
        TBIT AL,#1
        SB feature_adc_clear_interrupt_done,NTC
        MOVB AL,#2
        MOV *(0:ADCA_INTFLGCLR),@AL
        MOV *(0:0x7406),@AL ; ADCINTOVFCLR
feature_adc_clear_interrupt_done:
        ; Preserve the caller's complete ACC (AH is its PIE acknowledgement).
        MOVL *SP++,ACC
        MOVL *SP++,XAR4
        MOVL XAR4,#_feature_adc_scan
        MOV32 @ACC,*(0:0x5300)
        ADD ACC,#2500
        MOVL *+XAR4[0],ACC
        MOV *+XAR4[2],#0
        MOVL XAR4,*--SP
        MOVL ACC,*--SP
        LRETR

        .ref _feature_adc_scan
; Called with interrupts masked from the CLA-completion CPU ISR. TC means
; skip this request; DP must then already select its PIE acknowledgement.
; Otherwise replay the exact displaced SOC0 read/OR before stock launches it.
feature_adc_scan_gate_hook:
        MOVL *SP++,XAR4
        MOVL XAR4,#_feature_state
        MOV AL,*+XAR4[0]
        SB feature_adc_scan_permit,EQ
        MOVL XAR4,#_feature_adc_scan
        MOV AL,*+XAR4[2]
        SB feature_adc_scan_skip,NEQ
        MOV32 @ACC,*(0:0x5300)
        SUBL ACC,*+XAR4[0]
        SB feature_adc_scan_skip,LT
        MOV *+XAR4[2],#1
feature_adc_scan_permit:
        MOV @AL,*(0:ADCA_SOCFRC1)
        ORB AL,#1
        CLRC TC
        MOVL XAR4,*--SP
        LRETR
feature_adc_scan_skip:
        INC *+XAR4[3]
        MOVW DP,#PIECTRL_REGS_DP
        SETC TC
        MOVL XAR4,*--SP
        LRETR

        .ref _feature_vca_velocity
feature_vca_velocity_hook:
        MOVL *SP++,XAR4
        LCR _feature_vca_velocity
        MOVL XAR4,*--SP
        MOVB AL,#0
        LCR control_output_set_float
        LRETR

        .ref _feature_settings_gate
        .global _feature_setting_led, _feature_refresh_panel_references
feature_settings_guard_hook:
        LCR _feature_settings_gate
        CMPB AL,#0
        LRETR
_feature_setting_led:
        MOVB XAR0,#0x0a
        MOV *+XAR4[AR0],#0
        MOVB XAR0,#0x0b
        MOV *+XAR4[AR0],#0
        MOVB XAR0,#0x0e
        MOV *+XAR4[AR0],#0
        MOVB XAR0,#0x0f
        MOV *+XAR4[AR0],#0
        CMPB AL,#0
        SB feature_setting_led_on,NEQ
        LB drive_expander_signal_configured_active_level
feature_setting_led_on:
        LB drive_expander_signal_configured_inactive_level
_feature_refresh_panel_references:
        MOVL XAR4,#CONTROL_STATE_SINGLETON
        MOVB AL,#0
        LB refresh_all_panel_analog_control_references

        .global _feature_setting_button_edge
_feature_setting_button_edge:
        MOVB AL,#0
        LB poll_digital_control_activation_edge


; Reset private divider phases after stock resets both sequence positions.
        .ref _feature_rhythm_reset
feature_rhythm_reset_hook:
        MOVL XAR1,*--SP
        LCR feature_rhythm_reset_bridge
        LRETR
feature_rhythm_reset_bridge:
        MOVL *SP++,ACC
        MOVL *SP++,P
        MOVL *SP++,XT
        MOVL *SP++,XAR0
        MOVL *SP++,XAR4
        MOVL *SP++,XAR5
        MOVL *SP++,XAR6
        MOVL *SP++,XAR7
        PUSH DP
        PUSH ST0
        MOV32 *SP++,R0H
        MOV32 *SP++,R1H
        MOV32 *SP++,R2H
        MOV32 *SP++,R3H
        MOVB AL,#1
        LCR _feature_rhythm_reset
        MOV32 R3H,*--SP
        MOV32 R2H,*--SP
        MOV32 R1H,*--SP
        MOV32 R0H,*--SP
        POP ST0
        POP DP
        MOVL XAR7,*--SP
        MOVL XAR6,*--SP
        MOVL XAR5,*--SP
        MOVL XAR4,*--SP
        MOVL XAR0,*--SP
        MOVL XT,*--SP
        MOVL P,*--SP
        MOVL ACC,*--SP
        LRETR

        .ref _feature_scheduler_advance
; Timer 1 has saved its caller-clobbered registers and still masks nesting.
; Keep the stock bootstrap/off-mode clock until eCAP5 and RAM code are ready.
feature_scheduler_epoch_hook:
        MOVL XAR7,#_feature_state
        MOV AL,*+XAR7[0]
        SB feature_scheduler_epoch_stock,EQ
        LB _feature_scheduler_advance
feature_scheduler_epoch_stock:
        MOVW DP,#PERIODIC_SCHEDULER_EPOCH_DP
        MOVL ACC,@PERIODIC_SCHEDULER_TICK_US_DIRECT
        ADDL @PERIODIC_SCHEDULER_EPOCH_US_DIRECT,ACC
        LRETR

        .ref _feature_settings
feature_step_led_guard:
        MOVL *-SP[0x6],XAR4
        MOVL *SP++,XAR0
        MOVL XAR0,#_feature_settings
        MOV AL,*+XAR0[2]
        SB feature_step_led_owned,NEQ
        MOV AL,@FRONT_PANEL_MODE_DIRECT
feature_step_led_owned:
        MOVL XAR0,*--SP
        CMPB AL,#0
        LRETR
