# Subharmonicon firmware extension

An unofficial firmware extension for the Moog Subharmonicon:

* 3 new pitch modes (Paraphonic, Harmony, Morph) that unlock the sub-oscillators from their
shackles and makes different uses of them for each mode.
* New midi features: 6 voices (in paraphonic mode), midi velocity, 303 style glide, CC over rhythm knobs
* Alternate rhythm mode that plays a selection from a library of 32 64 step gate patterns and allows indpendent triggering of VCA and VCF envelopes.

See [FEATURES.md](FEATURES.md) for details and the
[cheat sheet](https://brandon-tallent.github.io/subharmonicon-extended-firmware/cheatsheet.html) for a quick reference.

## Quick Start

Open the [browser patcher](https://brandon-tallent.github.io/subharmonicon-extended-firmware/).
Download Moog's firmware 1.1.1, unzip it, and choose
`Subharmonicon_Firmware_v1_1_1.syx`. The modified SysEx downloads automatically;
your original file never leaves your browser. Install the result using the
update instructions included in Moog's download.

You can also apply a release patch locally:

```sh
python3 patcher.py /path/to/Subharmonicon_Firmware_v1_1_1.syx \
  subharmonicon.subhpatch -o subharmonicon-custom.syx
```

## Building from source

first:

install [TI C2000 Code Generation Tools](https://www.ti.com/tool/download/C2000-CGT)
25.11.1.LTS
```sh
make -j C2000_CGT_ROOT=/path/to/ti-cgt-c2000_25.11.1.LTS
make check
```

next, grab subharmonicon firmware 1.1.1 from Moog's download site.

verify the firmware is correct :
```sh
SUBH_STOCK_SYSEX=/path/to/Subharmonicon_Firmware_v1_1_1.syx make check
```

finally, build the firmware patch.
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

## Browser patcher

`make release` also regenerates `patcher.html`, a self-contained HTML file with the JavaScript, extension
patch, and allowed address layout embedded. It also writes
`dist/subharmonicon-patcher.html` for a single-file download. Commit this generated file with firmware
changes. GitHub Pages serves this HTML file and the cheat sheet; no compiler
or vendor firmware is needed during deployment. The private sync command rebuilds
and synchronizes this bundle automatically.

Open `patcher.html` directly in a current browser to use it offline. The source
template and scripts live in `web/`; edit those files and run `make release`
to regenerate the downloadable HTML.
