/**
 * @file DAPHNEEthFrame_test.cxx DAPHNEEthFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEEthFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEEthFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <random>
#include <array>
#include <cstring>
#include <vector>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(DAPHNEEthFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_ADCDataMutators)
{
  using dunedaq::fddetdataformats::DAPHNEEthFrame;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<uint16_t> dist(0, (1 << DAPHNEEthFrame::s_bits_per_adc) - 1);

  std::vector<uint16_t> adcs(DAPHNEEthFrame::s_num_adcs);
  for (auto& adc : adcs) {
    adc = dist(rng);
  }

  DAPHNEEthFrame frame{};

  for (int i = 0; i < DAPHNEEthFrame::s_num_adcs; ++i) {
    frame.set_adc(i, adcs[i]);
  }

  for (int i = 0; i < DAPHNEEthFrame::s_num_adcs; ++i) {
    BOOST_REQUIRE_EQUAL(frame.get_adc(i), adcs[i]);
  }

  frame.set_adc(0, adcs[0]);

  for (int i = 0; i < DAPHNEEthFrame::s_num_adcs; ++i) {
    BOOST_REQUIRE_EQUAL(frame.get_adc(i), adcs[i]);
  }
}

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_IndexAndValueBounds)
{
  using dunedaq::fddetdataformats::DAPHNEEthFrame;

  DAPHNEEthFrame frame{};

  BOOST_CHECK_THROW(frame.get_adc(-1), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(DAPHNEEthFrame::s_num_adcs), std::out_of_range);

  BOOST_CHECK_THROW(frame.set_adc(-1, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(DAPHNEEthFrame::s_num_adcs, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, static_cast<uint16_t>(1 << DAPHNEEthFrame::s_bits_per_adc)), std::out_of_range);

  BOOST_CHECK_NO_THROW(frame.set_adc(0, static_cast<uint16_t>((1 << DAPHNEEthFrame::s_bits_per_adc) - 1)));
  BOOST_CHECK_EQUAL(frame.get_adc(0), static_cast<uint16_t>((1 << DAPHNEEthFrame::s_bits_per_adc) - 1));
}

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_BitPackingBoundaryIsolation)
{
  using dunedaq::fddetdataformats::DAPHNEEthFrame;

  DAPHNEEthFrame frame{};
  constexpr auto max_adc = static_cast<uint16_t>((1u << DAPHNEEthFrame::s_bits_per_adc) - 1u);

  constexpr int boundary_adc = 4;
  frame.set_adc(boundary_adc - 1, 0x0000u);
  frame.set_adc(boundary_adc, 0x0000u);
  frame.set_adc(boundary_adc + 1, 0x0000u);

  frame.set_adc(boundary_adc, 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc - 1), 0x0000u);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc + 1), 0x0000u);

  frame.set_adc(boundary_adc - 1, max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc - 1), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc + 1), 0x0000u);

  frame.set_adc(boundary_adc + 1, 0x1555u);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc - 1), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_adc + 1), 0x1555u);
}

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_MetadataMutators)
{
  using dunedaq::fddetdataformats::DAPHNEEthFrame;

  DAPHNEEthFrame frame{};

  frame.set_timestamp(0x0123456789ABCDEFuLL);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), 0x0123456789ABCDEFuLL);

  frame.set_channel(0);
  BOOST_CHECK_EQUAL(frame.get_channel(), 0);

  frame.set_channel(255);
  BOOST_CHECK_EQUAL(frame.get_channel(), 255);
}

using dunedaq::fddetdataformats::DAPHNEEthFrame;

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_layout)
{
  BOOST_CHECK_EQUAL(DAPHNEEthFrame::s_num_adcs, 256);
  BOOST_CHECK_EQUAL(DAPHNEEthFrame::s_num_adc_words, 56);
  BOOST_CHECK_EQUAL(sizeof(DAPHNEEthFrame::Header) + sizeof(uint64_t) +
                      DAPHNEEthFrame::s_num_adc_words * sizeof(uint64_t),
                    504);
  BOOST_CHECK_EQUAL(sizeof(DAPHNEEthFrame), 512);

  DAPHNEEthFrame frame{};
  frame.header.trigger_sample_value = 0x1234;
  frame.header.threshold = 0x2345;
  frame.header.baseline = 0x3456;
  frame.header.calibration_tag = 2;
  frame.header.descriptor_overflow = 1;
  frame.header.fragment_descriptor = 1;
  frame.header.continuation = 1;
  frame.header.version = 4;
  frame.header.channel = 31;

  uint64_t header_word = 0;
  std::memcpy(&header_word, &frame.header, sizeof(header_word));
  const uint64_t expected_header = 0x1234ULL | (0x2345ULL << 16) | (0x3456ULL << 32) |
                                   (2ULL << 46) | (1ULL << 49) | (1ULL << 50) |
                                   (1ULL << 51) | (4ULL << 52) | (31ULL << 56);
  BOOST_CHECK_EQUAL(header_word, expected_header);

  for (int idx = 0; idx < DAPHNEEthFrame::s_max_peaks; ++idx) {
    auto& peak = frame.header.peaks_data.peaks[idx];
    peak.reserved = 0;
    peak.adc_integral = 0x100 + idx;
    peak.found = 1;
    peak.adc_peak = 0x200 + idx;
    peak.time_peak = 0x10 + idx;
    peak.duration_minus_one = 0x20 + idx;
    peak.sample_start = idx * 40;

    const uint64_t expected = (0x100ULL + idx) | ((0x200ULL + idx) << 22) |
      ((0x20ULL + idx) << 36) | ((0x10ULL + idx) << 44) |
      (static_cast<uint64_t>(idx * 40) << 52) | (1ULL << 60);
    BOOST_CHECK_EQUAL(frame.header.get_descriptor_word(idx), expected);
    BOOST_CHECK_EQUAL(peak.get_duration(), 0x21 + idx);
  }

  const uint16_t starts[DAPHNEEthFrame::s_max_peaks] = { 5, 40, 128, 200, 255 };
  for (int idx = 0; idx < DAPHNEEthFrame::s_max_peaks; ++idx) {
    frame.header.peaks_data.set_sample_start(idx, starts[idx]);
    BOOST_CHECK_EQUAL(frame.header.peaks_data.get_sample_start(idx), starts[idx]);
  }
  BOOST_CHECK_THROW(frame.header.get_descriptor_word(5), std::out_of_range);
  BOOST_CHECK_THROW(frame.header.peaks_data.set_sample_start(0, 256), std::out_of_range);
  frame.header.set_descriptor_word(0, 0);
  BOOST_CHECK_EQUAL(frame.header.peaks_data.peaks[0].get_duration(), 0);
  frame.header.set_descriptor_word(0, (1ULL << 60) | (255ULL << 36) |
                                       (16383ULL << 22) | (256ULL * 16383));
  BOOST_CHECK_EQUAL(frame.header.peaks_data.peaks[0].get_duration(), 256);
  BOOST_CHECK_EQUAL(frame.header.peaks_data.peaks[0].adc_integral, 4194048);

  for (int idx = 0; idx < DAPHNEEthFrame::s_num_adcs; ++idx)
    frame.set_adc(idx, (idx * 73) & 0x3fff);
  for (int idx = 0; idx < DAPHNEEthFrame::s_num_adcs; ++idx)
    BOOST_CHECK_EQUAL(frame.get_adc(idx), (idx * 73) & 0x3fff);

  BOOST_CHECK_THROW(frame.get_adc(256), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_wire_offsets)
{
  std::array<uint64_t, 64> wire{};
  wire[1] = 0xfedcba9876543210ULL; // DAQ timestamp, bytes8..15
  wire[2] = (31ULL << 56) | (4ULL << 52) | (1ULL << 50);
  wire[3] = (1ULL << 60) | (7ULL << 52) | (2ULL << 44) |
            (3ULL << 36) | (100ULL << 22) | 250ULL;
  // Independent packing, one bit at a time, starting at byte64.
  for (int i = 0; i < 256; ++i) {
    const uint16_t value = (i * 73) & 0x3fff;
    for (int bit = 0; bit < 14; ++bit) {
      const int offset = i * 14 + bit;
      wire[8 + offset / 64] |= uint64_t((value >> bit) & 1) << (offset % 64);
    }
  }
  DAPHNEEthFrame frame{};
  std::memcpy(&frame, wire.data(), sizeof(frame));
  BOOST_CHECK_EQUAL(frame.get_timestamp(), wire[1]);
  BOOST_CHECK_EQUAL(frame.get_channel(), 31);
  BOOST_CHECK_EQUAL(frame.header.version, DAPHNEEthFrame::version);
  const auto& peak = frame.get_peaks_data().peaks[0];
  BOOST_CHECK_EQUAL(peak.sample_start, 7);
  BOOST_CHECK_EQUAL(peak.time_peak, 2);
  BOOST_CHECK_EQUAL(peak.get_duration(), 4);
  BOOST_CHECK_EQUAL(peak.adc_peak, 100);
  BOOST_CHECK_EQUAL(peak.adc_integral, 250);
  for (int i = 0; i < 256; ++i) BOOST_CHECK_EQUAL(frame.get_adc(i), (i * 73) & 0x3fff);
}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
