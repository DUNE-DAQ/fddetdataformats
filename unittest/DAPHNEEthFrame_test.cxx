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

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
