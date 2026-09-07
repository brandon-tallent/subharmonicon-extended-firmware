# Optional firmware: panel quick reference

The four rhythm knobs select 32 patterns spanning 64 sixteenth steps. 
The left pair advances pitches; the right pair triggers 
the shared VCA and VCF independently. 

Pitch modes cycle **Normal → Paraphonic → Harmony → Morph → Normal**. Normal uses
stock oscillator tuning, Paraphonic has six independent pitch memories, and
Harmony builds automatic scale-derived chords. Morph reshapes SUB1
reset patterns and their main-VCO PWM modulation. Rhythm type is independent
of pitch mode. The default is Gate Patterns in every mode.

## Four rhythm lanes

In **Gate Patterns**, default routing when no valid saved assignments exist:

| Knob | Role | Lit assignment button |
|---|---|---|
| RHYTHM 1 | SEQ 1 pitch steps | Upper (SEQ 1) |
| RHYTHM 2 | SEQ 2 pitch steps | Lower (SEQ 2) |
| RHYTHM 3 | VCA envelope hits | Upper (now VCA) |
| RHYTHM 4 | VCF envelope accents | Lower (now VCF) |

RHYTHM 1/2's upper/lower buttons still route to SEQ1/SEQ2. RHYTHM 3/4's
upper/lower buttons now route to VCA/VCF. Either pattern can feed both of its
pair's destinations or neither. Matching hits combine with OR; CC113's XOR
selection has no effect in this mode. Oscillator-assignment buttons keep their
pitch meaning. Pitch advances alone never trigger either envelope.

All patterns share a four-bar phrase (64 sixteenths), with a finer 24-PPQN
clock for MIDI synchronization. RESET returns the phrase and both pitch
sequences to their start; the first hit presents step 1 before advancing on
later hits. PLAY pauses/resumes the phrase. NEXT advances both pitch sequences
once without moving the phrase or triggering envelopes, even with silent patterns.
Holding RESET retains the stock selected-step override.
In independent-pitch mode the musical clock follows elapsed hardware time,
including time spent processing oscillator changes. Adding pitch assignments
must not change tempo. Hot pitch calculations run from the reserved fast RAM.

TEMPO spans 40–239 BPM (noon 140). Internal pattern ticks and MIDI sync run
at 24 PPQN. **Each physical CLOCK rising edge advances one sixteenth step**
in Gate Patterns (64 edges per phrase), or one divider count in Stock rhythm
mode. CLOCK OUT follows each accepted physical edge while playing; with
internal/MIDI clock it remains sixteenths (4 PPQN). SEQ1/2 CLK outputs follow their pitch
lanes; TRIGGER OUT follows VCA-pattern hits.
Manual TRIGGER, TRIGGER input, and MIDI keyboard gates still address both
envelopes. 

Pattern order, counterclockwise to clockwise. Numbers are zero-based selector
bins, not panel markings. `x` = hit, `.` = rest; each group is one beat, with
`/` separating bars within each two-bar half. Every hit lies on the sixteenth grid.

| Bin | Pattern | Bars 1–2 | Bars 3–4 |
|---:|---|---|---|
| 0 | Silence | `.... .... .... .... / .... .... .... ....` | `.... .... .... .... / .... .... .... ....` |
| 1 | Whole notes | `x... .... .... .... / x... .... .... ....` | `x... .... .... .... / x... .... .... ....` |
| 2 | Half notes | `x... .... x... .... / x... .... x... ....` | `x... .... x... .... / x... .... x... ....` |
| 3 | Quarter notes | `x... x... x... x... / x... x... x... x...` | `x... x... x... x... / x... x... x... x...` |
| 4 | Broken house | `x... ..x. x... ...x / x... ..x. ..x. .x..` | `x... ..x. x... ..x. / x... ..x. x.x. ...x` |
| 5 | Eighth notes | `x.x. x.x. x.x. x.x. / x.x. x.x. x.x. x.x.` | `x.x. x.x. x.x. x.x. / x.x. x.x. x.x. x.x.` |
| 6 | Funk break | `x..x x... ..x. x.x. / x... x..x .x.. x.xx` | `x..x x... .xx. x.x. / x... x..x ..x. xxxx` |
| 7 | Sixteenth notes | `xxxx xxxx xxxx xxxx / xxxx xxxx xxxx xxxx` | `xxxx xxxx xxxx xxxx / xxxx xxxx xxxx xxxx` |
| 8 | Jungle turnaround | `x.x. .x.. x..x .x.. / x..x .x.x ..x. xx.x` | `x.x. .x.. x..x .x.x / x..x .x.x x.x. xx.x` |
| 9 | Offbeat eighths | `..x. ..x. ..x. ..x. / ..x. ..x. ..x. ..x.` | `..x. ..x. ..x. ..x. / ..x. ..x. ..x. ..x.` |
| 10 | Backbeat | `.... x... .... x... / .... x... .... x...` | `.... x... .... x... / .... x... .... x...` |
| 11 | Disco pickup | `x... x... x... x... / x... x... x... x.x.` | `x... x... x... x... / x... x... x... x.xx` |
| 12 | House kick turnaround | `x... x... x... x... / x... x... x..x x..x` | `x... x... x... x.x. / x... x... x.x. x..x` |
| 13 | Electro kick | `x... ..x. ..x. .... / x..x ..x. .... x.x.` | `x... ..x. x.x. .... / x... .xx. .... x.xx` |
| 14 | Two-step garage | `x... x... ..x. x... / x... x..x ..x. x..x` | `x... x... ..x. x... / x... x..x ..x. x.x.` |
| 15 | Tresillo with turnaround | `x... ..x. .... x... / x... ..x. ...x ..x.` | `x... ..x. .... x... / x... ..x. ...x ..xx` |
| 16 | Euclidean 5/16 | `x... x..x ..x. .x.. / x... x..x ..x. .x..` | `x... x..x ..x. ..x. / x... x..x ..x. ..xx` |
| 17 | Euclidean 7/16 | `x..x .x.x ..x. x.x. / x..x .x.x ..x. x.x.` | `x..x .x.x ..x. x.x. / x..x .x.x ..x. xx.x` |
| 18 | Euclidean 9/32 | `x... x... x..x ...x / ..x. ..x. .x.. .x..` | `x... x... x..x ...x / ..x. ..x. .x.. x..x` |
| 19 | Euclidean 11/32 | `x..x ..x. .x.. x..x / ..x. .x.. x..x ..x.` | `x..x ..x. .x.. x..x / ..x. ..x. x..x ..xx` |
| 20 | Sparse bass | `x... ..x. .... x... / .... x... .... ....` | `x... ..x. .... x... / .... x... ..x. ....` |
| 21 | Rolling bass | `x... ..x. .... x... / .... x... ..x. x...` | `x... ..x. .... x... / x... x... ..x. x.x.` |
| 22 | Broken bass | `x... x.x. .... .... / .... x.x. .... x.x.` | `x... x.x. .... ..x. / .... x.x. .... x.xx` |
| 23 | Clustered bass | `x... ..x. x... ..x. / x... ..x. x... ....` | `x... ..x. x... ..x. / x... ..x. x... ..x.` |
| 24 | Busy bass | `x... x... x.x. .... / x.x. ..x. x... ....` | `x... x... x.x. .... / x.x. ..x. x... ..x.` |
| 25 | Flams | `.... .... x..x ..x. / .... .... x... xx..` | `.... .... x..x ..x. / .... .... x... x.xx` |
| 26 | Snare pickup | `.... .... x... x... / .... ..x. x... x.x.` | `.... .... x... x... / .... ..x. x... x.xx` |
| 27 | Backbeat pickup | `.... x... .... x... / .... x... .... x.x.` | `.... x... .... x... / .... x... .... x.xx` |
| 28 | Eighth hats with fill | `x.x. x.x. x.x. x.x. / x.x. x.x. x.x. xxxx` | `x.x. x.x. x.x. x.x. / x.x. x.x. xxxx xxxx` |
| 29 | Turnaround | `x... .... .... .... / x... .... x.x. x.x.` | `x... .... .... .... / x... .... x.x. x.xx` |
| 30 | Hat bursts bursts | `x.x. xxxx .... .... / x.x. xxxx x... ....` | `x.x. xxxx xxxx .... / x.x. xxxx xxxx x.x.` |
| 31 | Four-floor pickup | `x... x... x... x... / x... x... x.x. x...` | `x... x... x... x... / x... x... x... x.xx` |

Changing a pattern retains phrase position. Fully
counterclockwise silences a lane; fully clockwise now selects the final groove.

## Rhythm type and MIDI selection

Settings **SEQ 2 SUB1** toggles **Stock / Gate Patterns** with EG. NEXT selects
Gate Patterns; RESET selects Stock. CC115 values 0–63 select Stock, 64–127
select Gate Patterns. This setting applies in all four pitch modes.

Stock uses divisors 1–16 of the quarter-note clock. All four rhythms route to
SEQ1/SEQ2 via upper/lower assignments; SEQ1 events trigger both envelopes.
CC113 selects OR or the original exclusive-one rule (exactly one assigned hit).
Gate Patterns uses the separate pitch/envelope routing described above and OR.
Physical CLOCK preserves one-edge-per-divider-count behavior in Stock; Gate
Patterns advances one sixteenth per edge. CLOCK OUT follows physical edges.
Internal/MIDI clock uses 24 PPQN and publishes sixteenth CLOCK OUT pulses.
RESET clears both divider and pattern phase. Switching type retains phrase
position and the stored divider counters; use RESET for a fresh start.

**CC80–83** control R1–R4 in every mode and rhythm type. In Gate Patterns,
`bin = value >> 2`: values 0–3 select bin 0, 4–7 select bin 1, …, 124–127
select bin 31. In Stock, `divisor = 16 - (value >> 3)` before patch CV.
Moving the physical knob at least 16 ADC counts takes ownership back.
Stock bipolar CV still adds to the selected knob/MIDI value before clamping.
A CC changes only its lane's selection, preserving phase and assignments.
These selections are volatile; after reboot the physical knobs select rhythms.

## Optional pitch and MIDI settings

With PLAY stopped, hold **SEQ 1 SUB1 + SUB2 for one second**, then release.

| Page button | Feature |
|---|---|
| SEQ 1 OSC1 | Harmony spread, 0–127 |
| SEQ 1 SUB1 | MIDI legato glide |
| SEQ 1 SUB2 | MIDI velocity depth |
| SEQ 2 OSC2 | Pitch mode: Normal / Paraphonic / Harmony / Morph |
| SEQ 2 SUB1 | Rhythm type: Stock / Gate Patterns |
| SEQ 2 SUB2 | VCO2 SUB2 noise: off / on |
| QUANTIZE | Independent tuning span |

- **EG:** toggle the selected feature; spread/velocity recall their last nonzero
  amount, span alternates ±1/±2 octaves, pitch mode cycles all four modes.
- **NEXT / RESET:** increase/decrease value; on glide, adjust time.
- **SEQ OCT:** toggle fine/coarse adjustment within settings. Spread and velocity
  use steps 1/16. Glide coarse steps select approximately 10, 22, 50, 100,
  200, 500, 1000, or 2000 ms; fine steps adjust the 14-bit time by 128.
  Discrete choices use one step.
- **PLAY:** save and exit without starting playback; 30 seconds idle also exits.

The selected page blinks at 2 Hz (all four QUANTIZE LEDs for tuning span); SEQ1 step LEDs show a steady coarse value and
SEQ2 step LEDs stay off except on the glide page. Glide uses all eight LEDs
(SEQ1 then SEQ2) to show the nearest time preset; the default 22 ms lights two.
EG indicates whether glide is enabled, independently of its remembered time.
These times are RC time constants, not a fixed duration to reach the target.
On the mode page, zero/one/two/three LEDs mean Normal /
Paraphonic / Harmony / Morph. Settings owns these LEDs; clock refresh is suppressed and
old blink/timeout state is cleared. On exit both sequencers' displays return.
Entry assignments are restored. The both-SUB2 paraphonic shortcut is removed.
Stock both-OSC Fine Tune and both-SUB1 calibration remain.

All six pitches follow QUANTIZE in paraphonic mode. Each Note On allocates one
source; unallocated sources use a neutral middle-C anchor. Note Off snaps only
its still-owned source back to that anchor, preserving knob and sequencer offsets.
A stolen note's later Note Off leaves its replacement untouched. Entering
Paraphonic and All Notes/Sound Off restore all six anchors to neutral.
The shared VCA can make this return audible during the envelope release tail.
Noon is the allocated note, or middle C when free, with a small tuning deadband;
pitch knobs span ±1 or ±2 octaves. Sequence octave
range remains a separate control. MIDI glide applies to overlapping notes,
without envelope retrigger; separate articulations snap. Internal sequencer slide
links are not implemented. Velocity affects the shared VCA, hence the whole chord.

| Control | CC | Values |
|---|---:|---|
| Harmony spread | 112 | 0–127; zero to ±30 cents |
| R1 / R2 / R3 / R4 | 80 / 81 / 82 / 83 | 0–127; all modes |
| Tuning span | 116 | 0 default; 1–63 ±1, 64–127 ±2 octaves |
| Glide enable | 65 | 0–63 off, 64–127 on |
| Glide time | 5 / 37 | 14-bit MSB / LSB; zero selects nominal 303 timing |
| Velocity depth | 111 | 0–127; zero bypasses |
| Pitch mode | 114 | 0 Normal, 1 Paraphonic, 2 Harmony, 3 Morph; other values ignored |
| Rhythm type | 115 | 0–63 Stock, 64–127 Gate Patterns |
| VCO2 SUB2 noise | 117 | 0–63 off, 64–127 on |

CC changes are temporary; panel edits save on exit. Persistent SysEx is available
through `tools/feature_config.py`.

## VCO2 SUB2 noise

With PLAY stopped, hold **SEQ 1 SUB1 + SUB2 for one second**, release, then
select **SEQ 2 SUB2**. **EG** toggles noise; **NEXT** selects on and **RESET**
selects off. EG and all four SEQ1 step LEDs light when on. **PLAY** saves/exits.
Tuning span is now on **QUANTIZE** within settings; outside settings QUANTIZE
retains its normal pitch function.

Noise replaces VCO2 SUB2 in all four pitch modes. Mix it with the
existing VCO2 SUB2 level knob through the shared filter and envelopes. Its
noise rate is fixed, independent of notes, tuning, glide, and sequencer pitch.
The VCO2 SUB2 knob still selects Harmony voicing for the remaining chord voices.
Switching noise off restores the sub's current pitched role. The other five
voices retain the selected pitch mode; Normal retains their stock tuning.

CC117 changes the toggle temporarily. `tools/feature_config.py noise 0|1`
generates a persistent SysEx setting (ID10); `tools/feature_protocol.py
noise-enable 0|1` generates a temporary development setting (ID14).

## Morph

Enter settings while stopped, select **SEQ 2 OSC2**, press RESET until Normal,
then NEXT three times. **Three SEQ1 step LEDs** mean Morph. PLAY saves/exits.
CC114 = 3 selects it temporarily; `tools/feature_config.py mode morph` creates
the persistent setting.

- **VCO1 / VCO2 FREQ:** parent pitch, with Paraphonic tuning.
  Every MIDI Note On sets the pitch anchor for VCO1 and VCO2 together,
  preserving their individual tuning offsets. SUB2 modulation rates are
  independent of incoming notes; SUB1 retains its parent-following timing.
  The latest Note On remains the anchor after Note Off. Overlapping notes glide
  when enabled; separate articulations snap.
- **Each SUB1 FREQ:** continuous shape scan: even → alternating → Brake
  → Accelerate → double burst → triple burst → irregular →
  single cluster. CC103/105 match the knobs; whichever moves last owns shape.
- **Each SUB1 pitch:** its nominal reset rate follows its parent one octave
  down. Shapes and timing modulation introduce subharmonics and pitch shifts.
- **Each SUB2 FREQ:** independent Paraphonic pitch **and reset-timing modulation
  rate for its paired SUB1**. CC104/106 match these controls. Modulation stays
  active with the SUB2 mixer level down. Depth is fixed; no new setting.
- **Sequence assignments:** SUB1 assigns its sequence to shape. Step noon
  is neutral; either end adds/subtracts half the bank, clipping at its limits.
  Put SUB1 FREQ near noon to hear the whole sweep. This stays continuous
  regardless of pitch QUANTIZE or sequence octave range. Parent assignments
  transpose parent and SUB1; SUB2 assignments change modulation rate and pitch.
- **VCO2 SUB2 noise:** retains its original fixed noise. When enabled, it
  bypasses periodic timing modulation of VCO2 SUB1; shape and sequence still work.

**Clock behavior:** while PLAY runs, the two subs in each VCO group restart
together on each bar downbeat. Between bars, the reset after the longest interval in the unmodulated SUB1
shape is pulled to a nearby sixteenth line only when it lies within a quarter
of that grid interval. Most internal resets remain free. The analog main VCO
phase is unchanged, and VCO2 SUB2 noise is not restarted. This gives bar
anchors and some grid-aligned clicks, not full click quantization.

STOP restores free interaction. NEXT only advances the sequence. The grid
acquires over two sixteenth boundaries and expires after two missing ones.
Use internal tempo, MIDI clock, or a sixteenth clock at CLOCK IN. Brake and Accelerate
are the third and fourth shape points (about 29% and 43% clockwise): successive
intervals widen or shrink geometrically, inspired by Nihilo's bounce/brake
curves. SUB2 modulation can still bend their timing.

## Harmony

With PLAY stopped, enter settings, select **SEQ 2 OSC2**, use **RESET** to reach
Normal (no step LEDs), then **NEXT** twice for Harmony (first two step LEDs).
**PLAY** saves/exits. Pitch mode is one setting; no separate enable exists.
Changing pitch mode clears held MIDI notes, closes software gates and resets
glide; panel EG Held is then restored when selected. Rhythm type is unchanged.
Chord quality always follows the selected scale and root.

**Spread**, on settings SEQ1 OSC1 or CC112, applies after quantization. At 127,
source order VCO1 / SUB1 / SUB2 / VCO2 / SUB1 / SUB2 gets offsets
−30 / −18 / −6 / +6 / +18 / +30 cents. Zero preserves exact tuning. Intermediate
amounts scale linearly; try 32–64. Spread only affects Harmony.

| Knob | Harmony function | Matching sequence assignment |
|---|---|---|
| VCO1 | Chord root | SEQ 1 OSC1 moves chord root and bass |
| VCO1 SUB1 | Scale: 14 Ionian/harmonic-minor modes | SEQ 1 SUB1 offsets scale selection |
| VCO1 SUB2 | Key: C through B | SEQ 1 SUB2 offsets key selection |
| VCO2 | Independent in-scale melody pitch | SEQ 2 OSC2 moves the melody |
| VCO2 SUB1 | Inversion: root, first, second, third | SEQ 2 SUB1 offsets inversion |
| VCO2 SUB2 | Voicing: close, drop 2, drop 3, open | SEQ 2 SUB2 offsets voicing |

| Audible source | Harmony role |
|---|---|
| VCO1 | Un-inverted root one octave down (bass) |
| VCO1 SUB1 | Chord root |
| VCO1 SUB2 | Third |
| VCO2 | Independent scale melody |
| VCO2 SUB1 | Fifth |
| VCO2 SUB2 | Seventh |

**Starting patch:** turn up the four SUB mixer levels. Put all four SUB pitch
knobs fully counterclockwise for C Ionian, root inversion, close voicing.
Assign only SEQ 1 OSC1 initially; four sequence
steps now move through diatonic seventh chords. Add VCO1 for bass. Add VCO2 and
its SEQ 2 OSC2 assignment for a second melodic line. To sequence inversions,
turn on SEQ 2 SUB1's assignment and adjust SEQ 2's steps.

Scale order: Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian;
then the seven harmonic-minor rotations (harmonic minor, Locrian natural 6,
Ionian sharp 5, Dorian sharp 4, Phrygian natural 3, Lydian sharp 2, Super Locrian).
Chord quality always stacks thirds in the selected scale. 
