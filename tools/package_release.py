#!/usr/bin/env python3
"""Package the extension-only patch for a GitHub release."""
import hashlib
import json
from pathlib import Path
import sys
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from patcher import decode_patch


def main():
    patch = (ROOT / 'build/subharmonicon.subhpatch').read_bytes()
    decode_patch(patch)
    dist = ROOT / 'dist'
    dist.mkdir(exist_ok=True)
    (dist / 'subharmonicon.subhpatch').write_bytes(patch)
    manifest = {
        'format': 1,
        'baseline': '20260906-tallent-mod',
        'files': {'subharmonicon.subhpatch': hashlib.sha256(patch).hexdigest()},
    }
    (dist / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(f'Packaged extension-only patch in {dist}; no full firmware image included')


if __name__ == '__main__':
    main()
