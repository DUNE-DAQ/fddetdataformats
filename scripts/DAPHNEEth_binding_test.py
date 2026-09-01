#!/usr/bin/env python3

import sys
from fddetdataformats import (
    DAPHNEEthFrame,
    DAPHNEEthHeader,
    DAPHNEEthFramePeakDescriptor,
    DAPHNEEthFramePeakDescriptorData,
)


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
            f"FAIL: DAPHNEEthFrame.s_num_adcs expected {ADCS_PER_CHANNEL}, "
            f"got {DAPHNEEthFrame.s_num_adcs}"
        )
        return 1
    if DAPHNEEthFrame.s_bits_per_adc != 14:
        print(f"FAIL: DAPHNEEthFrame.s_bits_per_adc expected 14, got {DAPHNEEthFrame.s_bits_per_adc}")
        return 1
    if DAPHNEEthFrame.s_expected_bytes != DAPHNEEthFrame.sizeof():
        print(
            f"FAIL: DAPHNEEthFrame.s_expected_bytes {DAPHNEEthFrame.s_expected_bytes} "
            f"!= sizeof {DAPHNEEthFrame.sizeof()}"
        )
        return 1
    if DAPHNEEthFrame.s_max_peaks != 5:
        print(f"FAIL: DAPHNEEthFrame.s_max_peaks expected 5, got {DAPHNEEthFrame.s_max_peaks}")
        return 1
    if DAPHNEEthHeader.s_expected_bytes <= 0:
        print(f"FAIL: DAPHNEEthHeader.s_expected_bytes invalid: {DAPHNEEthHeader.s_expected_bytes}")
        return 1
    if DAPHNEEthFramePeakDescriptor.s_expected_bytes <= 0:
        print(
            f"FAIL: DAPHNEEthFramePeakDescriptor.s_expected_bytes invalid: "
            f"{DAPHNEEthFramePeakDescriptor.s_expected_bytes}"
        )
        return 1
    if DAPHNEEthFramePeakDescriptorData.s_expected_bytes <= 0:
        print(
            f"FAIL: DAPHNEEthFramePeakDescriptorData.s_expected_bytes invalid: "
            f"{DAPHNEEthFramePeakDescriptorData.s_expected_bytes}"
        )
        return 1
    print("PASS: DAPHNEEthFrame/DAPHNEEthHeader/PeakDescriptor static members")
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


def test_header_properties() -> int:
    frame = DAPHNEEthFrame()
    hdr = frame.header
    hdr_alias = frame.get_daphneheader()

    if hdr_alias is None:
        print("FAIL: get_daphneheader() returned None")
        return 1

    hdr.channel = 5
    hdr.version = 1
    hdr.trigger_sample_value = 123
    hdr.threshold = 456
    hdr.baseline = 789

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

    peaks = hdr.peaks_data
    if peaks is None:
        print("FAIL: header.peaks_data returned None")
        return 1
    if not isinstance(peaks, DAPHNEEthFramePeakDescriptorData):
        print(f"FAIL: header.peaks_data is not DAPHNEEthFramePeakDescriptorData, got {type(peaks)}")
        return 1

    print("PASS: DAPHNEEthHeader properties")
    return 0


def test_peaks_data_accessible() -> int:
    frame = DAPHNEEthFrame()

    peaks_via_method = frame.get_peaks_data()
    if peaks_via_method is None:
        print("FAIL: get_peaks_data() returned None")
        return 1
    if not isinstance(peaks_via_method, DAPHNEEthFramePeakDescriptorData):
        print(f"FAIL: get_peaks_data() returned unexpected type {type(peaks_via_method)}")
        return 1

    peaks_via_header = frame.header.peaks_data
    if peaks_via_header is None:
        print("FAIL: header.peaks_data returned None")
        return 1
    if not isinstance(peaks_via_header, DAPHNEEthFramePeakDescriptorData):
        print(f"FAIL: header.peaks_data returned unexpected type {type(peaks_via_header)}")
        return 1

    # Both paths should refer to the same underlying data
    peaks_via_method.set_adc_integral(12345, 0)
    if peaks_via_header.get_adc_integral(0) != 12345:
        print("FAIL: get_peaks_data() and header.peaks_data do not alias the same data")
        return 1

    print("PASS: peaks_data accessible via get_peaks_data() and header.peaks_data")
    return 0


def test_peak_descriptor_data_mutators() -> int:
    frame = DAPHNEEthFrame()
    n_peaks = DAPHNEEthFrame.s_max_peaks
    peaks = frame.get_peaks_data()

    # Field bit widths: num_subpeaks=4, adc_integral=23, adc_max=14,
    # sample_max=9, samples_over_baseline=9, sample_start=10
    test_vals = [
        dict(found=True,  num_subpeaks=i,      adc_integral=100000 + i*1000,
             adc_max=1000 + i*100, sample_max=200 + i*10,
             samples_over_baseline=50 + i*5, sample_start=300 + i*7)
        for i in range(n_peaks)
    ]

    for peak, v in enumerate(test_vals):
        peaks.set_found(v["found"], peak)
        peaks.set_num_subpeaks(v["num_subpeaks"], peak)
        peaks.set_adc_integral(v["adc_integral"], peak)
        peaks.set_adc_max(v["adc_max"], peak)
        peaks.set_sample_max(v["sample_max"], peak)
        peaks.set_samples_over_baseline(v["samples_over_baseline"], peak)
        peaks.set_sample_start(v["sample_start"], peak)

    for peak, v in enumerate(test_vals):
        if peaks.is_found(peak) != v["found"]:
            print(f"FAIL: is_found({peak}) expected {v['found']}, got {peaks.is_found(peak)}")
            return 1
        if peaks.get_num_subpeaks(peak) != v["num_subpeaks"]:
            print(f"FAIL: get_num_subpeaks({peak}) expected {v['num_subpeaks']}, got {peaks.get_num_subpeaks(peak)}")
            return 1
        if peaks.get_adc_integral(peak) != v["adc_integral"]:
            print(f"FAIL: get_adc_integral({peak}) expected {v['adc_integral']}, got {peaks.get_adc_integral(peak)}")
            return 1
        if peaks.get_adc_max(peak) != v["adc_max"]:
            print(f"FAIL: get_adc_max({peak}) expected {v['adc_max']}, got {peaks.get_adc_max(peak)}")
            return 1
        if peaks.get_sample_max(peak) != v["sample_max"]:
            print(f"FAIL: get_sample_max({peak}) expected {v['sample_max']}, got {peaks.get_sample_max(peak)}")
            return 1
        if peaks.get_samples_over_baseline(peak) != v["samples_over_baseline"]:
            print(f"FAIL: get_samples_over_baseline({peak}) expected {v['samples_over_baseline']}, "
                  f"got {peaks.get_samples_over_baseline(peak)}")
            return 1
        if peaks.get_sample_start(peak) != v["sample_start"]:
            print(f"FAIL: get_sample_start({peak}) expected {v['sample_start']}, got {peaks.get_sample_start(peak)}")
            return 1

    print(f"PASS: PeakDescriptorData mutators for all {n_peaks} peaks")
    return 0


def test_peak_descriptor_independence() -> int:
    frame = DAPHNEEthFrame()
    n_peaks = DAPHNEEthFrame.s_max_peaks
    peaks = frame.get_peaks_data()

    for peak in range(n_peaks):
        peaks.set_adc_integral(peak * 111, peak)

    for peak in range(n_peaks):
        got = peaks.get_adc_integral(peak)
        if got != peak * 111:
            print(f"FAIL: peak independence: peak {peak} adc_integral expected {peak * 111}, got {got}")
            return 1

    print("PASS: peak descriptor fields are independent across peak indices")
    return 0


def test_peak_descriptor_out_of_range() -> int:
    frame = DAPHNEEthFrame()
    n_peaks = DAPHNEEthFrame.s_max_peaks
    peaks = frame.get_peaks_data()

    for bad_idx in (n_peaks, n_peaks + 1, -1):
        try:
            peaks.is_found(bad_idx)
            print(f"FAIL: is_found({bad_idx}) should have raised an exception")
            return 1
        except Exception:
            pass
        try:
            peaks.set_adc_integral(0, bad_idx)
            print(f"FAIL: set_adc_integral(0, {bad_idx}) should have raised an exception")
            return 1
        except Exception:
            pass

    print("PASS: PeakDescriptorData out-of-range indices raise exceptions")
    return 0


def test_peak_data_bytes_roundtrip() -> int:
    frame = DAPHNEEthFrame()
    n_peaks = DAPHNEEthFrame.s_max_peaks
    peaks = frame.get_peaks_data()

    for peak in range(n_peaks):
        peaks.set_found(True, peak)
        peaks.set_num_subpeaks(peak, peak)
        peaks.set_adc_integral(500000 + peak * 7777, peak)
        peaks.set_adc_max(2000 + peak * 111, peak)
        peaks.set_sample_max(300 + peak * 13, peak)
        peaks.set_samples_over_baseline(100 + peak * 17, peak)
        peaks.set_sample_start(400 + peak * 11, peak)

    clone = DAPHNEEthFrame(frame.get_bytes())
    clone_peaks = clone.get_peaks_data()

    for peak in range(n_peaks):
        if not clone_peaks.is_found(peak):
            print(f"FAIL: bytes roundtrip: is_found({peak}) expected True")
            return 1
        if clone_peaks.get_num_subpeaks(peak) != peak:
            print(f"FAIL: bytes roundtrip: get_num_subpeaks({peak})")
            return 1
        if clone_peaks.get_adc_integral(peak) != 500000 + peak * 7777:
            print(f"FAIL: bytes roundtrip: get_adc_integral({peak})")
            return 1
        if clone_peaks.get_adc_max(peak) != 2000 + peak * 111:
            print(f"FAIL: bytes roundtrip: get_adc_max({peak})")
            return 1
        if clone_peaks.get_sample_max(peak) != 300 + peak * 13:
            print(f"FAIL: bytes roundtrip: get_sample_max({peak})")
            return 1
        if clone_peaks.get_samples_over_baseline(peak) != 100 + peak * 17:
            print(f"FAIL: bytes roundtrip: get_samples_over_baseline({peak})")
            return 1
        if clone_peaks.get_sample_start(peak) != 400 + peak * 11:
            print(f"FAIL: bytes roundtrip: get_sample_start({peak})")
            return 1

    print("PASS: peak descriptor data survives bytes roundtrip")
    return 0


def test_set_geoid() -> int:
    frame = DAPHNEEthFrame()
    crate_id = 5
    slot_id = 3
    stream_id = 12
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
        test_peaks_data_accessible,
        test_peak_descriptor_data_mutators,
        test_peak_descriptor_independence,
        test_peak_descriptor_out_of_range,
        test_peak_data_bytes_roundtrip,
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
