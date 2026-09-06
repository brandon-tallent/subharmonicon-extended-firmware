; Four-lane rhythm variant, using the stock clock, panel and envelope engines.
        .sect ".feature_bridge"
        .retain
        .ref _feature_rhythm_tick

; Replace the complete integer rhythm transaction. Preserve the floating
; registers and address registers that stock callers keep across this leaf.
; C preserves XAR1..3. AL remains the return value; ACC/ST0 are caller-clobbered.
feature_rhythm_tick_hook:
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
        LCR _feature_rhythm_tick
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
        LRETR

; Entered by LB after the tick, with the original control-loop stack intact.
; R3/R4's routed flags occupy former divider counters +0xb/+0xc. Clear only
; the gate request after arming an attack. Calling envelope_set_gate(0) here
; cancels ATTACK before its first tick has asserted ACTIVE. A cleared request
; instead lets that first tick run and completes the AD even if PLAY stops.
feature_rhythm_clock_dispatch:
        MOVL XAR0,#ENGINE_TRANSPORT_RUNNING_OFFSET
        MOV AL,*+XAR2[AR0]
        SB feature_rhythm_dispatch_done,EQ
        MOVL XAR0,#ENGINE_SEQ1_ENVELOPE_TRIGGER_ENABLE_OFFSET
        MOV AL,*+XAR2[AR0]
        SB feature_rhythm_trigger_output,EQ
        MOVL XAR0,#ENGINE_RHYTHM_ENGINE_OFFSET+0x0b
        MOV AL,*+XAR2[AR0]
        MOVL XAR4,*-SP[0x20]
        LCR envelope_set_gate
        MOVB XAR0,#ENVELOPE_GATE_REQUEST_OFFSET
        MOV *+XAR4[AR0],#0
        MOVL XAR0,#ENGINE_RHYTHM_ENGINE_OFFSET+0x0c
        MOV AL,*+XAR2[AR0]
        MOVL XAR4,*-SP[0x22]
        LCR envelope_set_gate
        MOVB XAR0,#ENVELOPE_GATE_REQUEST_OFFSET
        MOV *+XAR4[AR0],#0
feature_rhythm_trigger_output:
        MOVL XAR0,#ENGINE_RHYTHM_ENGINE_OFFSET+0x0b
        MOV AL,*+XAR2[AR0]
        SB feature_rhythm_dispatch_done,EQ
        LB emit_trigger_output_pulse
feature_rhythm_dispatch_done:
        LB emit_clock_and_sequencer_pulses

; Restore only the four routing words from the stock marker-guarded save.
; Playback/phase remain at initialize_rhythm_engine_state's clean defaults.
; Both reads must succeed, and every route is validated before publication.
; The stock foreground writer continues to save changes about every 5 seconds.
feature_rhythm_restore_routes:
        MOVL *SP++,XAR1
        MOVL *SP++,XAR2
        ADDB SP,#8
        MOVL XAR2,#NVRAM_SERVICE_SINGLETON
        MOVZ AR6,@SP
        SUBB XAR6,#3
        MOVL *-SP[2],XAR6
        MOVL XAR4,*+XAR2[0]
        MOVB XAR1,#PERSISTENCE_LOAD_CALLBACK_OFFSET
        MOVB XAR0,#PERSISTENCE_LOAD_CONTEXT_OFFSET
        MOVL XAR7,*+XAR4[AR1]
        MOVL ACC,XAR2
        SETC SXM
        ADD ACC,*+XAR4[AR0]
        MOVL XAR4,ACC
        MOVL XAR5,#NVRAM_RHYTHM_FORMAT_RECORD
        MOVB AL,#1
        LCR *XAR7
        CMPB AL,#1
        SB feature_rhythm_routes_default,NEQ
        MOV AL,*-SP[3]
        CMP AL,#NVRAM_RHYTHM_FORMAT_MARKER
        SB feature_rhythm_routes_default,NEQ
        MOVZ AR6,@SP
        SUBB XAR6,#8
        MOVL *-SP[2],XAR6
        MOVL XAR4,*+XAR2[0]
        MOVB XAR1,#PERSISTENCE_LOAD_CALLBACK_OFFSET
        MOVB XAR0,#PERSISTENCE_LOAD_CONTEXT_OFFSET
        MOVL XAR7,*+XAR4[AR1]
        MOVL ACC,XAR2
        SETC SXM
        ADD ACC,*+XAR4[AR0]
        MOVL XAR4,ACC
        MOVL XAR5,#NVRAM_RHYTHM_STATE_RECORD+RHYTHM_ENGINE_ASSIGN1_OFFSET
        MOVB AL,#4
        LCR *XAR7
        CMPB AL,#1
        SB feature_rhythm_routes_default,NEQ
        MOVZ AR7,@SP
        SUBB XAR7,#8
        MOV AL,*+XAR7[0]
        CMPB AL,#3
        SB feature_rhythm_routes_default,HI
        MOV AL,*+XAR7[1]
        CMPB AL,#3
        SB feature_rhythm_routes_default,HI
        MOV AL,*+XAR7[2]
        CMPB AL,#3
        SB feature_rhythm_routes_default,HI
        MOV AL,*+XAR7[3]
        CMPB AL,#3
        SB feature_rhythm_routes_default,HI
        MOVL XAR6,#RHYTHM_ASSIGNMENT_UI_BASE
        MOV AL,*+XAR7[0]
        MOV *+XAR6[0],AL
        MOV AL,*+XAR7[1]
        MOV *+XAR6[1],AL
        MOV AL,*+XAR7[2]
        MOV *+XAR6[2],AL
        MOV AL,*+XAR7[3]
        MOV *+XAR6[3],AL
        MOVB AL,#1
        SB feature_rhythm_routes_done,UNC
feature_rhythm_routes_default:
        MOVL XAR7,#RHYTHM_ASSIGNMENT_UI_BASE
        MOV *+XAR7[0],#1
        MOV *+XAR7[1],#2
        MOV *+XAR7[2],#1
        MOV *+XAR7[3],#2
        MOVB AL,#0
feature_rhythm_routes_done:
        ; Seed the save comparison with the actual boot routing. Otherwise
        ; an all-off edit before the first save could match zeroed RAM while
        ; EEPROM still contains the previously enabled assignments.
        MOV AH,AL
        MOVL XAR7,#RHYTHM_ASSIGNMENT_UI_BASE
        MOVL XAR6,#CONTROL_STATE_SINGLETON+CONTROL_STATE_RHYTHM_SNAPSHOT_OFFSET+RHYTHM_ENGINE_ASSIGN1_OFFSET
        MOV AL,*+XAR7[0]
        MOV *+XAR6[0],AL
        MOV AL,*+XAR7[1]
        MOV *+XAR6[1],AL
        MOV AL,*+XAR7[2]
        MOV *+XAR6[2],AL
        MOV AL,*+XAR7[3]
        MOV *+XAR6[3],AL
        MOV AL,AH
        SUBB SP,#8
        MOVL XAR2,*--SP
        MOVL XAR1,*--SP
        LRETR

; The finer pattern clock must not change the existing sixteenth CLOCK OUT.
; XAR4 is the GPIO object. Restore the pulse duration after testing the flag.
feature_rhythm_clock_output:
        MOVL XAR0,#ENGINE_RHYTHM_ENGINE_OFFSET+0x0d
        MOV AH,*+XAR2[AR0]
        CMPB AH,#0
        SB feature_rhythm_clock_output_done,EQ
        MOVB ACC,#RHYTHM_OUTPUT_PULSE_MS
        LB trigger_timed_gpio_pulse_ms
feature_rhythm_clock_output_done:
        LRETR

; XAR4/XAR5 point at the saved/live rhythm blocks. Only assignments are
; persistent settings; phase, event flags and the adjacent vtable must not
; force EEPROM writes while performing. Keep the stock incremental writer.
feature_rhythm_routes_changed:
        ADDB XAR4,#RHYTHM_ENGINE_ASSIGN1_OFFSET
        ADDB XAR5,#RHYTHM_ENGINE_ASSIGN1_OFFSET
        MOVB ACC,#4
        LB rts_memcmp

        .ref _feature_rhythm_knob
feature_rhythm_knob0_hook:
        LCR read_discrete_control_value
        MOV AH,AL
        MOVB AL,#0
        LB feature_rhythm_knob_bridge
feature_rhythm_knob1_hook:
        LCR read_discrete_control_value
        MOV AH,AL
        MOVB AL,#1
        LB feature_rhythm_knob_bridge
feature_rhythm_knob2_hook:
        LCR read_discrete_control_value
        MOV AH,AL
        MOVB AL,#2
        LB feature_rhythm_knob_bridge
feature_rhythm_knob3_hook:
        LCR read_discrete_control_value
        MOV AH,AL
        MOVB AL,#3
        LB feature_rhythm_knob_bridge
feature_rhythm_knob_bridge:
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
        LCR _feature_rhythm_knob
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
        LRETR
