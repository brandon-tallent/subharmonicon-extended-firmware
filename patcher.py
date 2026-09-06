#!/usr/bin/env python3
"""Apply an extension-only patch to a user-supplied Subharmonicon 1.1.1 update."""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
from firmware_sysex import (IMAGE_START_ADDRESS, SYSEX_BYTE_COUNT, VENDOR_BINARY_SHA256,
                            VENDOR_SYSEX_SHA256, pack_sysex, unpack_sysex)

ROOT = Path(__file__).resolve().parent
MAX_PATCH_BYTES = 400_000


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def decode_patch(data: bytes) -> list[tuple[int, bytes]]:
    if len(data) > MAX_PATCH_BYTES:
        raise ValueError('patch exceeds size limit')
    patch = json.loads(data)
    if not isinstance(patch, dict) or set(patch) != {'format', 'base_sha256', 'segments'}:
        raise ValueError('invalid patch fields')
    if type(patch['format']) is not int or patch['format'] != 1:
        raise ValueError('unsupported patch format')
    if patch['base_sha256'] != VENDOR_BINARY_SHA256:
        raise ValueError('patch targets an unsupported firmware')
    segments = patch['segments']
    if not isinstance(segments, list) or not 1 <= len(segments) <= 128:
        raise ValueError('invalid patch segment count')
    layout = json.loads((ROOT / 'src/patch_layout.json').read_text())
    hooks = {h['section']: h for h in layout['hooks']}
    extension = {'.text', '.econst', '.switch', '.feature_bridge', '.feature_fast'}
    seen = set()
    result = []
    for segment in segments:
        if not isinstance(segment, dict) or set(segment) != {'section', 'address', 'data', 'sha256'}:
            raise ValueError('invalid segment fields')
        name, address = segment['section'], segment['address']
        if not isinstance(name, str) or name in seen or type(address) is not int:
            raise ValueError('invalid or duplicate segment')
        if not isinstance(segment['data'], str) or not isinstance(segment['sha256'], str):
            raise ValueError('invalid segment encoding')
        payload = bytes.fromhex(segment['data'])
        if not payload or len(payload) % 2 or sha256(payload) != segment['sha256']:
            raise ValueError('invalid segment length or checksum')
        end = address + len(payload) // 2
        if name in hooks:
            hook = hooks[name]
            if address != hook['address'] or len(payload) != hook['words'] * 2:
                raise ValueError('hook escapes its fixed replacement window')
        elif name not in extension or not layout['extension_start'] <= address < end <= layout['extension_end']:
            raise ValueError('segment escapes the extension region')
        seen.add(name)
        result.append((address, payload))
    if not set(hooks).issubset(seen) or not {'.text', '.econst', '.feature_bridge', '.feature_fast'}.issubset(seen):
        raise ValueError('patch is missing required sections')
    result.sort()
    for (address, payload), (following, _) in zip(result, result[1:]):
        if address + len(payload) // 2 > following:
            raise ValueError('overlapping patch segments')
    return result


def apply_patch(vendor: bytes, patch_data: bytes) -> tuple[bytes, str]:
    if sha256(vendor) != VENDOR_SYSEX_SHA256:
        raise ValueError('input must be the unmodified official Subharmonicon 1.1.1 SysEx')
    stock = unpack_sysex(vendor).data
    if sha256(stock) != VENDOR_BINARY_SHA256:
        raise ValueError('unexpected stock image hash')
    image = bytearray(stock)
    for address, payload in decode_patch(patch_data):
        offset = (address - IMAGE_START_ADDRESS) * 2
        image[offset:offset + len(payload)] = payload
    packed = pack_sysex(bytes(image), vendor)
    if unpack_sysex(packed).data != image:
        raise ValueError('generated update failed its container round trip')
    return packed, sha256(image)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('firmware', type=Path, help='official 1.1.1 .syx downloaded separately')
    parser.add_argument('patch', type=Path, help='extension-only .subhpatch file')
    parser.add_argument('-o', '--output', type=Path, required=True, help='new .syx file; existing files are never overwritten')
    args = parser.parse_args()
    try:
        with args.patch.open('rb') as source:
            patch_data = source.read(MAX_PATCH_BYTES + 1)
        with args.firmware.open('rb') as source:
            vendor = source.read(SYSEX_BYTE_COUNT + 1)
        packed, digest = apply_patch(vendor, patch_data)
        with args.output.open('xb') as output:
            output.write(packed)
    except (OSError, ValueError) as error:
        parser.error(str(error))
    print(f'Created {args.output}\nImage SHA-256: {digest}')
    print('No MIDI was sent. Keep the official update and its recovery instructions.')


if __name__ == '__main__':
    main()
