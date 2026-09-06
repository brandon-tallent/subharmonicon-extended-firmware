#!/usr/bin/env python3
"""Strict codec for the Subharmonicon 1.1.1 firmware SysEx container."""

from __future__ import annotations

from dataclasses import dataclass
import struct


VENDOR_SYSEX_SHA256 = (
    "15884732d192208d8f66d130104d15c66335cce85d030f44c5497e7b76677b5b"
)
VENDOR_BINARY_SHA256 = (
    "eb27aabcbfdb3aab9f66bb52e396e5185729ad96661eae68a1f3370da50b4d97"
)

WIRE_PREFIX = bytes.fromhex("f0 04 1b 00 00 00 00 00 00 00 00 00 00 6f 01 00")
END_OF_EXCLUSIVE = 0xF7
START_OF_HEADER = 0x01
RESYNC_ZERO_BYTES = 64

HEADER_WORD_COUNT = 16
HEADER_CHECKSUM_INDEX = 14
HEADER_DESTINATION_INDEX = 12
HEADER_RECORD_OFFSET = 145
HEADER_FIXED_WORDS = (
    0x0012,
    0x0010,
    0x0380,
    0x0000,
    0x0037,
    0x0001,
    0x0001,
    0x0001,
    0x0001,
    0x0000,
    0x006F,
    0x0000,
)

DATA_RECORD_COUNT = 896
DATA_RECORD_WORD_COUNT = 136
DATA_RECORD_PAYLOAD_MAX = 128
DATA_RECORD_CHECKSUM_INDEX = 6
DATA_RECORD_PAYLOAD_INDEX = 7
DATA_RECORD_OFFSET = 258
DATA_RECORD_SIZE = 473

IMAGE_START_ADDRESS = 0x084000
IMAGE_END_ADDRESS = 0x09FFFA
IMAGE_WORD_COUNT = IMAGE_END_ADDRESS - IMAGE_START_ADDRESS
IMAGE_BYTE_COUNT = IMAGE_WORD_COUNT * 2
SYSEX_BYTE_COUNT = 424_068


@dataclass(frozen=True)
class RecordLayout:
    address: int
    word_count: int


@dataclass(frozen=True)
class FirmwareImage:
    data: bytes
    start_address: int
    end_address: int
    header_words: tuple[int, ...]
    records: tuple[RecordLayout, ...]


def encode_word(word: int) -> bytes:
    """Encode one 16-bit word as a marked, MIDI-safe 6-bit triplet."""
    if not isinstance(word, int) or not 0 <= word <= 0xFFFF:
        raise ValueError("firmware word is outside the 16-bit range")
    return bytes((0x40 | (word >> 12), (word >> 6) & 0x3F, word & 0x3F))


def decode_word(encoded: bytes, context: str = "firmware word") -> int:
    """Decode one marked 6-bit triplet and reject malformed wire bytes."""
    if len(encoded) != 3:
        raise ValueError(f"{context}: encoded word is not three bytes")
    if not 0x40 <= encoded[0] <= 0x4F:
        raise ValueError(f"{context}: missing or invalid word-start marker")
    if encoded[1] > 0x3F or encoded[2] > 0x3F:
        raise ValueError(f"{context}: invalid 6-bit data byte")
    return ((encoded[0] & 0x0F) << 12) | (encoded[1] << 6) | encoded[2]


def _decode_words(encoded: bytes, count: int, context: str) -> tuple[int, ...]:
    if len(encoded) != count * 3:
        raise ValueError(f"{context}: invalid encoded span length")
    return tuple(
        decode_word(encoded[offset : offset + 3], f"{context} word {number}")
        for number, offset in enumerate(range(0, len(encoded), 3))
    )


def _checksum(words: tuple[int, ...] | list[int], checksum_index: int) -> int:
    return sum(word for index, word in enumerate(words) if index != checksum_index) & 0xFFFF


def _require_zero_span(data: bytes, context: str) -> None:
    if any(data):
        raise ValueError(f"{context}: nonzero resynchronization padding")


def unpack_sysex(data: bytes) -> FirmwareImage:
    """Validate and unpack one fixed-layout Subharmonicon firmware image."""
    if len(data) != SYSEX_BYTE_COUNT:
        raise ValueError(
            f"unexpected firmware SysEx size: {len(data)} (expected {SYSEX_BYTE_COUNT})"
        )
    if data[: len(WIRE_PREFIX)] != WIRE_PREFIX:
        raise ValueError("not a Subharmonicon 1.1.1 firmware SysEx prefix")
    if data[-1] != END_OF_EXCLUSIVE:
        raise ValueError("firmware SysEx has no end-of-exclusive byte")
    if any(byte > 0x7F for byte in data[1:-1]):
        raise ValueError("firmware SysEx contains a non-MIDI-safe payload byte")

    cursor = len(WIRE_PREFIX)
    _require_zero_span(
        data[cursor : cursor + RESYNC_ZERO_BYTES], "initial preamble"
    )
    cursor += RESYNC_ZERO_BYTES
    if data[cursor] != START_OF_HEADER:
        raise ValueError("initial preamble has no record delimiter")
    cursor += 1
    _require_zero_span(
        data[cursor : cursor + RESYNC_ZERO_BYTES], "header preamble"
    )
    cursor += RESYNC_ZERO_BYTES
    if cursor != HEADER_RECORD_OFFSET or data[cursor] != START_OF_HEADER:
        raise ValueError("firmware header has no record delimiter")
    cursor += 1

    header_bytes = data[cursor : cursor + HEADER_WORD_COUNT * 3]
    header_words = _decode_words(header_bytes, HEADER_WORD_COUNT, "header")
    cursor += len(header_bytes)
    if header_words[:HEADER_DESTINATION_INDEX] != HEADER_FIXED_WORDS:
        raise ValueError("unknown firmware header identity or version")
    if header_words[15] != 0:
        raise ValueError("firmware header reserved word is nonzero")
    if header_words[HEADER_CHECKSUM_INDEX] != _checksum(
        header_words, HEADER_CHECKSUM_INDEX
    ):
        raise ValueError("firmware header checksum mismatch")
    header_address = (
        header_words[HEADER_DESTINATION_INDEX]
        | (header_words[HEADER_DESTINATION_INDEX + 1] << 16)
    )

    _require_zero_span(
        data[cursor : cursor + RESYNC_ZERO_BYTES], "header trailer"
    )
    cursor += RESYNC_ZERO_BYTES
    if cursor != DATA_RECORD_OFFSET:
        raise ValueError("unexpected firmware data offset")

    output = bytearray()
    records: list[RecordLayout] = []
    next_address = header_address
    for number in range(DATA_RECORD_COUNT):
        if data[cursor] != START_OF_HEADER:
            raise ValueError(f"record {number}: missing record delimiter")
        cursor += 1
        record_bytes = data[cursor : cursor + DATA_RECORD_WORD_COUNT * 3]
        words = _decode_words(record_bytes, DATA_RECORD_WORD_COUNT, f"record {number}")
        cursor += len(record_bytes)

        if words[0] != 0x13 or words[1] != DATA_RECORD_WORD_COUNT or words[3] != 0:
            raise ValueError(f"record {number}: unknown record type or length")
        count = words[2]
        if not 1 <= count <= DATA_RECORD_PAYLOAD_MAX:
            raise ValueError(f"record {number}: invalid payload length")
        address = words[4] | (words[5] << 16)
        if address != next_address:
            raise ValueError(
                f"record {number}: expected address {next_address:#08x}, got {address:#08x}"
            )
        if words[DATA_RECORD_CHECKSUM_INDEX] != _checksum(
            words[: DATA_RECORD_PAYLOAD_INDEX + count], DATA_RECORD_CHECKSUM_INDEX
        ):
            raise ValueError(f"record {number}: checksum mismatch")
        if any(words[DATA_RECORD_PAYLOAD_INDEX + count :]):
            raise ValueError(f"record {number}: nonzero record padding")

        payload = words[
            DATA_RECORD_PAYLOAD_INDEX : DATA_RECORD_PAYLOAD_INDEX + count
        ]
        output.extend(struct.pack(f"<{count}H", *payload))
        records.append(RecordLayout(address=address, word_count=count))
        next_address = address + count

        _require_zero_span(
            data[cursor : cursor + RESYNC_ZERO_BYTES], f"record {number} trailer"
        )
        cursor += RESYNC_ZERO_BYTES

    if data[cursor:] != bytes((START_OF_HEADER, END_OF_EXCLUSIVE)):
        raise ValueError("unexpected firmware SysEx trailer")
    if header_address != IMAGE_START_ADDRESS or next_address != IMAGE_END_ADDRESS:
        raise ValueError(
            f"unexpected firmware image extent {header_address:#08x}..{next_address - 1:#08x}"
        )
    if len(output) != IMAGE_BYTE_COUNT:
        raise ValueError("unexpected unpacked firmware image size")
    if any(record.word_count != DATA_RECORD_PAYLOAD_MAX for record in records[:-1]):
        raise ValueError("short firmware record before final record")
    if records[-1].word_count != IMAGE_WORD_COUNT % DATA_RECORD_PAYLOAD_MAX:
        raise ValueError("unexpected final firmware record length")

    return FirmwareImage(
        data=bytes(output),
        start_address=header_address,
        end_address=next_address,
        header_words=header_words,
        records=tuple(records),
    )


def pack_sysex(image: bytes, template: bytes) -> bytes:
    """Pack a fixed-size image using the validated vendor record layout."""
    if len(image) != IMAGE_BYTE_COUNT:
        raise ValueError(
            f"unexpected firmware image size: {len(image)} (expected {IMAGE_BYTE_COUNT})"
        )
    layout = unpack_sysex(template)
    image_words = struct.unpack(f"<{IMAGE_WORD_COUNT}H", image)

    output = bytearray(WIRE_PREFIX)
    output.extend(bytes(RESYNC_ZERO_BYTES))
    output.append(START_OF_HEADER)
    output.extend(bytes(RESYNC_ZERO_BYTES))
    output.append(START_OF_HEADER)

    header_words = list(layout.header_words)
    header_words[HEADER_CHECKSUM_INDEX] = 0
    header_words[HEADER_CHECKSUM_INDEX] = _checksum(
        header_words, HEADER_CHECKSUM_INDEX
    )
    output.extend(b"".join(encode_word(word) for word in header_words))
    output.extend(bytes(RESYNC_ZERO_BYTES))

    image_cursor = 0
    for record in layout.records:
        payload = image_words[image_cursor : image_cursor + record.word_count]
        words = [
            0x13,
            DATA_RECORD_WORD_COUNT,
            record.word_count,
            0,
            record.address & 0xFFFF,
            record.address >> 16,
            0,
            *payload,
        ]
        words.extend([0] * (DATA_RECORD_WORD_COUNT - len(words)))
        words[DATA_RECORD_CHECKSUM_INDEX] = _checksum(
            words[: DATA_RECORD_PAYLOAD_INDEX + record.word_count],
            DATA_RECORD_CHECKSUM_INDEX,
        )

        output.append(START_OF_HEADER)
        output.extend(b"".join(encode_word(word) for word in words))
        output.extend(bytes(RESYNC_ZERO_BYTES))
        image_cursor += record.word_count

    if image_cursor != IMAGE_WORD_COUNT:
        raise ValueError("template record layout does not consume the complete image")
    output.extend((START_OF_HEADER, END_OF_EXCLUSIVE))
    packed = bytes(output)
    if unpack_sysex(packed).data != image:
        raise AssertionError("internal firmware SysEx round-trip failure")
    return packed
