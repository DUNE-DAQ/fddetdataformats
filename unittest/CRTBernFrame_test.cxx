/**
 * @file CRTBernFrame_test.cxx CRTBernFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/CRTBernFrame.hpp"

#define BOOST_TEST_MODULE CRTBernFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <random>
#include <vector>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(CRTBernFrame_test)

BOOST_AUTO_TEST_CASE(CRTBernFrame_ADCDataMutators)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<uint16_t> dist(0, 65535);

  std::vector<uint16_t> adcs(CRTBernFrame::s_num_channels);
  for (auto& value : adcs) {
    value = dist(rng);
  }

  CRTBernFrame frame{};

  for (int channel = 0; channel < CRTBernFrame::s_num_channels; ++channel) {
    frame.set_adc(channel, adcs[channel]);
  }

  for (int channel = 0; channel < CRTBernFrame::s_num_channels; ++channel) {
    BOOST_REQUIRE_EQUAL(frame.get_adc(channel), adcs[channel]);
  }

  frame.set_adc(0, adcs[0]);
  for (int channel = 0; channel < CRTBernFrame::s_num_channels; ++channel) {
    BOOST_REQUIRE_EQUAL(frame.get_adc(channel), adcs[channel]);
  }
}

BOOST_AUTO_TEST_CASE(CRTBernFrame_IndexBounds)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  CRTBernFrame frame{};

  BOOST_CHECK_THROW(frame.get_adc(-1), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(CRTBernFrame::s_num_channels), std::out_of_range);

  BOOST_CHECK_THROW(frame.set_adc(-1, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(CRTBernFrame::s_num_channels, 123), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(CRTBernFrame_TimestampMutators)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  CRTBernFrame frame{};

  uint64_t timestamp = 0x0123456789ABCDEFuLL;
  frame.set_timestamp(timestamp);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), timestamp);

  // Verify ts0 is also set consistently with timestamp
  uint32_t expected_ts0 = (timestamp % CRTBernFrame::s_DTS_ticks_per_second) * CRTBernFrame::s_ns_per_DTS_tick;
  BOOST_CHECK_EQUAL(frame.get_ts0(), expected_ts0);
}

BOOST_AUTO_TEST_CASE(CRTBernFrame_MAC5Mutators)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  CRTBernFrame frame{};

  uint16_t mac5_value = 0xABCD;
  frame.set_mac5(mac5_value);
  BOOST_CHECK_EQUAL(frame.get_mac5(), mac5_value);

  // Test with different values
  mac5_value = 0x0000;
  frame.set_mac5(mac5_value);
  BOOST_CHECK_EQUAL(frame.get_mac5(), mac5_value);

  mac5_value = 0xFFFF;
  frame.set_mac5(mac5_value);
  BOOST_CHECK_EQUAL(frame.get_mac5(), mac5_value);
}

BOOST_AUTO_TEST_CASE(CRTBernFrame_FlagsAndCounters)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  CRTBernFrame frame{};

  uint16_t flags = 0x00FF;
  frame.set_flags(flags);
  BOOST_CHECK_EQUAL(frame.get_flags(), flags);

  uint16_t lostcpu = 0x1234;
  frame.set_lostcpu(lostcpu);
  BOOST_CHECK_EQUAL(frame.get_lostcpu(), lostcpu);

  uint16_t lostfpga = 0x5678;
  frame.set_lostfpga(lostfpga);
  BOOST_CHECK_EQUAL(frame.get_lostfpga(), lostfpga);

  // Verify all values are independent
  BOOST_CHECK_EQUAL(frame.get_flags(), flags);
  BOOST_CHECK_EQUAL(frame.get_lostcpu(), lostcpu);
  BOOST_CHECK_EQUAL(frame.get_lostfpga(), lostfpga);
}

BOOST_AUTO_TEST_CASE(CRTBernFrame_TimestampFields)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  CRTBernFrame frame{};

  uint32_t ts0 = 0x12345678;
  frame.set_ts0(ts0);
  BOOST_CHECK_EQUAL(frame.get_ts0(), ts0);

  uint32_t ts1 = 0x9ABCDEF0;
  frame.set_ts1(ts1);
  BOOST_CHECK_EQUAL(frame.get_ts1(), ts1);

  uint32_t coinc = 0xDEADBEEF;
  frame.set_coinc(coinc);
  BOOST_CHECK_EQUAL(frame.get_coinc(), coinc);

  // Verify all values are independent
  BOOST_CHECK_EQUAL(frame.get_ts0(), ts0);
  BOOST_CHECK_EQUAL(frame.get_ts1(), ts1);
  BOOST_CHECK_EQUAL(frame.get_coinc(), coinc);
}

BOOST_AUTO_TEST_CASE(CRTBernFrame_Constants)
{
  using dunedaq::fddetdataformats::CRTBernFrame;

  BOOST_CHECK_EQUAL(CRTBernFrame::s_DTS_ticks_per_second, 62500000u);
  BOOST_CHECK_EQUAL(CRTBernFrame::s_ns_per_DTS_tick, 16u);
  BOOST_CHECK_EQUAL(CRTBernFrame::s_num_channels, 32);
}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
