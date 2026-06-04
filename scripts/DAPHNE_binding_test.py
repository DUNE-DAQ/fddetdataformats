#!/usr/bin/env python3

import sys
from fddetdataformats import DAPHNEFrame, DAPHNEFrameHeader, DAPHNEFramePeakDescriptorData


NUM_CHANNELS = 1
NUM_ADCS = 1024
ADC_MAX = (1 << 14) - 1


def test_construction_and_size() -> int:
    frame = DAPHNEFrame()
    if frame is None:
        print("FAIL: DAPHNEFrame default construction returned None")
        return 1

    size = DAPHNEFrame.sizeof()
    if size <= 0:
        print(f"FAIL: DAPHNEFrame.sizeof() returned {size}, expected > 0")
        return 1

    print("PASS: DAPHNEFrame construction and sizeof")
    return 0


def test_static_members() -> int:
    if DAPHNEFrame.version != 2:
        print(f"FAIL: DAPHNEFrame.version expected 2, got {DAPHNEFrame.version}")
        return 1
    if DAPHNEFrame.s_num_adcs != NUM_ADCS:
        print(f"FAIL: DAPHNEFrame.s_num_adcs expected {NUM_ADCS}, got {DAPHNEFrame.s_num_adcs}")
        return 1
    if DAPHNEFrame.s_bits_per_adc != 14:
        print(f"FAIL: DAPHNEFrame.s_bits_per_adc expected 14, got {DAPHNEFrame.s_bits_per_adc}")
        return 1
    if DAPHNEFrame.s_expected_bytes != DAPHNEFrame.sizeof():
        print(f"FAIL: DAPHNEFrame.s_expected_bytes {DAPHNEFrame.s_expected_bytes} != sizeof {DAPHNEFrame.sizeof()}")
        return 1
    if DAPHNEFrameHeader.s_expected_bytes <= 0:
        print(f"FAIL: DAPHNEFrameHeader.s_expected_bytes invalid: {DAPHNEFrameHeader.s_expected_bytes}")
        return 1
    if DAPHNEFramePeakDescriptorData.s_expected_bytes <= 0:
        print(
            "FAIL: DAPHNEFramePeakDescriptorData.s_expected_bytes invalid: "
            f"{DAPHNEFramePeakDescriptorData.s_expected_bytes}"
        )
        return 1
    if DAPHNEFramePeakDescriptorData.max_peaks != 5:
        print(f"FAIL: DAPHNEFramePeakDescriptorData.max_peaks expected 5, got {DAPHNEFramePeakDescriptorData.max_peaks}")
        return 1
    print("PASS: DAPHNEFrame nested/static members")
    return 0


def test_bytes_roundtrip() -> int:
    frame = DAPHNEFrame()
    frame.set_timestamp(0x112233445566)
    frame.set_channel(9)
    raw = frame.get_bytes()
    if len(raw) != DAPHNEFrame.sizeof():
        print(f"FAIL: get_bytes() length {len(raw)} != sizeof() {DAPHNEFrame.sizeof()}")
        return 1
    clone = DAPHNEFrame(raw)
    if clone.get_timestamp() != 0x112233445566:
        print(f"FAIL: bytes round-trip timestamp mismatch: got {clone.get_timestamp():#x}")
        return 1
    if clone.get_channel() != 9:
        print(f"FAIL: bytes round-trip channel mismatch: got {clone.get_channel()}")
        return 1
    print("PASS: DAPHNEFrame bytes constructor roundtrip")
    return 0


def test_less_than_operator() -> int:
    earlier = DAPHNEFrame()
    later = DAPHNEFrame()
    earlier.set_timestamp(100)
    later.set_timestamp(200)
    if not (earlier < later):
        print("FAIL: __lt__ expected earlier < later by timestamp")
        return 1
    if later < earlier:
        print("FAIL: __lt__ expected later !< earlier by timestamp")
        return 1

    same_ts_low_ch = DAPHNEFrame()
    same_ts_high_ch = DAPHNEFrame()
    same_ts_low_ch.set_timestamp(300)
    same_ts_high_ch.set_timestamp(300)
    same_ts_low_ch.set_channel(1)
    same_ts_high_ch.set_channel(2)
    if not (same_ts_low_ch < same_ts_high_ch):
        print("FAIL: __lt__ expected channel tie-breaker low < high")
        return 1
    print("PASS: __lt__ ordering by timestamp then channel")
    return 0


def test_timestamp() -> int:
    frame = DAPHNEFrame()
    test_ts = 0xAABBCCDDEEFF
    frame.set_timestamp(test_ts)
    got = frame.get_timestamp()
    if got != test_ts:
        print(f"FAIL: set/get_timestamp: set {test_ts:#x}, got {got:#x}")
        return 1
    print("PASS: set_timestamp / get_timestamp")
    return 0


def test_channel() -> int:
    frame = DAPHNEFrame()
    for val in (0, 1, 31, 63):
        frame.set_channel(val)
        got = frame.get_channel()
        if got != val:
            print(f"FAIL: set_channel({val}), get_channel() returned {got}")
            return 1
    print("PASS: set_channel / get_channel")
    return 0


def test_daqheader_accessible() -> int:
    frame = DAPHNEFrame()
    hdr = frame.get_daqheader()
    if hdr is None:
        print("FAIL: get_daqheader() returned None")
        return 1
    print("PASS: get_daqheader() accessible")
    return 0


def test_set_geoid() -> int:
    frame = DAPHNEFrame()
    crate_id = 8
    slot_id = 5
    link_id = 12
    frame.set_geoid(crate_id, slot_id, link_id)

    hdr = frame.get_daqheader()
    if hdr.crate_id != crate_id:
        print(f"FAIL: set_geoid crate_id mismatch, got {hdr.crate_id}")
        return 1
    if hdr.slot_id != slot_id:
        print(f"FAIL: set_geoid slot_id mismatch, got {hdr.slot_id}")
        return 1
    if hdr.link_id != link_id:
        print(f"FAIL: set_geoid link_id mismatch, got {hdr.link_id}")
        return 1

    print("PASS: DAPHNEFrame set_geoid updates DAQHeader geoid fields")
    return 0


def test_header_properties() -> int:
    frame = DAPHNEFrame()
    header = frame.get_header()

    header.channel = 17
    if frame.get_channel() != 17:
        print(f"FAIL: header.channel set/get mismatch, got {frame.get_channel()}")
        return 1

    header.algorithm_id = 3
    if header.algorithm_id != 3:
        print(f"FAIL: header.algorithm_id mismatch, got {header.algorithm_id}")
        return 1

    header.r1 = 1
    if header.r1 != 1:
        print(f"FAIL: header.r1 mismatch, got {header.r1}")
        return 1

    header.trigger_sample_value = 512
    if header.trigger_sample_value != 512:
        print(f"FAIL: header.trigger_sample_value mismatch, got {header.trigger_sample_value}")
        return 1

    header.threshold = 1200
    if header.threshold != 1200:
        print(f"FAIL: header.threshold mismatch, got {header.threshold}")
        return 1

    header.baseline = 900
    if header.baseline != 900:
        print(f"FAIL: header.baseline mismatch, got {header.baseline}")
        return 1

    print("PASS: DAPHNEFrameHeader properties")
    return 0


def test_adc_single() -> int:
    frame = DAPHNEFrame()
    frame.set_adc(0, 321)
    if frame.get_adc(0) != 321:
        print(f"FAIL: set_adc/get_adc mismatch at index 0, got {frame.get_adc(0)}")
        return 1
    print("PASS: single ADC set/get (adc index 0)")
    return 0


def test_adc_max_value() -> int:
    frame = DAPHNEFrame()
    frame.set_adc(0, ADC_MAX)
    got = frame.get_adc(0)
    if got != ADC_MAX:
        print(f"FAIL: max ADC value {ADC_MAX}: got {got}")
        return 1
    print(f"PASS: max ADC value ({ADC_MAX}) preserved")
    return 0


def test_adc_independence() -> int:
    frame = DAPHNEFrame()
    test_cases = [
        (0, 100),
        (1, 200),
        (2, 300),
        (NUM_ADCS - 1, 9999),
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
    frame = DAPHNEFrame()
    for adc_idx in range(NUM_ADCS):
        frame.set_adc(adc_idx, adc_idx * 100 % (ADC_MAX + 1))
    for adc_idx in range(NUM_ADCS):
        expected = adc_idx * 100 % (ADC_MAX + 1)
        got = frame.get_adc(adc_idx)
        if got != expected:
            print(f"FAIL: all-adcs test: adc index {adc_idx}: expected {expected}, got {got}")
            return 1
    print(f"PASS: all {NUM_ADCS} ADC values read back correctly")
    return 0


def test_adc_out_of_range() -> int:
    frame = DAPHNEFrame()
    try:
        frame.get_adc(NUM_ADCS)
        print("FAIL: get_adc with out-of-range index should have raised an exception")
        return 1
    except Exception:
        pass
    print("PASS: out-of-range ADC index raises exception as expected")
    return 0


def test_peaks_data() -> int:
    frame = DAPHNEFrame()
    peaks = frame.get_peaks_data()

    peaks.set_found(1, 0)
    if peaks.is_found(0) != 1:
        print(f"FAIL: peaks found(0) expected 1, got {peaks.is_found(0)}")
        return 1

    peaks.set_adc_integral(12345, 1)
    if peaks.get_adc_integral(1) != 12345:
        print(f"FAIL: adc_integral(1) mismatch, got {peaks.get_adc_integral(1)}")
        return 1

    peaks.set_num_subpeaks(7, 2)
    if peaks.get_num_subpeaks(2) != 7:
        print(f"FAIL: num_subpeaks(2) mismatch, got {peaks.get_num_subpeaks(2)}")
        return 1

    peaks.set_samples_over_baseline(200, 3)
    if peaks.get_samples_over_baseline(3) != 200:
        print(f"FAIL: samples_over_baseline(3) mismatch, got {peaks.get_samples_over_baseline(3)}")
        return 1

    peaks.set_adc_max(4000, 4)
    if peaks.get_adc_max(4) != 4000:
        print(f"FAIL: adc_max(4) mismatch, got {peaks.get_adc_max(4)}")
        return 1

    peaks.set_sample_max(123, 4)
    if peaks.get_sample_max(4) != 123:
        print(f"FAIL: sample_max(4) mismatch, got {peaks.get_sample_max(4)}")
        return 1

    peaks.set_sample_start(777, 0)
    if peaks.get_sample_start(0) != 777:
        print(f"FAIL: sample_start(0) mismatch, got {peaks.get_sample_start(0)}")
        return 1

    peaks.found_0 = 1
    if peaks.found_0 != 1:
        print(f"FAIL: found_0 property mismatch, got {peaks.found_0}")
        return 1

    peaks.adc_integral_0 = 54321
    if peaks.adc_integral_0 != 54321:
        print(f"FAIL: adc_integral_0 property mismatch, got {peaks.adc_integral_0}")
        return 1

    print("PASS: DAPHNEFramePeakDescriptorData methods and properties")
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
        test_peaks_data,
    ]

    failures = 0
    for test in tests:
        failures += test()

    print()
    if failures == 0:
        print("All DAPHNEFrame Python binding tests passed")
    else:
        print(f"{failures} DAPHNEFrame Python binding test(s) FAILED")
    return failures


if __name__ == "__main__":
    sys.exit(main())
