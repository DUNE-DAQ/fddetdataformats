/**
 * @file DAPHNEEthFrame_test.cxx
 */

#include "fddetdataformats/DAPHNEEthFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEEthFrame_test
#include "boost/test/unit_test.hpp"

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <vector>

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
  frame.header.trig_sample = 0x1234;
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

// Optional replay of the negative-polarity DAPHNE015 qualification capture.
BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_capture)
{
  const char* path = std::getenv("DAPHNE_CAPTURE_PCAP");
  if (!path) { BOOST_TEST_MESSAGE("Capture replay not requested"); return; }
  std::ifstream input(path, std::ios::binary);
  BOOST_REQUIRE(input.good());
  std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(input)), {});
  auto le32 = [&](size_t p) { return uint32_t(bytes.at(p)) | (uint32_t(bytes.at(p+1)) << 8) |
    (uint32_t(bytes.at(p+2)) << 16) | (uint32_t(bytes.at(p+3)) << 24); };
  auto be16 = [&](size_t p) { return (unsigned(bytes.at(p)) << 8) | bytes.at(p+1); };
  BOOST_REQUIRE_GE(bytes.size(), 24);
  BOOST_REQUIRE_EQUAL(le32(0), 0xa1b2c3d4);
  size_t records = 0, peaks = 0;
  for (size_t pos = 24; pos < bytes.size();) {
    BOOST_REQUIRE_LE(pos+16, bytes.size());
    const size_t length = le32(pos+8), start = pos+16, end = start+length;
    BOOST_REQUIRE_LE(end, bytes.size());
    pos = end;
    if (length < 42 || be16(start+12) != 0x0800 || bytes[start+23] != 17) continue;
    const std::array<uint8_t,6> mac{0xde,0xad,0xbe,0xef,0xca,0xfe};
    if (!std::equal(mac.begin(), mac.end(), bytes.begin()+start+6)) continue;
    const size_t udp = start+14+4*(bytes[start+14]&15);
    BOOST_REQUIRE_LE(udp+8, end);
    const size_t size = be16(udp+4);
    BOOST_REQUIRE_GE(size, 8);
    BOOST_REQUIRE_LE(udp+size, end);
    BOOST_REQUIRE_EQUAL((size-8)%sizeof(DAPHNEEthFrame), 0);
    for (size_t p = udp+8; p < udp+size; p += sizeof(DAPHNEEthFrame)) {
      DAPHNEEthFrame frame{};
      std::memcpy(&frame, bytes.data()+p, sizeof(frame));
      BOOST_REQUIRE_EQUAL(frame.header.version, 4);
      for (const auto& peak : frame.get_peaks_data().peaks) {
        if (!peak.found) continue;
        const unsigned duration = peak.get_duration(), first = peak.sample_start;
        BOOST_REQUIRE_LE(first+duration, 256);
        BOOST_REQUIRE_LT(peak.time_peak, duration);
        unsigned integral = 0, height = 0, maximum = 0;
        for (unsigned i = 0; i < duration; ++i) {
          const unsigned amp = std::max(0, int(frame.header.baseline)-int(frame.get_adc(first+i)));
          integral += amp;
          if (amp > height) { height = amp; maximum = i; }
        }
        BOOST_REQUIRE_EQUAL(peak.adc_integral, integral);
        BOOST_REQUIRE_EQUAL(peak.adc_peak, height);
        BOOST_REQUIRE_EQUAL(peak.time_peak, maximum);
        ++peaks;
      }
      ++records;
    }
  }
  BOOST_REQUIRE_GT(records, 0);
  BOOST_REQUIRE_GT(peaks, 0);
  BOOST_TEST_MESSAGE("Capture verified: " << records << " records, " << peaks << " peaks");
}
