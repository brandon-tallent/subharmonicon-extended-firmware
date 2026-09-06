from __future__ import annotations

import hashlib
import os
from pathlib import Path
import unittest

from firmware_sysex import (
    DATA_RECORD_OFFSET,
    DATA_RECORD_SIZE,
    DATA_RECORD_WORD_COUNT,
    HEADER_RECORD_OFFSET,
    IMAGE_BYTE_COUNT,
    VENDOR_BINARY_SHA256,
    VENDOR_SYSEX_SHA256,
    pack_sysex,
    unpack_sysex,
)


ROOT = Path(__file__).resolve().parents[1]
VENDOR_PATH = Path(os.environ.get("SUBH_STOCK_SYSEX", str(ROOT / "vendor/Subharmonicon_Firmware_v1_1_1.syx")))


class FirmwareSysexTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        if not VENDOR_PATH.is_file():
            raise unittest.SkipTest("set SUBH_STOCK_SYSEX to your official 1.1.1 update")
        cls.vendor = VENDOR_PATH.read_bytes()
        cls.image = unpack_sysex(cls.vendor).data

    def test_vendor_hashes_and_extent(self) -> None:
        decoded = unpack_sysex(self.vendor)
        self.assertEqual(hashlib.sha256(self.vendor).hexdigest(), VENDOR_SYSEX_SHA256)
        self.assertEqual(hashlib.sha256(decoded.data).hexdigest(), VENDOR_BINARY_SHA256)
        self.assertEqual(decoded.start_address, 0x084000)
        self.assertEqual(decoded.end_address, 0x09FFFA)
        self.assertEqual(len(decoded.records), 896)

    def test_vendor_image_repacks_byte_for_byte(self) -> None:
        self.assertEqual(pack_sysex(self.image, self.vendor), self.vendor)

    def test_modified_image_round_trips_without_reusing_vendor_checksum(self) -> None:
        modified = bytearray(self.image)
        modified[0x200] ^= 0x01
        packed = pack_sysex(bytes(modified), self.vendor)
        self.assertNotEqual(packed, self.vendor)
        self.assertEqual(unpack_sysex(packed).data, modified)

    def test_rejects_wrong_image_size(self) -> None:
        with self.assertRaisesRegex(ValueError, "image size"):
            pack_sysex(bytes(IMAGE_BYTE_COUNT - 2), self.vendor)

    def test_rejects_non_midi_safe_byte(self) -> None:
        damaged = bytearray(self.vendor)
        damaged[20] = 0x80
        with self.assertRaisesRegex(ValueError, "non-MIDI-safe"):
            unpack_sysex(bytes(damaged))

    def test_rejects_header_checksum_damage(self) -> None:
        damaged = bytearray(self.vendor)
        checksum_low_byte = HEADER_RECORD_OFFSET + 1 + 14 * 3 + 2
        damaged[checksum_low_byte] ^= 0x01
        with self.assertRaisesRegex(ValueError, "header checksum"):
            unpack_sysex(bytes(damaged))

    def test_rejects_unknown_header_identity(self) -> None:
        damaged = bytearray(self.vendor)
        identity_low_byte = HEADER_RECORD_OFFSET + 1 + 2 * 3 + 2
        damaged[identity_low_byte] ^= 0x01
        with self.assertRaisesRegex(ValueError, "header identity"):
            unpack_sysex(bytes(damaged))

    def test_rejects_data_checksum_damage(self) -> None:
        damaged = bytearray(self.vendor)
        first_payload_low_byte = DATA_RECORD_OFFSET + 1 + 7 * 3 + 2
        damaged[first_payload_low_byte] ^= 0x01
        with self.assertRaisesRegex(ValueError, "record 0: checksum"):
            unpack_sysex(bytes(damaged))

    def test_rejects_nonzero_short_record_padding(self) -> None:
        damaged = bytearray(self.vendor)
        last_record = DATA_RECORD_OFFSET + 895 * DATA_RECORD_SIZE
        first_padding_low_byte = last_record + 1 + (7 + 122) * 3 + 2
        damaged[first_padding_low_byte] = 1
        with self.assertRaisesRegex(ValueError, "record 895: nonzero record padding"):
            unpack_sysex(bytes(damaged))

    def test_rejects_unmarked_record_word(self) -> None:
        damaged = bytearray(self.vendor)
        damaged[DATA_RECORD_OFFSET + 1] = 0
        with self.assertRaisesRegex(ValueError, "word-start marker"):
            unpack_sysex(bytes(damaged))

    def test_record_geometry_constant_matches_wire_layout(self) -> None:
        self.assertEqual(DATA_RECORD_SIZE, 1 + DATA_RECORD_WORD_COUNT * 3 + 64)


if __name__ == "__main__":
    unittest.main()
