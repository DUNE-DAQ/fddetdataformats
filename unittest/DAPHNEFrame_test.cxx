/**
 * @file DAPHNEFrame_test.cxx - Comprehensive unit tests for DAPHNEFrame
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 */

#include "fddetdataformats/DAPHNEFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <algorithm>
#include <random>
#include <vector>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(DAPHNEFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEFrame_AllFieldsTest)
{
  constexpr int n_adcs = 320;
  constexpr int n_peaks = 5;

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

  frame.set_adc(0, adcs[0]);

  for (int i = 0; i < n_adcs; ++i)
    BOOST_CHECK_EQUAL(frame.get_adc(i), adcs[i]);


}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
