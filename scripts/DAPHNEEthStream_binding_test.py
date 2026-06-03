#!/usr/bin/env python3

import sys
from fddetdataformats import DAPHNEEthStreamFrame, DAPHNEEthStreamHeader, DAPHNEEthStreamChannelWord


NUM_CHANNELS = 4
ADCS_PER_CHANNEL = 280
ADC_MAX = (1 << 14) - 1


def test_construction_and_size() -> int:
    frame = DAPHNEEthStreamFrame()
    if frame is None:
        print("FAIL: DAPHNEEthStreamFrame default construction returned None")
        return 1
    size = DAPHNEEthStreamFrame.sizeof()
    if size <= 0:
        print(f"FAIL: DAPHNEEthStreamFrame.sizeof() returned {size}, expected > 0")
        return 1
    print(f"PASS: DAPHNEEthStreamFrame construction and sizeof={size}")
    return 0


def test_static_members() -> int:
    if DAPHNEEthStreamFrame.version != 1:
        print(f"FAIL: DAPHNEEthStreamFrame.version expected 1, got {DAPHNEEthStreamFrame.version}")
        return 1
    if DAPHNEEthStreamFrame.s_num_channels != NUM_CHANNELS:
        print(
            "FAIL: DAPHNEEthStreamFrame.s_num_channels expected "
            f"{NUM_CHANNELS}, got {DAPHNEEthStreamFrame.s_num_channels}"
        )
        return 1
    if DAPHNEEthStreamFrame.s_adcs_per_channel != ADCS_PER_CHANNEL:
        print(
            "FAIL: DAPHNEEthStreamFrame.s_adcs_per_channel expected "
            f"{ADCS_PER_CHANNEL}, got {DAPHNEEthStreamFrame.s_adcs_per_channel}"
        )
        return 1
    if DAPHNEEthStreamFrame.s_expected_bytes != DAPHNEEthStreamFrame.sizeof():
        print(
            "FAIL: DAPHNEEthStreamFrame.s_expected_bytes "
            f"{DAPHNEEthStreamFrame.s_expected_bytes} != sizeof {DAPHNEEthStreamFrame.sizeof()}"
        )
        return 1
    if DAPHNEEthStreamHeader.s_expected_bytes <= 0:
        print(f"FAIL: DAPHNEEthStreamHeader.s_expected_bytes invalid: {DAPHNEEthStreamHeader.s_expected_bytes}")
        return 1
    if DAPHNEEthStreamChannelWord.s_expected_bytes != 8:
        print(
            "FAIL: DAPHNEEthStreamChannelWord.s_expected_bytes expected 8, "
            f"got {DAPHNEEthStreamChannelWord.s_expected_bytes}"
        )
        return 1
    print("PASS: DAPHNEEthStreamFrame nested/static members")
    return 0


def test_bytes_roundtrip() -> int:
    frame = DAPHNEEthStreamFrame()
    frame.set_timestamp(0x1234ABCD)
    frame.set_channel(0, 7)
    frame.set_channel(1, 8)

    raw = frame.get_bytes()
    if len(raw) != DAPHNEEthStreamFrame.sizeof():
        print(f"FAIL: get_bytes() length {len(raw)} != sizeof() {DAPHNEEthStreamFrame.sizeof()}")
        return 1

    clone = DAPHNEEthStreamFrame(raw)
    if clone.get_timestamp() != 0x1234ABCD:
        print(f"FAIL: bytes round-trip timestamp mismatch: got {clone.get_timestamp():#x}")
        return 1
    if (clone.get_channel(0), clone.get_channel(1)) != (7, 8):
        print("FAIL: bytes round-trip channel mismatch")
        return 1

    print("PASS: DAPHNEEthStreamFrame bytes constructor roundtrip")
    return 0


def test_timestamp() -> int:
    frame = DAPHNEEthStreamFrame()
    test_ts = 0x998877665544
    frame.set_timestamp(test_ts)
    got = frame.get_timestamp()
    if got != test_ts:
        print(f"FAIL: set/get_timestamp: set {test_ts:#x}, got {got:#x}")
        return 1
    print("PASS: set_timestamp / get_timestamp")
    return 0


def test_channel() -> int:
    frame = DAPHNEEthStreamFrame()
    set_vals = [30, 31, 32, 33]

    for idx, val in enumerate(set_vals):
        frame.set_channel(idx, val)

    for idx, val in enumerate(set_vals):
        got = frame.get_channel(idx)
        if got != val:
            print(f"FAIL: get_channel({idx}) expected {val}, got {got}")
            return 1

    if (frame.get_channel0(), frame.get_channel1(), frame.get_channel2(), frame.get_channel3()) != tuple(set_vals):
        print("FAIL: get_channel0..3 mismatch")
        return 1

    print("PASS: set_channel / get_channel and get_channel0..3")
    return 0


def test_daqheader_accessible() -> int:
    frame = DAPHNEEthStreamFrame()
    hdr = frame.get_daqheader()
    if hdr is None:
        print("FAIL: get_daqheader() returned None")
        return 1
    print("PASS: get_daqheader() accessible")
    return 0


def test_set_geoid() -> int:
    frame = DAPHNEEthStreamFrame()
    crate_id = 6
    slot_id = 4
    stream_id = 21
    frame.set_geoid(crate_id, slot_id, stream_id)

    hdr = frame.get_daqheader()
    if hdr.crate_id != crate_id:
        print(f"FAIL: set_geoid crate_id mismatch, got {hdr.crate_id}")
        return 1
    if hdr.slot_id != slot_id:
        print(f"FAIL: set_geoid slot_id mismatch, got {hdr.slot_id}")
        return 1
    if hdr.stream_id != stream_id:
        print(f"FAIL: set_geoid stream_id mismatch, got {hdr.stream_id}")
        return 1

    print("PASS: set_geoid updates DAQEthHeader geoid fields")
    return 0


def test_channelword_properties() -> int:
    frame = DAPHNEEthStreamFrame()
    frame.set_channel(0, 33)
    frame.set_channel(1, 34)
    frame.set_channel(2, 35)
    frame.set_channel(3, 36)

    words = frame.get_header().channel_words
    if len(words) != NUM_CHANNELS:
        print(f"FAIL: expected {NUM_CHANNELS} channel words, got {len(words)}")
        return 1

    expected = [33, 34, 35, 36]
    for idx, exp in enumerate(expected):
        if words[idx].channel != exp:
            print(f"FAIL: ChannelWord[{idx}].channel mismatch, got {words[idx].channel}, expected {exp}")
            return 1
        if not isinstance(words[idx].version, int) or not isinstance(words[idx].tbd, int):
            print("FAIL: ChannelWord.version/tbd should be int")
            return 1

    print("PASS: DAPHNEEthStreamChannelWord properties via frame/header")
    return 0


def test_header_channel_words_property() -> int:
    frame = DAPHNEEthStreamFrame()
    hdr = frame.get_header()

    words = hdr.channel_words
    if len(words) != NUM_CHANNELS:
        print(f"FAIL: expected {NUM_CHANNELS} channel words, got {len(words)}")
        return 1

    print("PASS: DAPHNEEthStreamHeader.channel_words readable list property")
    return 0


def test_adc_single() -> int:
    frame = DAPHNEEthStreamFrame()
    frame.set_adc(0, 0, 111)
    got = frame.get_adc(0, 0)
    if got != 111:
        print(f"FAIL: set_adc(0,0,111) -> get_adc(0,0) returned {got}")
        return 1
    print("PASS: single ADC set/get (adc index 0, channel 0)")
    return 0


def test_adc_max_value() -> int:
    frame = DAPHNEEthStreamFrame()
    frame.set_adc(0, 0, ADC_MAX)
    got = frame.get_adc(0, 0)
    if got != ADC_MAX:
        print(f"FAIL: max ADC value {ADC_MAX}: got {got}")
        return 1
    print(f"PASS: max ADC value ({ADC_MAX}) preserved")
    return 0


def test_adc_independence() -> int:
    frame = DAPHNEEthStreamFrame()
    test_cases = [
        (0, 0, 100),
        (1, 0, 200),
        (0, 1, 300),
        (ADCS_PER_CHANNEL - 1, NUM_CHANNELS - 1, 9999),
    ]
    for adc_idx, channel_idx, val in test_cases:
        frame.set_adc(adc_idx, channel_idx, val)
    for adc_idx, channel_idx, expected in test_cases:
        got = frame.get_adc(adc_idx, channel_idx)
        if got != expected:
            print(f"FAIL: ADC independence: adc={adc_idx}, channel={channel_idx}: expected {expected}, got {got}")
            return 1
    print("PASS: ADC cell independence across multiple (adc, channel) pairs")
    return 0


def test_adc_all_channels_one_sample() -> int:
    frame = DAPHNEEthStreamFrame()
    adc_index = 0
    for channel_idx in range(NUM_CHANNELS):
        frame.set_adc(adc_index, channel_idx, (channel_idx + 1) * 100)
    for channel_idx in range(NUM_CHANNELS):
        expected = (channel_idx + 1) * 100
        got = frame.get_adc(adc_index, channel_idx)
        if got != expected:
            print(f"FAIL: all-channels test: channel {channel_idx}: expected {expected}, got {got}")
            return 1
    print(f"PASS: all {NUM_CHANNELS} channels read back correctly for adc index {adc_index}")
    return 0


def test_adc_out_of_range() -> int:
    frame = DAPHNEEthStreamFrame()
    try:
        frame.get_adc(ADCS_PER_CHANNEL, 0)
        print("FAIL: get_adc with out-of-range adc index should have raised an exception")
        return 1
    except Exception:
        pass
    try:
        frame.get_channel(NUM_CHANNELS)
        print("FAIL: get_channel with out-of-range channel index should have raised an exception")
        return 1
    except Exception:
        pass

    print("PASS: out-of-range channel/adc indices raise exceptions as expected")
    return 0


def main() -> int:
    tests = [
        test_construction_and_size,
        test_static_members,
        test_bytes_roundtrip,
        test_timestamp,
        test_channel,
        test_daqheader_accessible,
        test_set_geoid,
        test_channelword_properties,
        test_header_channel_words_property,
        test_adc_single,
        test_adc_max_value,
        test_adc_independence,
        test_adc_all_channels_one_sample,
        test_adc_out_of_range,
    ]

    failures = 0
    for test in tests:
        failures += test()

    print()
    if failures == 0:
        print("All DAPHNEEthStreamFrame Python binding tests passed")
    else:
        print(f"{failures} DAPHNEEthStreamFrame Python binding test(s) FAILED")
    return failures


if __name__ == "__main__":
    sys.exit(main())
