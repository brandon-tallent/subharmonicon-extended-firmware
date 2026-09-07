// Browser equivalent of firmware_sysex.py and patcher.py. Word addresses are 16-bit.
export const SYSEX_BYTES = 424068;
const STOCK_SYSEX = '15884732d192208d8f66d130104d15c66335cce85d030f44c5497e7b76677b5b';
const STOCK_BINARY = 'eb27aabcbfdb3aab9f66bb52e396e5185729ad96661eae68a1f3370da50b4d97';
const START = 0x084000, END = 0x09fffa, IMAGE_BYTES = (END - START) * 2;
const PREFIX = [0xf0, 4, 0x1b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x6f, 1, 0];
const HEADER = [0x12, 0x10, 0x380, 0, 0x37, 1, 1, 1, 1, 0, 0x6f, 0];
const EXTENSIONS = new Set(['.text', '.econst', '.switch', '.feature_bridge', '.feature_fast']);

function require(condition, message) {
  if (!condition) throw new Error(message);
}
function fields(object, names) {
  return object !== null && typeof object === 'object' && !Array.isArray(object) &&
    Object.keys(object).sort().join(',') === names.split(',').sort().join(',');
}
function checksum(words, index, end = words.length) {
  let sum = 0;
  for (let i = 0; i < end; i++) if (i !== index) sum += words[i];
  return sum & 0xffff;
}
export async function sha256(bytes) {
  const hash = new Uint8Array(await crypto.subtle.digest('SHA-256', bytes));
  return Array.from(hash, b => b.toString(16).padStart(2, '0')).join('');
}
function word(bytes, offset) {
  const [a, b, c] = bytes.subarray(offset, offset + 3);
  require(a >= 0x40 && a <= 0x4f && b <= 0x3f && c <= 0x3f, 'Invalid SysEx word encoding.');
  return ((a & 15) << 12) | (b << 6) | c;
}
function putWord(bytes, offset, value) {
  bytes.set([0x40 | (value >> 12), (value >> 6) & 63, value & 63], offset);
}
function zeroes(bytes, start, end) {
  require(bytes.subarray(start, end).every(b => b === 0), 'Invalid SysEx padding.');
}

export function unpack(bytes) {
  require(bytes instanceof Uint8Array && bytes.length === SYSEX_BYTES, 'Unexpected firmware file size.');
  require(PREFIX.every((b, i) => bytes[i] === b) && bytes.at(-1) === 0xf7, 'Invalid firmware container.');
  require(bytes.subarray(1, -1).every(b => b <= 127), 'Invalid MIDI payload.');
  zeroes(bytes, 16, 80);
  require(bytes[80] === 1 && bytes[145] === 1, 'Invalid firmware header delimiter.');
  zeroes(bytes, 81, 145);
  const header = Array.from({length: 16}, (_, i) => word(bytes, 146 + i * 3));
  require(HEADER.every((w, i) => header[i] === w) && header[15] === 0, 'Unknown firmware header.');
  require(header[14] === checksum(header, 14), 'Firmware header checksum mismatch.');
  require(header[12] + header[13] * 65536 === START, 'Invalid firmware start address.');
  zeroes(bytes, 194, 258);
  const image = new Uint8Array(IMAGE_BYTES);
  const view = new DataView(image.buffer);
  let next = START;
  for (let n = 0; n < 896; n++) {
    const offset = 258 + n * 473;
    require(bytes[offset] === 1, 'Invalid firmware record delimiter.');
    const words = Array.from({length: 136}, (_, i) => word(bytes, offset + 1 + i * 3));
    const count = words[2], address = words[4] + words[5] * 65536;
    require(words[0] === 0x13 && words[1] === 136 && words[3] === 0, 'Invalid firmware record.');
    require(count === (n === 895 ? 122 : 128) && address === next && address + count <= END,
      'Invalid firmware record address or length.');
    require(words[6] === checksum(words, 6, 7 + count), 'Firmware record checksum mismatch.');
    require(words.slice(7 + count).every(w => w === 0), 'Invalid firmware record padding.');
    for (let i = 0; i < count; i++) view.setUint16((address - START + i) * 2, words[7 + i], true);
    zeroes(bytes, offset + 409, offset + 473);
    next += count;
  }
  require(next === END && bytes[424066] === 1 && bytes[424067] === 0xf7, 'Invalid firmware trailer.');
  return image;
}

function pack(image, template) {
  const result = template.slice();
  const view = new DataView(image.buffer, image.byteOffset, image.byteLength);
  let cursor = 0;
  for (let n = 0; n < 896; n++) {
    const offset = 258 + n * 473, count = n === 895 ? 122 : 128;
    const address = START + cursor;
    const words = [0x13, 136, count, 0, address & 0xffff, address >> 16, 0];
    for (let i = 0; i < count; i++) words.push(view.getUint16((cursor + i) * 2, true));
    words[6] = checksum(words, 6);
    words.forEach((w, i) => putWord(result, offset + 1 + i * 3, w));
    cursor += count;
  }
  require(unpack(result).every((b, i) => b === image[i]), 'Generated firmware failed verification.');
  return result;
}

export async function decodePatch(bundle) {
  require(fields(bundle, 'patch,layout'), 'Invalid patch bundle.');
  const {patch, layout} = bundle;
  require(fields(patch, 'format,base_sha256,segments') && patch.format === 1 &&
    patch.base_sha256 === STOCK_BINARY, 'Unsupported firmware patch.');
  require(new TextEncoder().encode(JSON.stringify(patch)).length <= 400000, 'Patch exceeds size limit.');
  require(fields(layout, 'hooks,extension_start,extension_end') && Array.isArray(layout.hooks) &&
    layout.hooks.length > 0 && layout.hooks.length <= 128 &&
    layout.extension_start === 0x09497e && layout.extension_end === 0x09fff0, 'Invalid patch layout.');
  const hooks = new Map();
  for (const hook of layout.hooks) {
    require(fields(hook, 'section,address,words') && typeof hook.section === 'string' &&
      /^\.patch_[0-9a-f]{6}$/.test(hook.section) && !hooks.has(hook.section) &&
      Number.isInteger(hook.address) && Number.isInteger(hook.words) && hook.words > 0 &&
      hook.address >= START && hook.address + hook.words <= layout.extension_start,
      'Invalid patch hook.');
    require(parseInt(hook.section.slice(7), 16) === hook.address, 'Patch hook address mismatch.');
    hooks.set(hook.section, hook);
  }
  require(Array.isArray(patch.segments) && patch.segments.length >= 1 && patch.segments.length <= 128,
    'Invalid patch segment count.');
  const seen = new Set(), segments = [];
  for (const segment of patch.segments) {
    require(fields(segment, 'section,address,data,sha256'), 'Invalid patch segment fields.');
    const {section, address, data, sha256: expectedHash} = segment;
    require(typeof section === 'string' && !seen.has(section) && Number.isInteger(address) &&
      typeof data === 'string' && /^(?:[0-9a-fA-F]{4})+$/.test(data) &&
      typeof expectedHash === 'string' && /^[0-9a-f]{64}$/.test(expectedHash), 'Invalid patch segment.');
    const payload = Uint8Array.from(data.match(/../g), h => parseInt(h, 16));
    require(await sha256(payload) === expectedHash, 'Patch checksum mismatch.');
    const end = address + payload.length / 2;
    if (hooks.has(section)) {
      const hook = hooks.get(section);
      require(address === hook.address && payload.length === hook.words * 2, 'Patch escapes its hook boundary.');
    } else {
      require(EXTENSIONS.has(section) && address >= layout.extension_start && end <= layout.extension_end,
        'Patch escapes the extension region.');
    }
    seen.add(section);
    segments.push({address, payload});
  }
  require([...hooks.keys(), '.text', '.econst', '.feature_bridge', '.feature_fast'].every(s => seen.has(s)),
    'Patch is missing required sections.');
  segments.sort((a, b) => a.address - b.address);
  for (let i = 1; i < segments.length; i++) {
    require(segments[i - 1].address + segments[i - 1].payload.length / 2 <= segments[i].address,
      'Patch segments overlap.');
  }
  return segments;
}

export async function patchFirmware(vendor, bundle) {
  require(vendor.length === SYSEX_BYTES && await sha256(vendor) === STOCK_SYSEX,
    'Choose the unmodified Subharmonicon_Firmware_v1_1_1.syx from Moog’s firmware download.');
  const image = unpack(vendor);
  require(await sha256(image) === STOCK_BINARY, 'Unexpected stock firmware image.');
  for (const {address, payload} of await decodePatch(bundle)) image.set(payload, (address - START) * 2);
  return pack(image, vendor);
}
