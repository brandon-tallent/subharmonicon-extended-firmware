#!/usr/bin/env python3
"""Package only linked extension sections; no vendor firmware is an input."""
from pathlib import Path
import argparse
import base64
import json
import sys
import xml.etree.ElementTree as ET
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from patcher import decode_patch, sha256
from firmware_sysex import VENDOR_BINARY_SHA256


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('xml', type=Path)
    p.add_argument('output', type=Path)
    a = p.parse_args()
    tree = ET.parse(a.xml)
    segments = []
    for section in tree.findall('.//ti_coff/section'):
        name = section.findtext('name')
        size = int(section.findtext('raw_data_size'), 16)
        address = int(section.findtext('virtual_addr'), 16)  # TI COFF load address
        raw = section.findtext('raw_data') or ''
        payload = base64.b64decode(''.join(raw.split()), validate=True)
        if name.startswith('.debug') or name == '$build.attributes':
            continue
        if not payload:
            continue
        if len(payload) != size:
            raise ValueError(f'inconsistent linked section size: {name}')
        segments.append(dict(section=name, address=address, data=payload.hex(), sha256=sha256(payload)))
    patch = dict(format=1, base_sha256=VENDOR_BINARY_SHA256,
                 segments=sorted(segments, key=lambda s: s['address']))
    encoded = (json.dumps(patch, indent=2) + '\n').encode()
    decode_patch(encoded)
    a.output.write_bytes(encoded)
    print(f'{a.output}: {len(segments)} sections, {sum(len(s["data"])//2 for s in segments)} replacement bytes')


if __name__ == '__main__':
    main()
