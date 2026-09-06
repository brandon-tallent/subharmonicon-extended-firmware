#!/usr/bin/env python3
"""Persistent optional-feature parameters (Subharmonicon command 0x23).

mode: normal, paraphonic, harmony, morph. rhythm: stock, patterns.
spread and velocity-depth: 0..127. glide and noise: 0/1. span: default, 12, 24.
glide-time: 0..16383; zero selects the documented default.
"""
import argparse
from pathlib import Path

def encode_config(setting,value,unit=127):
    if not isinstance(unit,int) or not 0<=unit<=127:raise ValueError('unit must be 0..127')
    if setting in ('glide','noise'):
        if str(value) not in ('0','1'):raise ValueError('toggle must be 0 or 1')
        ident,raw=5 if setting=='glide' else 10,int(value)
    elif setting=='mode':
        values={'normal':0,'paraphonic':1,'harmony':2,'morph':3,'3':3,'0':0,'1':1,'2':2}
        if str(value) not in values:raise ValueError('mode must be normal, paraphonic, harmony, or morph')
        ident,raw=8,values[str(value)]
    elif setting=='span':
        values={'default':0,'12':1,'24':2}
        if str(value) not in values:raise ValueError('span must be default, 12, or 24')
        ident,raw=4,values[str(value)]
    elif setting in ('spread','velocity-depth'):
        try:raw=int(str(value))
        except ValueError:raise ValueError('scalar value must be an integer 0..127')
        if not 0<=raw<=127:raise ValueError('scalar value must be 0..127')
        ident=3 if setting=='spread' else 7
    elif setting=='rhythm':
        values={'stock':0,'patterns':1,'0':0,'1':1}
        if str(value) not in values:raise ValueError('rhythm must be stock or patterns')
        ident,raw=9,values[str(value)]
    elif setting=='glide-time':
        try:raw=int(str(value))
        except ValueError:raise ValueError('glide time must be an integer 0..16383')
        if not 0<=raw<=16383:raise ValueError('glide time must be 0..16383')
        ident=6
    else:raise ValueError('unknown persistent feature setting')
    return bytes((0xf0,4,0x1b,0x23,ident,raw>>7,raw&127,*([0]*8),unit,0xf7))

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('setting',choices=('spread','span','glide','glide-time','velocity-depth','mode','rhythm','noise'));p.add_argument('value')
    p.add_argument('--unit',type=int,default=127);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args()
    try:wire=encode_config(a.setting,a.value,a.unit)
    except ValueError as error:p.error(str(error))
    with a.output.open('xb') as output:output.write(wire)
if __name__=='__main__':main()
