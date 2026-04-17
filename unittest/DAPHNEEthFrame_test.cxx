/**
 * @file DAPHNEEthFrame_test.cxx - Unit tests for DAPHNEEthFrame
 */

#include "fddetdataformats/DAPHNEEthFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEEthFrame_test

#include "boost/test/unit_test.hpp"

#include <algorithm>
#include <random>
#include <vector>

BOOST_AUTO_TEST_SUITE(DAPHNEEthFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_AllFieldsTest)
{
  constexpr int n_adcs = dunedaq::fddetdataformats::DAPHNEEthFrame::s_num_adcs;
  constexpr int n_peaks = dunedaq::fddetdataformats::DAPHNEEthFrame::PeakDescriptorData::max_peaks;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint16_t> adc_dist(1, (1 << 14) - 1);
  std::uniform_int_distribution<uint16_t> u10bit(0, 0x3FF);
  std::uniform_int_distribution<uint16_t> u9bit(0, 0x1FF);
  std::uniform_int_distribution<uint16_t> u4bit(0, 0xF);
  std::uniform_int_distribution<uint32_t> u23bit(0, 0x7FFFFF);
  std::uniform_int_distribution<uint16_t> u14bit(0, 0x3FFF);
  std::uniform_int_distribution<uint8_t> u1bit(0, 1);

  dunedaq::fddetdataformats::DAPHNEEthFrame frame{};

  BOOST_CHECK_EQUAL(sizeof(dunedaq::fddetdataformats::DAPHNEEthFrame::PeakDescriptorData), 12 * sizeof(uint32_t));
  BOOST_CHECK_EQUAL(sizeof(dunedaq::fddetdataformats::DAPHNEEthFrame::Header), 7 * sizeof(uint64_t));

  std::vector<uint16_t> adcs(n_adcs);
  std::generate(adcs.begin(), adcs.end(), [&]() { return adc_dist(gen); });

  for (int i = 0; i < n_adcs; ++i) {
    frame.set_adc(i, adcs[i]);
  }

  for (int i = 0; i < n_adcs; ++i) {
    BOOST_CHECK_EQUAL(frame.get_adc(i), adcs[i]);
  }

  frame.set_channel(23);
  frame.header.trig_sample = 0x2AAA;
  frame.header.threshold = 0x1555;
  frame.header.baseline = 0x0123;
  frame.header.version = dunedaq::fddetdataformats::DAPHNEEthFrame::version;
  frame.set_timestamp(0x123456789ABCDEF0ULL);

  BOOST_CHECK_EQUAL(frame.get_channel(), 23);
  BOOST_CHECK_EQUAL(frame.header.trig_sample, 0x2AAA);
  BOOST_CHECK_EQUAL(frame.header.threshold, 0x1555);
  BOOST_CHECK_EQUAL(frame.header.get_baseline(), 0x0123);
  BOOST_CHECK_EQUAL(frame.header.version, dunedaq::fddetdataformats::DAPHNEEthFrame::version);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), 0x123456789ABCDEF0ULL);

  for (int peak = 0; peak < n_peaks; ++peak) {
    uint8_t num_subpeaks = u4bit(gen);
    uint8_t found = u1bit(gen);
    uint32_t adc_integral = u23bit(gen);
    uint16_t adc_max = u14bit(gen);
    uint16_t sample_peak = u9bit(gen);
    uint16_t tob = u9bit(gen);
    uint16_t sample_start = u10bit(gen);

    frame.get_peaks_data().set_num_subpeaks(num_subpeaks, peak);
    frame.get_peaks_data().set_found(found, peak);
    frame.get_peaks_data().set_adc_integral(adc_integral, peak);
    frame.get_peaks_data().set_adc_max(adc_max, peak);
    frame.get_peaks_data().set_sample_max(sample_peak, peak);
    frame.get_peaks_data().set_samples_over_baseline(tob, peak);
    frame.get_peaks_data().set_sample_start(sample_start, peak);

    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_num_subpeaks(peak), num_subpeaks);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().is_found(peak), found);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_adc_integral(peak), adc_integral);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_adc_max(peak), adc_max);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_sample_max(peak), sample_peak);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_samples_over_baseline(peak), tob);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_sample_start(peak), sample_start);
  }

  for (int packed_word = 0; packed_word < dunedaq::fddetdataformats::DAPHNEEthFrame::s_packed_peak_descriptor_words; ++packed_word) {
    auto value = frame.header.get_packed_peak_word(packed_word);
    dunedaq::fddetdataformats::DAPHNEEthFrame::word_t roundtrip = 0;
    roundtrip = value;
    BOOST_CHECK_EQUAL(roundtrip, value);
  }
}

BOOST_AUTO_TEST_SUITE_END()
