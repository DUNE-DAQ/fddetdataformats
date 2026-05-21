/**
 * @file CRTGrenobleFrame_test.cxx CRTGrenobleFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/CRTGrenobleFrame.hpp"

#define BOOST_TEST_MODULE CRTGrenobleFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <random>
#include <vector>

BOOST_AUTO_TEST_SUITE(CRTGrenobleFrame_test)

BOOST_AUTO_TEST_CASE(CRTGrenobleFrame_ADCDataMutators)
{
  using dunedaq::fddetdataformats::CRTGrenobleFrame;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<int> dist(-100000, 100000);

  std::vector<int> adcs(CRTGrenobleFrame::s_num_channels);
  for (auto& value : adcs) {
    value = dist(rng);
  }

  CRTGrenobleFrame frame{};

  for (int channel = 0; channel < CRTGrenobleFrame::s_num_channels; ++channel) {
    frame.set_adc(channel, adcs[channel]);
  }

  for (int channel = 0; channel < CRTGrenobleFrame::s_num_channels; ++channel) {
    BOOST_REQUIRE_EQUAL(frame.get_adc(channel), adcs[channel]);
  }

  frame.set_adc(0, adcs[0]);
  for (int channel = 0; channel < CRTGrenobleFrame::s_num_channels; ++channel) {
    BOOST_REQUIRE_EQUAL(frame.get_adc(channel), adcs[channel]);
  }
}

BOOST_AUTO_TEST_CASE(CRTGrenobleFrame_IndexBounds)
{
  using dunedaq::fddetdataformats::CRTGrenobleFrame;

  CRTGrenobleFrame frame{};

  BOOST_CHECK_THROW(frame.get_adc(-1), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(CRTGrenobleFrame::s_num_channels), std::out_of_range);

  BOOST_CHECK_THROW(frame.set_adc(-1, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(CRTGrenobleFrame::s_num_channels, 123), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(CRTGrenobleFrame_TimestampMutators)
{
  using dunedaq::fddetdataformats::CRTGrenobleFrame;

  CRTGrenobleFrame frame{};

  frame.set_timestamp(0x0123456789ABCDEFuLL);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), 0x0123456789ABCDEFuLL);
}

BOOST_AUTO_TEST_CASE(CRTGrenobleFrame_Constants)
{
  using dunedaq::fddetdataformats::CRTGrenobleFrame;

  BOOST_CHECK_EQUAL(CRTGrenobleFrame::s_DTS_ticks_per_second, 62500000u);
  BOOST_CHECK_EQUAL(CRTGrenobleFrame::s_ns_per_DTS_tick, 16u);
}

BOOST_AUTO_TEST_SUITE_END()
