# Subharmonicon firmware extension

An unofficial firmware extension for the Moog Subharmonicon: 

* 3 new pitch modes (Paraphonic, Harmony, Morph) that unlock the sub-oscillators from their 
shackles and makes different uses of them for each mode. 
* New midi features: 6 voices (in paraphonic mode), midi velocity, 303 style glide, CC over rhythm knobs
* Alternate rhythm mode that plays a selection from a library of 32 64 step gate patterns and allows indpendent triggering of VCA and VCF envelopes. 

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

## Patching existing firmware without build tools 

if you just want to install the custom firwmare on your module, download `subharmonicon.subhpatch` from the
[latest GitHub release](../../releases/latest).

then run
```sh
python3 patcher.py /path/to/Subharmonicon_Firmware_v1_1_1.syx \
  /path/to/subharmonicon.subhpatch -o subharmonicon-custom.syx
```

FLash the subharmonicon with the resulting syx file per the manual instructions. 

## License

Our original work is MIT licensed. Retained third-party rhythm data has its own
notice; see [NOTICE.md](NOTICE.md) for scope and the unresolved distribution
question. No rights to Moog's firmware are granted by this repository.
