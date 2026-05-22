/**
 * @file DAPHNEStreamFrame_test.cxx DAPHNEStreamFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEStreamFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEStreamFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <cstdint>
#include <random>
#include <vector>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(DAPHNEStreamFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEStreamFrame_ADCDataMutators)
{
  using dunedaq::fddetdataformats::DAPHNEStreamFrame;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<uint16_t> dist(0, (1 << DAPHNEStreamFrame::s_bits_per_adc) - 1);

  std::vector<std::vector<uint16_t>> adcs(DAPHNEStreamFrame::s_adcs_per_channel,
                                          std::vector<uint16_t>(DAPHNEStreamFrame::s_channels_per_frame));

  for (uint32_t sample = 0; sample < DAPHNEStreamFrame::s_adcs_per_channel; ++sample) {
    for (uint32_t channel = 0; channel < DAPHNEStreamFrame::s_channels_per_frame; ++channel) {
      adcs[sample][channel] = dist(rng);
    }
  }

  DAPHNEStreamFrame frame{};

  for (uint32_t sample = 0; sample < DAPHNEStreamFrame::s_adcs_per_channel; ++sample) {
    for (uint32_t channel = 0; channel < DAPHNEStreamFrame::s_channels_per_frame; ++channel) {
      frame.set_adc(sample, channel, adcs[sample][channel]);
    }
  }

  for (uint32_t sample = 0; sample < DAPHNEStreamFrame::s_adcs_per_channel; ++sample) {
    for (uint32_t channel = 0; channel < DAPHNEStreamFrame::s_channels_per_frame; ++channel) {
      BOOST_REQUIRE_EQUAL(frame.get_adc(sample, channel), adcs[sample][channel]);
    }
  }

  frame.set_adc(0, 0, adcs[0][0]);

  for (uint32_t sample = 0; sample < DAPHNEStreamFrame::s_adcs_per_channel; ++sample) {
    for (uint32_t channel = 0; channel < DAPHNEStreamFrame::s_channels_per_frame; ++channel) {
      BOOST_REQUIRE_EQUAL(frame.get_adc(sample, channel), adcs[sample][channel]);
    }
  }
}

BOOST_AUTO_TEST_CASE(DAPHNEStreamFrame_IndexAndValueBounds)
{
  using dunedaq::fddetdataformats::DAPHNEStreamFrame;

  DAPHNEStreamFrame frame{};

  BOOST_CHECK_THROW(frame.get_adc(DAPHNEStreamFrame::s_adcs_per_channel, 0), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(0, DAPHNEStreamFrame::s_channels_per_frame), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(static_cast<uint32_t>(-1), 0), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(0, static_cast<uint32_t>(-1)), std::out_of_range);

  BOOST_CHECK_THROW(frame.set_adc(DAPHNEStreamFrame::s_adcs_per_channel, 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, DAPHNEStreamFrame::s_channels_per_frame, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(static_cast<uint32_t>(-1), 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, static_cast<uint32_t>(-1), 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, 0, static_cast<uint16_t>(1 << DAPHNEStreamFrame::s_bits_per_adc)),
                    std::out_of_range);

  BOOST_CHECK_NO_THROW(frame.set_adc(0, 0, static_cast<uint16_t>((1 << DAPHNEStreamFrame::s_bits_per_adc) - 1)));
  BOOST_CHECK_EQUAL(frame.get_adc(0, 0), static_cast<uint16_t>((1 << DAPHNEStreamFrame::s_bits_per_adc) - 1));
}

BOOST_AUTO_TEST_CASE(DAPHNEStreamFrame_BitPackingBoundaryIsolation)
{
  using dunedaq::fddetdataformats::DAPHNEStreamFrame;

  DAPHNEStreamFrame frame{};
  constexpr auto max_adc = static_cast<uint16_t>((1u << DAPHNEStreamFrame::s_bits_per_adc) - 1u);

  constexpr uint32_t boundary_sample = 1;
  constexpr uint32_t boundary_channel = 0;

  frame.set_adc(0, 3, 0x0000u);
  frame.set_adc(boundary_sample, boundary_channel, 0x0000u);
  frame.set_adc(boundary_sample, 1, 0x0000u);

  frame.set_adc(boundary_sample, boundary_channel, 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(0, 3), 0x0000u);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, boundary_channel), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, 1), 0x0000u);

  frame.set_adc(0, 3, max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(0, 3), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, boundary_channel), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, 1), 0x0000u);

  frame.set_adc(boundary_sample, 1, 0x1555u);
  BOOST_CHECK_EQUAL(frame.get_adc(0, 3), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, boundary_channel), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, 1), 0x1555u);
}

BOOST_AUTO_TEST_CASE(DAPHNEStreamFrame_MetadataMutators)
{
  using dunedaq::fddetdataformats::DAPHNEStreamFrame;

  DAPHNEStreamFrame frame{};

  frame.set_timestamp(0x0123456789ABCDEFuLL);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), 0x0123456789ABCDEFuLL);

  frame.header.channel_0 = 10;
  frame.header.channel_1 = 20;
  frame.header.channel_2 = 30;
  frame.header.channel_3 = 40;

  BOOST_CHECK_EQUAL(frame.get_channel0(), 10);
  BOOST_CHECK_EQUAL(frame.get_channel1(), 20);
  BOOST_CHECK_EQUAL(frame.get_channel2(), 30);
  BOOST_CHECK_EQUAL(frame.get_channel3(), 40);
}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
