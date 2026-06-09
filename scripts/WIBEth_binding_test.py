#!/usr/bin/env python3

import sys
from fddetdataformats import WIBEthFrame, WIBEthHeader


NUM_CHANNELS = 64
NUM_SAMPLES = 64
ADC_MAX = (1 << 14) - 1  # 14-bit ADC


def test_construction_and_size() -> int:
    frame = WIBEthFrame()
    if frame is None:
        print("FAIL: WIBEthFrame default construction returned None")
        return 1
    size = WIBEthFrame.sizeof()
    if size <= 0:
        print(f"FAIL: WIBEthFrame.sizeof() returned {size}, expected > 0")
        return 1
    print(f"PASS: WIBEthFrame construction and sizeof={size}")
    return 0


def test_static_members() -> int:
    if WIBEthFrame.s_bits_per_adc != 14:
        print(f"FAIL: WIBEthFrame.s_bits_per_adc expected 14, got {WIBEthFrame.s_bits_per_adc}")
        return 1
    if WIBEthFrame.s_num_channels != NUM_CHANNELS:
        print(f"FAIL: WIBEthFrame.s_num_channels expected {NUM_CHANNELS}, got {WIBEthFrame.s_num_channels}")
        return 1
    if WIBEthFrame.s_time_samples_per_frame != NUM_SAMPLES:
        print(f"FAIL: WIBEthFrame.s_time_samples_per_frame expected {NUM_SAMPLES}, got {WIBEthFrame.s_time_samples_per_frame}")
        return 1
    if WIBEthFrame.s_expected_bytes != WIBEthFrame.sizeof():
        print(f"FAIL: WIBEthFrame.s_expected_bytes {WIBEthFrame.s_expected_bytes} != sizeof {WIBEthFrame.sizeof()}")
        return 1
    if WIBEthHeader.s_expected_bytes <= 0:
        print(f"FAIL: WIBEthHeader.s_expected_bytes invalid: {WIBEthHeader.s_expected_bytes}")
        return 1
    print("PASS: WIBEthFrame/WIBEthHeader static members")
    return 0


def test_bytes_roundtrip() -> int:
    frame = WIBEthFrame()
    frame.set_timestamp(0xABCDEF012345)
    frame.set_channel(12)
    raw = frame.get_bytes()
    if len(raw) != WIBEthFrame.sizeof():
        print(f"FAIL: get_bytes() length {len(raw)} != sizeof() {WIBEthFrame.sizeof()}")
        return 1
    clone = WIBEthFrame(raw)
    if clone.get_timestamp() != 0xABCDEF012345:
        print(f"FAIL: bytes round-trip timestamp mismatch: got {clone.get_timestamp():#x}")
        return 1
    if clone.get_channel() != 12:
        print(f"FAIL: bytes round-trip channel mismatch: got {clone.get_channel()}")
        return 1
    print("PASS: WIBEthFrame bytes constructor roundtrip")
    return 0


def test_less_than_operator() -> int:
    earlier = WIBEthFrame()
    later = WIBEthFrame()
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
    frame = WIBEthFrame()
    test_ts = 0x123456789ABC
    frame.set_timestamp(test_ts)
    got = frame.get_timestamp()
    if got != test_ts:
        print(f"FAIL: set/get_timestamp: set {test_ts:#x}, got {got:#x}")
        return 1
    print("PASS: set_timestamp / get_timestamp")
    return 0


def test_channel() -> int:
    frame = WIBEthFrame()
    for val in (0, 1, 127, 255):
        frame.set_channel(val)
        got = frame.get_channel()
        if got != val:
            print(f"FAIL: set_channel({val}), get_channel() returned {got}")
            return 1
    print("PASS: set_channel / get_channel")
    return 0


def test_daqheader_accessible() -> int:
    frame = WIBEthFrame()
    hdr = frame.daq_header
    if hdr is None:
        print("FAIL: daq_header returned None")
        return 1
    hdr_alias = frame.get_daqheader()
    if hdr_alias is None:
        print("FAIL: get_daqheader() returned None")
        return 1
    print("PASS: daq_header accessible")
    return 0


def test_set_geoid() -> int:
    frame = WIBEthFrame()
    crate_id = 9
    slot_id = 7
    stream_id = 42
    hdr = frame.daq_header
    hdr.crate_id = crate_id
    hdr.slot_id = slot_id
    hdr.stream_id = stream_id
    if hdr.crate_id != crate_id:
        print(f"FAIL: crate_id mismatch, got {hdr.crate_id}")
        return 1
    if hdr.slot_id != slot_id:
        print(f"FAIL: slot_id mismatch, got {hdr.slot_id}")
        return 1
    if hdr.stream_id != stream_id:
        print(f"FAIL: stream_id mismatch, got {hdr.stream_id}")
        return 1
    print("PASS: DAQEthHeader geoid fields can be set directly")
    return 0


def test_header_properties() -> int:
    frame = WIBEthFrame()
    hdr = frame.header
    hdr_alias = frame.get_wibheader()

    if hdr_alias is None:
        print("FAIL: get_wibheader() returned None")
        return 1

    hdr.channel = 4
    hdr.version = 2
    hdr.cd = 1
    hdr.context = 3
    hdr.ready = 1
    hdr.calibration = 1
    hdr.pulser = 1
    hdr.femb_sync = 2
    hdr.wib_sync = 1
    hdr.lol = 1
    hdr.link_valid = 2
    hdr.crc_err = 1
    hdr.colddata_timestamp_1 = 99
    hdr.colddata_timestamp_0 = 77
    hdr.extra_data = 0x1122334455667788

    if hdr.channel != 4:
        print(f"FAIL: WIBEthHeader.channel mismatch, got {hdr.channel}")
        return 1
    if hdr.version != 2:
        print(f"FAIL: WIBEthHeader.version mismatch, got {hdr.version}")
        return 1
    if hdr.cd != 1:
        print(f"FAIL: WIBEthHeader.cd mismatch, got {hdr.cd}")
        return 1
    if hdr.context != 3:
        print(f"FAIL: WIBEthHeader.context mismatch, got {hdr.context}")
        return 1
    if hdr.ready != 1 or hdr.calibration != 1 or hdr.pulser != 1:
        print("FAIL: WIBEthHeader ready/calibration/pulser mismatch")
        return 1
    if hdr.femb_sync != 2 or hdr.wib_sync != 1:
        print("FAIL: WIBEthHeader femb_sync/wib_sync mismatch")
        return 1
    if hdr.lol != 1:
        print(f"FAIL: WIBEthHeader.lol mismatch, got {hdr.lol}")
        return 1
    if hdr.link_valid != 2 or hdr.crc_err != 1:
        print("FAIL: WIBEthHeader link_valid/crc_err mismatch")
        return 1
    if hdr.colddata_timestamp_1 != 99:
        print(f"FAIL: WIBEthHeader.colddata_timestamp_1 mismatch, got {hdr.colddata_timestamp_1}")
        return 1
    if hdr.colddata_timestamp_0 != 77:
        print(f"FAIL: WIBEthHeader.colddata_timestamp_0 mismatch, got {hdr.colddata_timestamp_0}")
        return 1
    if hdr.extra_data != 0x1122334455667788:
        print(f"FAIL: WIBEthHeader.extra_data mismatch, got {hdr.extra_data:#x}")
        return 1
    if hdr_alias.extra_data != 0x1122334455667788:
        print(f"FAIL: get_wibheader alias mismatch, got {hdr_alias.extra_data:#x}")
        return 1

    print("PASS: WIBEthHeader property read/write and get_wibheader alias")
    return 0


def test_adc_single() -> int:
    frame = WIBEthFrame()
    frame.set_adc(0, 0, 1234)
    got = frame.get_adc(0, 0)
    if got != 1234:
        print(f"FAIL: set_adc(0,0,1234) -> get_adc(0,0) returned {got}")
        return 1
    print("PASS: single ADC set/get (channel 0, sample 0)")
    return 0


def test_adc_max_value() -> int:
    frame = WIBEthFrame()
    frame.set_adc(0, 0, ADC_MAX)
    got = frame.get_adc(0, 0)
    if got != ADC_MAX:
        print(f"FAIL: max ADC value {ADC_MAX}: got {got}")
        return 1
    print(f"PASS: max ADC value ({ADC_MAX}) preserved")
    return 0


def test_adc_independence() -> int:
    """Verify that writing to one (channel, sample) cell does not corrupt its neighbours."""
    frame = WIBEthFrame()
    test_cases = [
        (0, 0, 100),
        (0, 1, 200),
        (1, 0, 300),
        (NUM_CHANNELS - 1, NUM_SAMPLES - 1, 9999),
    ]
    for ch, samp, val in test_cases:
        frame.set_adc(ch, samp, val)
    for ch, samp, expected in test_cases:
        got = frame.get_adc(ch, samp)
        if got != expected:
            print(f"FAIL: ADC independence: channel={ch}, sample={samp}: expected {expected}, got {got}")
            return 1
    print("PASS: ADC cell independence across multiple (channel, sample) pairs")
    return 0


def test_adc_all_channels_one_sample() -> int:
    """Write a unique value to every channel in sample 0 and verify all read back correctly."""
    frame = WIBEthFrame()
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
    frame = WIBEthFrame()
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
        print("All WIBEthFrame Python binding tests passed")
    else:
        print(f"{failures} WIBEthFrame Python binding test(s) FAILED")
    return failures


if __name__ == "__main__":
    sys.exit(main())
