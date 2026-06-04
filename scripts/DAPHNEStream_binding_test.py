#!/usr/bin/env python3

import sys
from fddetdataformats import DAPHNEStreamFrame, DAPHNEStreamHeader


NUM_CHANNELS = 4
NUM_ADCS = 64
ADC_MAX = (1 << 14) - 1


def test_construction_and_size() -> int:
    frame = DAPHNEStreamFrame()
    if frame is None:
        print("FAIL: DAPHNEStreamFrame default construction returned None")
        return 1
    size = DAPHNEStreamFrame.sizeof()
    if size <= 0:
        print(f"FAIL: DAPHNEStreamFrame.sizeof() returned {size}, expected > 0")
        return 1
    print(f"PASS: DAPHNEStreamFrame construction and sizeof={size}")
    return 0


def test_static_members() -> int:
    if DAPHNEStreamFrame.s_channels_per_frame != NUM_CHANNELS:
        print(
            "FAIL: DAPHNEStreamFrame.s_channels_per_frame expected "
            f"{NUM_CHANNELS}, got {DAPHNEStreamFrame.s_channels_per_frame}"
        )
        return 1
    if DAPHNEStreamFrame.s_adcs_per_channel != NUM_ADCS:
        print(
            "FAIL: DAPHNEStreamFrame.s_adcs_per_channel expected "
            f"{NUM_ADCS}, got {DAPHNEStreamFrame.s_adcs_per_channel}"
        )
        return 1
    if DAPHNEStreamFrame.s_bits_per_adc != 14:
        print(f"FAIL: DAPHNEStreamFrame.s_bits_per_adc expected 14, got {DAPHNEStreamFrame.s_bits_per_adc}")
        return 1
    if DAPHNEStreamFrame.s_expected_bytes != DAPHNEStreamFrame.sizeof():
        print(
            "FAIL: DAPHNEStreamFrame.s_expected_bytes "
            f"{DAPHNEStreamFrame.s_expected_bytes} != sizeof {DAPHNEStreamFrame.sizeof()}"
        )
        return 1
    if DAPHNEStreamHeader.s_expected_bytes <= 0:
        print(f"FAIL: DAPHNEStreamHeader.s_expected_bytes invalid: {DAPHNEStreamHeader.s_expected_bytes}")
        return 1
    print("PASS: DAPHNEStreamFrame/DAPHNEStreamHeader static members")
    return 0


def test_bytes_roundtrip() -> int:
    frame = DAPHNEStreamFrame()
    frame.set_timestamp(0x1234ABCD)
    frame.get_header().channel_0 = 7
    frame.get_header().channel_1 = 8
    raw = frame.get_bytes()
    if len(raw) != DAPHNEStreamFrame.sizeof():
        print(f"FAIL: get_bytes() length {len(raw)} != sizeof() {DAPHNEStreamFrame.sizeof()}")
        return 1

    clone = DAPHNEStreamFrame(raw)
    if clone.get_timestamp() != 0x1234ABCD:
        print(f"FAIL: bytes round-trip timestamp mismatch: got {clone.get_timestamp():#x}")
        return 1
    if (clone.get_channel0(), clone.get_channel1()) != (7, 8):
        print("FAIL: bytes round-trip channel mismatch")
        return 1

    print("PASS: DAPHNEStreamFrame bytes constructor roundtrip")
    return 0


def test_less_than_operator() -> int:
    earlier = DAPHNEStreamFrame()
    later = DAPHNEStreamFrame()
    earlier.set_timestamp(100)
    later.set_timestamp(200)
    if not (earlier < later):
        print("FAIL: __lt__ expected earlier < later")
        return 1
    if later < earlier:
        print("FAIL: __lt__ expected later !< earlier")
        return 1
    print("PASS: __lt__ ordering by timestamp")
    return 0


def test_timestamp() -> int:
    frame = DAPHNEStreamFrame()
    test_ts = 0x998877665544
    frame.set_timestamp(test_ts)
    got = frame.get_timestamp()
    if got != test_ts:
        print(f"FAIL: set/get_timestamp: set {test_ts:#x}, got {got:#x}")
        return 1
    print("PASS: set_timestamp / get_timestamp")
    return 0


def test_channel() -> int:
    frame = DAPHNEStreamFrame()
    hdr = frame.get_header()
    hdr.channel_0 = 30
    hdr.channel_1 = 31
    hdr.channel_2 = 32
    hdr.channel_3 = 33

    if (frame.get_channel0(), frame.get_channel1(), frame.get_channel2(), frame.get_channel3()) != (30, 31, 32, 33):
        print("FAIL: get_channel0..3 mismatch")
        return 1

    print("PASS: channel_0..3 header properties and get_channel0..3")
    return 0


def test_daqheader_accessible() -> int:
    frame = DAPHNEStreamFrame()
    hdr = frame.get_daqheader()
    if hdr is None:
        print("FAIL: get_daqheader() returned None")
        return 1
    print("PASS: get_daqheader() accessible")
    return 0


def test_header_properties() -> int:
    frame = DAPHNEStreamFrame()
    hdr = frame.get_header()

    hdr.channel_0 = 10
    hdr.channel_1 = 11
    hdr.channel_2 = 12
    hdr.channel_3 = 13

    if (hdr.channel_0, hdr.channel_1, hdr.channel_2, hdr.channel_3) != (10, 11, 12, 13):
        print("FAIL: DAPHNEStreamHeader channel property mismatch")
        return 1

    print("PASS: DAPHNEStreamHeader properties")
    return 0


def test_adc_single() -> int:
    frame = DAPHNEStreamFrame()
    frame.set_adc(0, 0, 100)
    got = frame.get_adc(0, 0)
    if got != 100:
        print(f"FAIL: set_adc(0,0,100) -> get_adc(0,0) returned {got}")
        return 1
    print("PASS: single ADC set/get (adc index 0, channel 0)")
    return 0


def test_adc_max_value() -> int:
    frame = DAPHNEStreamFrame()
    frame.set_adc(0, 0, ADC_MAX)
    got = frame.get_adc(0, 0)
    if got != ADC_MAX:
        print(f"FAIL: max ADC value {ADC_MAX}: got {got}")
        return 1
    print(f"PASS: max ADC value ({ADC_MAX}) preserved")
    return 0


def test_adc_independence() -> int:
    frame = DAPHNEStreamFrame()
    test_cases = [
        (0, 0, 100),
        (1, 0, 200),
        (0, 1, 300),
        (NUM_ADCS - 1, NUM_CHANNELS - 1, 9999),
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
    frame = DAPHNEStreamFrame()
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
    frame = DAPHNEStreamFrame()
    try:
        frame.get_adc(NUM_ADCS, 0)
        print("FAIL: get_adc with out-of-range adc index should have raised an exception")
        return 1
    except Exception:
        pass
    try:
        frame.get_adc(0, NUM_CHANNELS)
        print("FAIL: get_adc with out-of-range channel index should have raised an exception")
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
        test_less_than_operator,
        test_timestamp,
        test_channel,
        test_daqheader_accessible,
        test_header_properties,
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
        print("All DAPHNEStreamFrame Python binding tests passed")
    else:
        print(f"{failures} DAPHNEStreamFrame Python binding test(s) FAILED")
    return failures


if __name__ == "__main__":
    sys.exit(main())
