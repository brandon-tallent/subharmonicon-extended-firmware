#!/usr/bin/env python3
"""Volatile development controls for the optional feature image (command 0x35)."""
import argparse
import math
from pathlib import Path

SLOTS=('vco1-note','vco1-sub1-note','vco1-sub2-note','vco2-note','vco2-sub1-note','vco2-sub2-note')

def encode_request(setting,value):
    if not math.isfinite(value):raise ValueError('value must be finite')
    if setting in ('enable','pitch-controls-enable','glide-enable','noise-enable'):
        if value not in (0,1):raise ValueError('enable must be 0 or 1')
        ident={'enable':7,'pitch-controls-enable':9,'glide-enable':12,'noise-enable':14}[setting];raw=int(value)
    elif setting=='mode':
        if value not in (0,1,2,3):raise ValueError('mode must be 0, 1, 2, or 3')
        ident=8;raw=int(value)
    elif setting=='glide-time':
        if value!=int(value) or not 0<=value<=16383:raise ValueError('glide time must be an integer 0..16383')
        ident=13;raw=int(value)
    elif setting=='knob-span':
        if value not in (12,24):raise ValueError('knob span must be 12 or 24')
        ident=10;raw=int(value)
    elif setting=='quantize':
        if value not in (0,1,3,4,5):raise ValueError('quantize must be 0, 1, 3, 4, or 5')
        ident=11;raw=int(value)
    elif setting in SLOTS:
        if not 0<=value<=127:raise ValueError('note must be within MIDI 0..127')
        ident=16+SLOTS.index(setting);raw=round(value*128)
    else:raise ValueError('unknown feature setting')
    return bytes((0xf0,4,0x1b,0x35,ident,raw>>7,raw&127,*([0]*8),127,0xf7))

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('setting',choices=('enable','mode','pitch-controls-enable','knob-span','quantize','glide-enable','glide-time','noise-enable',*SLOTS));p.add_argument('value',type=float)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    try:wire=encode_request(a.setting,a.value)
    except ValueError as e:p.error(str(e))
    with a.output.open('xb') as f:f.write(wire)
if __name__=='__main__':main()
