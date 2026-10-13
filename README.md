# Subharmonicon firmware - Tallent Edition

An unofficial firmware extension for the Moog Subharmonicon:

* 3 new pitch modes (Paraphonic, Harmony, Morph) free sub oscillators from their
shackles and applies them towards unique roles in each mode. 
* New midi features: 6 voices (in paraphonic mode), midi velocity, 303 style glide/legato, CC control of rhythm knobs. 
* Alternate rhythm mode with selectable gate patterns and indpendent triggering of VCA and VCF envelopes.

All original features remain in-tact and usable with the Tallent Edition firmware. All new features are optionally enabled on the panel and can be toggled off to restore factory function. The original firmware can be restored any time using the manual instructions. 

See [FEATURES.md](FEATURES.md) for details and the quick reference
[cheat sheet](https://brandon-tallent.github.io/subharmonicon-extended-firmware/cheatsheet.html).

## Installing

**[Open the firmware patcher](https://brandon-tallent.github.io/subharmonicon-extended-firmware/)**

1. Download **Subharmonicon Firmware v1.1.1** from
   [Moog's downloads page](https://www.moogmusic.com/downloads/?product=Subharmonicon)
   and unzip it.
2. Open the patcher above and select `Subharmonicon_Firmware_v1_1_1.syx`.
   Choose the firmware file, not the ZIP, erase, or invalidate file.
3. Your Tallent Edition firmware downloads automatically as `subharmonicon-tallent-edition.syx`.
4. Follow the update instructions included in Moog's download, using
   `subharmonicon-tallent-edition.syx` in place of the official firmware file. Keep using
   Moog's supplied invalidate and erase files as instructed.

Your firmware stays in your browser; nothing is uploaded. To use the patcher
offline, click **Save this tool for offline use** on the page, then open the
downloaded HTML file in a current browser.

### CLI alternative

Download `subharmonicon.subhpatch` from the
[latest release](https://github.com/brandon-tallent/subharmonicon-extended-firmware/releases/latest)
and run this command from a copy of this repository:

```sh
python3 patcher.py /path/to/Subharmonicon_Firmware_v1_1_1.syx \
  subharmonicon.subhpatch -o subharmonicon-tallent-edition.syx
```

## Building from source

- Download the offical firmware as described above
- install [TI C2000 Code Generation Tools](https://www.ti.com/tool/download/C2000-CGT)
25.11.1.LTS

```sh
make -j C2000_CGT_ROOT=/path/to/ti-cgt-c2000_25.11.1.LTS
make check
```

Verify downloaded firmware is correct
```sh
SUBH_STOCK_SYSEX=/path/to/Subharmonicon_Firmware_v1_1_1.syx make check
```

Build the firmware patch.
```sh
make release C2000_CGT_ROOT=/path/to/ti-cgt-c2000_25.11.1.LTS
```

## File layout

- `src/features*.c` feature headers
- `src/features_bridge.asm` and `src/features_rhythm_bridge.asm` ABI adaptors
- `src/patches.asm` replacement hooks. `src/stock_abi.inc` address/offsets; external routine addresses live in `src/extension_link.cmd`.

## License

Our original work is MIT licensed. No rights to Moog's firmware are granted by
this repository.
