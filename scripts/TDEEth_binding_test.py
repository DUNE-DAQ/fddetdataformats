#!/usr/bin/env python3

import sys
from fddetdataformats import TDEEthFrame, TDEEthHeader


NUM_CHANNELS = 64
NUM_SAMPLES = 64
ADC_MAX = (1 << 14) - 1  # 14-bit ADC


def test_construction_and_size() -> int:
    frame = TDEEthFrame()
    if frame is None:
        print("FAIL: TDEEthFrame default construction returned None")
        return 1
    size = TDEEthFrame.sizeof()
    if size <= 0:
        print(f"FAIL: TDEEthFrame.sizeof() returned {size}, expected > 0")
        return 1
    print(f"PASS: TDEEthFrame construction and sizeof={size}")
    return 0


def test_static_members() -> int:
    if TDEEthFrame.s_bits_per_adc != 14:
        print(f"FAIL: TDEEthFrame.s_bits_per_adc expected 14, got {TDEEthFrame.s_bits_per_adc}")
        return 1
    if TDEEthFrame.s_num_channels != NUM_CHANNELS:
        print(f"FAIL: TDEEthFrame.s_num_channels expected {NUM_CHANNELS}, got {TDEEthFrame.s_num_channels}")
        return 1
    if TDEEthFrame.s_time_samples_per_frame != NUM_SAMPLES:
        print(f"FAIL: TDEEthFrame.s_time_samples_per_frame expected {NUM_SAMPLES}, got {TDEEthFrame.s_time_samples_per_frame}")
        return 1
    if TDEEthFrame.s_expected_bytes != TDEEthFrame.sizeof():
        print(f"FAIL: TDEEthFrame.s_expected_bytes {TDEEthFrame.s_expected_bytes} != sizeof {TDEEthFrame.sizeof()}")
        return 1
    if TDEEthHeader.s_expected_bytes <= 0:
        print(f"FAIL: TDEEthHeader.s_expected_bytes invalid: {TDEEthHeader.s_expected_bytes}")
        return 1
    print("PASS: TDEEthFrame/TDEEthHeader static members")
    return 0


def test_bytes_roundtrip() -> int:
    frame = TDEEthFrame()
    frame.set_timestamp(0xDEADBEEFCAFE)
    raw = frame.get_bytes()
    if len(raw) != TDEEthFrame.sizeof():
        print(f"FAIL: get_bytes() length {len(raw)} != sizeof() {TDEEthFrame.sizeof()}")
        return 1
    clone = TDEEthFrame(raw)
    if clone.get_timestamp() != 0xDEADBEEFCAFE:
        print(f"FAIL: bytes round-trip timestamp mismatch: got {clone.get_timestamp():#x}")
        return 1
    print("PASS: TDEEthFrame bytes constructor roundtrip")
    return 0


def test_less_than_operator() -> int:
    earlier = TDEEthFrame()
    later = TDEEthFrame()
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
    frame = TDEEthFrame()
    test_ts = 0x123456789ABC
    frame.set_timestamp(test_ts)
    got = frame.get_timestamp()
    if got != test_ts:
        print(f"FAIL: set/get_timestamp: set {test_ts:#x}, got {got:#x}")
        return 1
    print("PASS: set_timestamp / get_timestamp")
    return 0


def test_channel() -> int:
    frame = TDEEthFrame()
    for val in (0, 1, 127, 255):
        frame.set_channel(val)
        got = frame.get_channel()
        if got != val:
            print(f"FAIL: set_channel({val}), get_channel() returned {got}")
            return 1
    print("PASS: set_channel / get_channel")
    return 0


def test_daqheader_accessible() -> int:
    frame = TDEEthFrame()
    hdr = frame.get_daqheader()
    if hdr is None:
        print("FAIL: get_daqheader() returned None")
        return 1
    print("PASS: get_daqheader() accessible")
    return 0


def test_set_geoid() -> int:
    frame = TDEEthFrame()
    crate_id = 11
    slot_id = 12
    stream_id = 33
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


def test_header_properties() -> int:
    frame = TDEEthFrame()
    hdr = frame.get_tdeheader()

    hdr.channel = 42
    if hdr.channel != 42:
        print(f"FAIL: TDEEthHeader.channel: set 42, got {hdr.channel}")
        return 1

    hdr.version = 3
    if hdr.version != 3:
        print(f"FAIL: TDEEthHeader.version: set 3, got {hdr.version}")
        return 1

    hdr.tde_header = 0x1F
    if hdr.tde_header != 0x1F:
        print(f"FAIL: TDEEthHeader.tde_header: set 0x1F, got {hdr.tde_header:#x}")
        return 1

    hdr.tde_errors = 0xAB
    if hdr.tde_errors != 0xAB:
        print(f"FAIL: TDEEthHeader.tde_errors: set 0xAB, got {hdr.tde_errors:#x}")
        return 1

    hdr.TAItime = 0xFEDCBA9876543210
    if hdr.TAItime != 0xFEDCBA9876543210:
        print(f"FAIL: TDEEthHeader.TAItime: set 0xFEDCBA9876543210, got {hdr.TAItime:#x}")
        return 1

    print("PASS: TDEEthHeader property read/write (channel, version, tde_header, tde_errors, TAItime)")
    return 0


def test_adc_single() -> int:  # noqa: E302
    frame = TDEEthFrame()
    # Write and read back a single ADC value in channel 0, sample 0
    frame.set_adc(0, 0, 1234)
    got = frame.get_adc(0, 0)
    if got != 1234:
        print(f"FAIL: set_adc(0,0,1234) -> get_adc(0,0) returned {got}")
        return 1
    print("PASS: single ADC set/get (channel 0, sample 0)")
    return 0


def test_adc_max_value() -> int:
    frame = TDEEthFrame()
    frame.set_adc(0, 0, ADC_MAX)
    got = frame.get_adc(0, 0)
    if got != ADC_MAX:
        print(f"FAIL: max ADC value {ADC_MAX}: got {got}")
        return 1
    print(f"PASS: max ADC value ({ADC_MAX}) preserved")
    return 0


def test_adc_independence() -> int:
    """Verify that writing to one (channel, sample) cell does not corrupt its neighbours."""
    frame = TDEEthFrame()

    # Write distinct values to several cells
    test_cases = [
        (0, 0, 100),
        (0, 1, 200),
        (1, 0, 300),
        (NUM_CHANNELS - 1, NUM_SAMPLES - 1, 9999),
    ]
    for ch, samp, val in test_cases:
        frame.set_adc(ch, samp, val)

    # Read them all back and check none leaked into each other
    for ch, samp, expected in test_cases:
        got = frame.get_adc(ch, samp)
        if got != expected:
            print(f"FAIL: ADC independence: channel={ch}, sample={samp}: expected {expected}, got {got}")
            return 1

    print("PASS: ADC cell independence across multiple (channel, sample) pairs")
    return 0


def test_adc_all_channels_one_sample() -> int:
    """Write a unique value to every channel in sample 0 and verify all read back correctly."""
    frame = TDEEthFrame()
    for ch in range(NUM_CHANNELS):
        frame.set_adc(ch, 0, ch * 100 % (ADC_MAX + 1))
    for ch in range(NUM_CHANNELS):
        expected = ch * 100 % (ADC_MAX + 1)
        got = frame.get_adc(ch, 0)
        if got != expected:
            print(f"FAIL: all-channels test: channel {ch}: expected {expected}, got {got}")
            return 1
    print(f"PASS: all {NUM_CHANNELS} channels read back correctly for sample 0")
    return 0


def test_adc_out_of_range() -> int:
    """get_adc / set_adc should raise when indices are out of range."""
    frame = TDEEthFrame()
    try:
        frame.get_adc(NUM_CHANNELS, 0)
        print("FAIL: get_adc with out-of-range channel should have raised an exception")
        return 1
    except Exception:
        pass
    try:
        frame.get_adc(0, NUM_SAMPLES)
        print("FAIL: get_adc with out-of-range sample should have raised an exception")
        return 1
    except Exception:
        pass

    print("PASS: out-of-range ADC indices raise exceptions as expected")
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
        print("All TDEEthFrame Python binding tests passed")
    else:
        print(f"{failures} TDEEthFrame Python binding test(s) FAILED")
    return failures


if __name__ == "__main__":
    sys.exit(main())
