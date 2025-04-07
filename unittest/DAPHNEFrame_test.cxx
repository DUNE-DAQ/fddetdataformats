/**
 * @file DAPHNEFrame_test.cxx DAPHNEFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEFrame_test
#include "boost/test/unit_test.hpp"

#include <random>

BOOST_AUTO_TEST_SUITE(DAPHNEFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEFrame_CompleteDataTest)
{
  using dunedaq::fddetdataformats::DAPHNEFrame;

  DAPHNEFrame frame {};

  // Random number generators
  std::random_device dev;
  std::mt19937 rng(dev());

  std::uniform_int_distribution<uint32_t> dist_integral(0, 0x7FFFFF);
  std::uniform_int_distribution<uint16_t> dist_adc_peak(0, 0x3FFF);
  std::uniform_int_distribution<uint16_t> dist_time(0, 0x1FF);
  std::uniform_int_distribution<uint16_t> dist_time_start(0, 0x3FF);
  std::uniform_int_distribution<uint8_t> dist_da(0, 1);
  std::uniform_int_distribution<uint8_t> dist_num_peak(0, 0xF);
  std::uniform_int_distribution<uint16_t> dist_adc_val(0, 0x3FFF);
  std::uniform_int_distribution<uint8_t> dist_channel(0, 63);
  std::uniform_int_distribution<uint16_t> dist_trigger_sample_value(0, 0xFFFF);
  std::uniform_int_distribution<uint16_t> dist_threshold(0, 0xFFFF);
  std::uniform_int_distribution<uint16_t> dist_baseline(0, 0xFFFF);

  // Fill header
  frame.set_channel(dist_channel(rng));
  frame.header.trigger_sample_value = dist_trigger_sample_value(rng);
  frame.header.threshold = dist_threshold(rng);
  frame.header.baseline = dist_baseline(rng);

  BOOST_REQUIRE_EQUAL(frame.get_channel(), frame.header.channel);
  BOOST_REQUIRE_EQUAL(frame.header.get_baseline(), frame.header.baseline);

  // Fill and check ADC values
  for(int i = 0; i < 320; ++i) {
    uint16_t val = dist_adc_val(rng);
    frame.set_adc(i, val);
    BOOST_REQUIRE_EQUAL(frame.get_adc(i), val);
  }

  // Set random values for each TP
  for(int tp = 0; tp < 5; ++tp) {
    uint8_t da = dist_da(rng);
    uint32_t adc_integral = dist_integral(rng);
    uint8_t num_peak = dist_num_peak(rng);
    uint16_t adc_peak = dist_adc_peak(rng);
    uint16_t time_peak = dist_time(rng);
    uint16_t time_over_baseline = dist_time(rng);

    frame.set_da(da, tp);
    frame.set_adc_integral(adc_integral, tp);
    frame.set_num_peak(num_peak, tp);
    frame.set_adc_peak(adc_peak, tp);
    frame.set_time_peak(time_peak, tp);
    frame.set_time_over_baseline(time_over_baseline, tp);

    BOOST_REQUIRE_EQUAL(frame.get_da(tp), da);
    BOOST_REQUIRE_EQUAL(frame.get_adc_integral(tp), adc_integral);
    BOOST_REQUIRE_EQUAL(frame.get_num_peak(tp), num_peak);
    BOOST_REQUIRE_EQUAL(frame.get_adc_peak(tp), adc_peak);
    BOOST_REQUIRE_EQUAL(frame.get_time_peak(tp), time_peak);
    BOOST_REQUIRE_EQUAL(frame.get_time_over_baseline(tp), time_over_baseline);
  }

  // Set and check Time_Start fields
  uint16_t ts[5];
  for(int idx = 0; idx < 5; ++idx) {
    ts[idx] = dist_time_start(rng);
  }

  frame.set_time_start_0(ts[0]);
  frame.set_time_start_1(ts[1]);
  frame.set_time_start_2(ts[2]);
  frame.set_time_start_3(ts[3]);
  frame.set_time_start_4(ts[4]);

  BOOST_REQUIRE_EQUAL(frame.get_time_start_0(), ts[0]);
  BOOST_REQUIRE_EQUAL(frame.get_time_start_1(), ts[1]);
  BOOST_REQUIRE_EQUAL(frame.get_time_start_2(), ts[2]);
  BOOST_REQUIRE_EQUAL(frame.get_time_start_3(), ts[3]);
  BOOST_REQUIRE_EQUAL(frame.get_time_start_4(), ts[4]);

  // Trailer word
  frame.trailer.trailer = 0xFFFFFFFF;
  BOOST_REQUIRE_EQUAL(frame.trailer.trailer, 0xFFFFFFFF);
}

BOOST_AUTO_TEST_SUITE_END()
