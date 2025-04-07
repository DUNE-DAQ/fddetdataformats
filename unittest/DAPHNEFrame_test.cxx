/**
 * @file DAPHNEFrame_test.cxx - Comprehensive unit tests for DAPHNEFrame
 */

#include "fddetdataformats/DAPHNEFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEFrame_test

#include "boost/test/unit_test.hpp"

#include <vector>
#include <random>
#include <algorithm>

BOOST_AUTO_TEST_SUITE(DAPHNEFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEFrame_AllFieldsTest)
{
  constexpr int n_adcs = 320;
  constexpr int n_tps = 5;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint16_t> adc_dist(1, (1 << 14) - 1);
  std::uniform_int_distribution<uint16_t> u10bit(0, 0x3FF);
  std::uniform_int_distribution<uint16_t> u9bit(0, 0x1FF);
  std::uniform_int_distribution<uint16_t> u4bit(0, 0xF);
  std::uniform_int_distribution<uint32_t> u23bit(0, 0x7FFFFF);
  std::uniform_int_distribution<uint16_t> u14bit(0, 0x3FFF);
  std::uniform_int_distribution<uint8_t> u1bit(0, 1);

  dunedaq::fddetdataformats::DAPHNEFrame frame{};

  // ─── Test ADC Values ──────────────────────────────────────────────
  std::vector<uint16_t> adcs(n_adcs);
  std::generate(adcs.begin(), adcs.end(), [&]() { return adc_dist(gen); });

  for (int i = 0; i < n_adcs; ++i)
    frame.set_adc(i, adcs[i]);

  for (int i = 0; i < n_adcs; ++i)
    BOOST_CHECK_EQUAL(frame.get_adc(i), adcs[i]);

  // ─── Test Header ──────────────────────────────────────────────────
  frame.set_channel(17);
  frame.header.algorithm_id = 9;
  frame.header.trigger_sample_value = 0xFACE;
  frame.header.threshold = 0xBEEF;
  frame.header.baseline = 0xABCD;

  BOOST_CHECK_EQUAL(frame.get_channel(), 17);
  BOOST_CHECK_EQUAL(frame.header.algorithm_id, 9);
  BOOST_CHECK_EQUAL(frame.header.trigger_sample_value, 0xFACE);
  BOOST_CHECK_EQUAL(frame.header.threshold, 0xBEEF);
  BOOST_CHECK_EQUAL(frame.header.get_baseline(), 0xABCD);

  // ─── Test Trailer ─────────────────────────────────────────────────
  for (int tp = 0; tp < n_tps; ++tp) {
    uint8_t num_peak = u4bit(gen);
    uint8_t da = u1bit(gen);
    uint32_t adc_integral = u23bit(gen);
    uint16_t adc_peak = u14bit(gen);
    uint16_t time_peak = u9bit(gen);
    uint16_t tob = u9bit(gen);
    uint16_t tstart = u10bit(gen);

    frame.set_num_peak(num_peak, tp);
    frame.set_da(da, tp);
    frame.set_adc_integral(adc_integral, tp);
    frame.set_adc_peak(adc_peak, tp);
    frame.set_time_peak(time_peak, tp);
    frame.set_time_over_baseline(tob, tp);
    frame.set_time_start(tstart, tp);

    BOOST_CHECK_EQUAL(frame.get_num_peak(tp), num_peak);
    BOOST_CHECK_EQUAL(frame.get_da(tp), da);
    BOOST_CHECK_EQUAL(frame.get_adc_integral(tp), adc_integral);
    BOOST_CHECK_EQUAL(frame.get_adc_peak(tp), adc_peak);
    BOOST_CHECK_EQUAL(frame.get_time_peak(tp), time_peak);
    BOOST_CHECK_EQUAL(frame.get_time_over_baseline(tp), tob);
    BOOST_CHECK_EQUAL(frame.get_time_start(tp), tstart);
  }
}

BOOST_AUTO_TEST_SUITE_END()