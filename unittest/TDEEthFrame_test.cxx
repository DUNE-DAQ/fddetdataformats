/**
 * @file TDEEthFrame_test.cxx TDEEthFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/TDEEthFrame.hpp"

#define BOOST_TEST_MODULE TDEEthFrame_test

#include "boost/test/unit_test.hpp"

#include <random>
#include <vector>

BOOST_AUTO_TEST_SUITE(TDEEthFrame_test)

BOOST_AUTO_TEST_CASE(TDEEthFrame_ADCDataMutators)
{
  using dunedaq::fddetdataformats::TDEEthFrame;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<uint16_t> dist(1, (1 << TDEEthFrame::s_bits_per_adc) - 1);

  std::vector<std::vector<uint16_t>> adcs(TDEEthFrame::s_num_channels,
                                          std::vector<uint16_t>(TDEEthFrame::s_time_samples_per_frame));

  for (int channel = 0; channel < TDEEthFrame::s_num_channels; ++channel) {
    for (int sample = 0; sample < TDEEthFrame::s_time_samples_per_frame; ++sample) {
      adcs[channel][sample] = dist(rng);
    }
  }

  TDEEthFrame frame{};

  for (int channel = 0; channel < TDEEthFrame::s_num_channels; ++channel) {
    for (int sample = 0; sample < TDEEthFrame::s_time_samples_per_frame; ++sample) {
      frame.set_adc(channel, sample, adcs[channel][sample]);
    }
  }

  for (int channel = 0; channel < TDEEthFrame::s_num_channels; ++channel) {
    for (int sample = 0; sample < TDEEthFrame::s_time_samples_per_frame; ++sample) {
      BOOST_REQUIRE_EQUAL(frame.get_adc(channel, sample), adcs[channel][sample]);
    }
  }

  frame.set_adc(0, 0, adcs[0][0]);

  for (int channel = 0; channel < TDEEthFrame::s_num_channels; ++channel) {
    for (int sample = 0; sample < TDEEthFrame::s_time_samples_per_frame; ++sample) {
      BOOST_REQUIRE_EQUAL(frame.get_adc(channel, sample), adcs[channel][sample]);
    }
  }
}

BOOST_AUTO_TEST_CASE(TDEEthFrame_IndexAndValueBounds)
{
  using dunedaq::fddetdataformats::TDEEthFrame;

  TDEEthFrame frame{};

  BOOST_CHECK_THROW(frame.get_adc(-1, 0), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(TDEEthFrame::s_num_channels, 0), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(0, -1), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(0, TDEEthFrame::s_time_samples_per_frame), std::out_of_range);

  BOOST_CHECK_THROW(frame.set_adc(-1, 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(TDEEthFrame::s_num_channels, 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, -1, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, TDEEthFrame::s_time_samples_per_frame, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, 0, static_cast<uint16_t>(1 << TDEEthFrame::s_bits_per_adc)), std::out_of_range);

  BOOST_CHECK_NO_THROW(frame.set_adc(0, 0, static_cast<uint16_t>((1 << TDEEthFrame::s_bits_per_adc) - 1)));
  BOOST_CHECK_EQUAL(frame.get_adc(0, 0), static_cast<uint16_t>((1 << TDEEthFrame::s_bits_per_adc) - 1));
}

BOOST_AUTO_TEST_CASE(TDEEthFrame_BitPackingBoundaryIsolation)
{
  using dunedaq::fddetdataformats::TDEEthFrame;

  TDEEthFrame frame{};
  constexpr uint16_t max_adc = static_cast<uint16_t>((1u << TDEEthFrame::s_bits_per_adc) - 1u);
  constexpr int sample = 5;
  constexpr int boundary_channel = 4;

  frame.set_adc(boundary_channel - 1, sample, 0x0000u);
  frame.set_adc(boundary_channel, sample, 0x0000u);
  frame.set_adc(boundary_channel + 1, sample, 0x0000u);

  frame.set_adc(boundary_channel, sample, 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel - 1, sample), 0x0000u);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel, sample), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel + 1, sample), 0x0000u);

  frame.set_adc(boundary_channel - 1, sample, max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel - 1, sample), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel, sample), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel + 1, sample), 0x0000u);

  frame.set_adc(boundary_channel + 1, sample, 0x1555u);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel - 1, sample), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel, sample), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_channel + 1, sample), 0x1555u);
}

BOOST_AUTO_TEST_CASE(TDEEthFrame_MetadataMutators)
{
  using dunedaq::fddetdataformats::TDEEthFrame;

  TDEEthFrame frame{};

  frame.set_timestamp(0x0123456789ABCDEFuLL);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), 0x0123456789ABCDEFuLL);

  frame.set_channel(0);
  BOOST_CHECK_EQUAL(frame.get_channel(), 0);

  frame.set_channel(255);
  BOOST_CHECK_EQUAL(frame.get_channel(), 255);
}

BOOST_AUTO_TEST_SUITE_END()
