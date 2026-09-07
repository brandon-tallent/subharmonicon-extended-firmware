#!/usr/bin/env python3
"""Package the extension-only patch and self-contained browser patcher."""
import hashlib
import json
from pathlib import Path
import re
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
    web = ROOT / 'web'
    bundle = {
        'patch': json.loads(patch),
        'layout': json.loads((ROOT / 'src/patch_layout.json').read_text()),
    }
    payload = json.dumps(bundle, separators=(',', ':')).replace('<', '\\u003c')
    core = re.sub(r'^export ', '', (web / 'patcher.mjs').read_text(), flags=re.M)
    app = (web / 'app.mjs').read_text()
    script = ('<script id="firmware-data" type="application/json">' + payload + '</script>\n'
              '<script type="module">\n' + core + '\n' + app + '\n</script>')
    template = (web / 'patcher.template.html').read_text()
    if template.count('<!-- PATCHER_SCRIPT -->') != 1:
        raise ValueError('browser template must contain exactly one script marker')
    html = template.replace('<!-- PATCHER_SCRIPT -->', script)
    (ROOT / 'patcher.html').write_text(html)
    (dist / 'subharmonicon-patcher.html').write_text(html)
    manifest = {
        'format': 1,
        'baseline': '20260906-tallent-mod',
        'files': {
            'subharmonicon.subhpatch': hashlib.sha256(patch).hexdigest(),
            'subharmonicon-patcher.html': hashlib.sha256(html.encode()).hexdigest(),
        },
    }
    (dist / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(f'Packaged extension-only patch and browser HTML in {dist}; no full firmware image included')


if __name__ == '__main__':
    main()
