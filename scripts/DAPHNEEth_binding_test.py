#!/usr/bin/env python3

import sys
from fddetdataformats import DAPHNEEthFrame, DAPHNEEthHeader


NUM_CHANNELS = 1
ADCS_PER_CHANNEL = 1024
ADC_MAX = (1 << 14) - 1


def test_construction_and_size() -> int:
    frame = DAPHNEEthFrame()
    if frame is None:
        print("FAIL: DAPHNEEthFrame default construction returned None")
        return 1
    size = DAPHNEEthFrame.sizeof()
    if size <= 0:
        print(f"FAIL: DAPHNEEthFrame.sizeof() returned {size}, expected > 0")
        return 1
    print(f"PASS: DAPHNEEthFrame construction and sizeof={size}")
    return 0


def test_static_members() -> int:
    if DAPHNEEthFrame.version != 1:
        print(f"FAIL: DAPHNEEthFrame.version expected 1, got {DAPHNEEthFrame.version}")
        return 1
    if DAPHNEEthFrame.s_num_adcs != ADCS_PER_CHANNEL:
        print(
            "FAIL: DAPHNEEthFrame.s_num_adcs expected "
            f"{ADCS_PER_CHANNEL}, got {DAPHNEEthFrame.s_num_adcs}"
        )
        return 1
    if DAPHNEEthFrame.s_bits_per_adc != 14:
        print(f"FAIL: DAPHNEEthFrame.s_bits_per_adc expected 14, got {DAPHNEEthFrame.s_bits_per_adc}")
        return 1
    if DAPHNEEthFrame.s_expected_bytes != DAPHNEEthFrame.sizeof():
        print(f"FAIL: DAPHNEEthFrame.s_expected_bytes {DAPHNEEthFrame.s_expected_bytes} != sizeof {DAPHNEEthFrame.sizeof()}")
        return 1
    if DAPHNEEthHeader.s_expected_bytes <= 0:
        print(f"FAIL: DAPHNEEthHeader.s_expected_bytes invalid: {DAPHNEEthHeader.s_expected_bytes}")
        return 1
    print("PASS: DAPHNEEthFrame/DAPHNEEthHeader static members")
    return 0


def test_bytes_roundtrip() -> int:
    frame = DAPHNEEthFrame()
    frame.set_timestamp(0x112233445566)
    frame.set_channel(9)
    raw = frame.get_bytes()
    if len(raw) != DAPHNEEthFrame.sizeof():
        print(f"FAIL: get_bytes() length {len(raw)} != sizeof() {DAPHNEEthFrame.sizeof()}")
        return 1

    clone = DAPHNEEthFrame(raw)
    if clone.get_timestamp() != 0x112233445566:
        print(f"FAIL: bytes round-trip timestamp mismatch: got {clone.get_timestamp():#x}")
        return 1
    if clone.get_channel() != 9:
        print(f"FAIL: bytes round-trip channel mismatch: got {clone.get_channel()}")
        return 1

    print("PASS: DAPHNEEthFrame bytes constructor roundtrip")
    return 0


def test_less_than_operator() -> int:
    earlier = DAPHNEEthFrame()
    later = DAPHNEEthFrame()
    earlier.set_timestamp(100)
    later.set_timestamp(200)
    if not (earlier < later):
        print("FAIL: __lt__ expected earlier < later")
        return 1
    if later < earlier:
        print("FAIL: __lt__ expected later !< earlier")
        return 1
    print("PASS: __lt__ ordering by timestamp/channel tuple")
    return 0


def test_timestamp() -> int:
    frame = DAPHNEEthFrame()
    test_ts = 0xAABBCCDDEEFF
    frame.set_timestamp(test_ts)
    got = frame.get_timestamp()
    if got != test_ts:
        print(f"FAIL: set/get_timestamp: set {test_ts:#x}, got {got:#x}")
        return 1
    print("PASS: set_timestamp / get_timestamp")
    return 0


def test_channel() -> int:
    frame = DAPHNEEthFrame()
    for val in (0, 1, 127, 255):
        frame.set_channel(val)
        got = frame.get_channel()
        if got != val:
            print(f"FAIL: set_channel({val}), get_channel() returned {got}")
            return 1
    print("PASS: set_channel / get_channel")
    return 0


def test_daqheader_accessible() -> int:
    frame = DAPHNEEthFrame()
    hdr = frame.get_daqheader()
    if hdr is None:
        print("FAIL: get_daqheader() returned None")
        return 1
    print("PASS: get_daqheader() accessible")
    return 0


def test_header_properties() -> int:
    frame = DAPHNEEthFrame()
    hdr = frame.get_header()

    hdr.channel = 5
    hdr.version = 1
    hdr.trigger_sample_value = 123
    hdr.threshold = 456
    hdr.baseline = 789
    hdr.w1 = 11
    hdr.w2 = 22
    hdr.w3 = 33
    hdr.w4 = 44
    hdr.w5 = 55
    hdr.w6 = 66

    if hdr.channel != 5 or frame.get_channel() != 5:
        print("FAIL: header.channel mismatch")
        return 1
    if hdr.version != 1:
        print("FAIL: header.version mismatch")
        return 1
    if hdr.trigger_sample_value != 123:
        print("FAIL: header.trigger_sample_value mismatch")
        return 1
    if hdr.threshold != 456:
        print("FAIL: header.threshold mismatch")
        return 1
    if hdr.baseline != 789:
        print("FAIL: header.baseline mismatch")
        return 1
    if (hdr.w1, hdr.w2, hdr.w3, hdr.w4, hdr.w5, hdr.w6) != (11, 22, 33, 44, 55, 66):
        print("FAIL: w1..w6 mismatch")
        return 1

    print("PASS: DAPHNEEthHeader properties")
    return 0


def test_set_geoid() -> int:
    frame = DAPHNEEthFrame()
    crate_id = 5
    slot_id = 3
    stream_id = 12
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


def test_adc_single() -> int:
    frame = DAPHNEEthFrame()
    frame.set_adc(0, 321)
    if frame.get_adc(0) != 321:
        print(f"FAIL: set_adc(0,321) -> get_adc(0) returned {frame.get_adc(0)}")
        return 1
    print("PASS: single ADC set/get (adc index 0)")
    return 0


def test_adc_max_value() -> int:
    frame = DAPHNEEthFrame()
    frame.set_adc(0, ADC_MAX)
    got = frame.get_adc(0)
    if got != ADC_MAX:
        print(f"FAIL: max ADC value {ADC_MAX}: got {got}")
        return 1
    print(f"PASS: max ADC value ({ADC_MAX}) preserved")
    return 0


def test_adc_independence() -> int:
    frame = DAPHNEEthFrame()
    test_cases = [
        (0, 100),
        (1, 200),
        (2, 300),
        (ADCS_PER_CHANNEL - 1, 9999),
    ]
    for adc_idx, val in test_cases:
        frame.set_adc(adc_idx, val)
    for adc_idx, expected in test_cases:
        got = frame.get_adc(adc_idx)
        if got != expected:
            print(f"FAIL: ADC independence: index={adc_idx}: expected {expected}, got {got}")
            return 1
    print("PASS: ADC cell independence across multiple indices")
    return 0


def test_adc_all_channels_one_sample() -> int:
    frame = DAPHNEEthFrame()
    for adc_idx in range(ADCS_PER_CHANNEL):
        frame.set_adc(adc_idx, adc_idx * 100 % (ADC_MAX + 1))
    for adc_idx in range(ADCS_PER_CHANNEL):
        expected = adc_idx * 100 % (ADC_MAX + 1)
        got = frame.get_adc(adc_idx)
        if got != expected:
            print(f"FAIL: all-adcs test: adc index {adc_idx}: expected {expected}, got {got}")
            return 1
    print(f"PASS: all {ADCS_PER_CHANNEL} ADC values read back correctly")
    return 0


def test_adc_out_of_range() -> int:
    frame = DAPHNEEthFrame()
    try:
        frame.get_adc(ADCS_PER_CHANNEL)
        print("FAIL: get_adc with out-of-range index should have raised an exception")
        return 1
    except Exception:
        pass
    print("PASS: out-of-range ADC index raises exception as expected")
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
        test_set_geoid,
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
        print("All DAPHNEEthFrame Python binding tests passed")
    else:
        print(f"{failures} DAPHNEEthFrame Python binding test(s) FAILED")
    return failures


if __name__ == "__main__":
    sys.exit(main())
