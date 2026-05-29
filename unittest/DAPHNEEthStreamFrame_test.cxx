/**
 * @file DAPHNEEthStreamFrame_test.cxx DAPHNEEthStreamFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEEthStreamFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEEthStreamFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <cstdint>
#include <random>
#include <vector>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(DAPHNEEthStreamFrame_test)

BOOST_AUTO_TEST_CASE(DAPHNEEthStreamFrame_ADCDataMutators)
{
  using dunedaq::fddetdataformats::DAPHNEEthStreamFrame;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<uint16_t> dist(0, (1 << DAPHNEEthStreamFrame::s_bits_per_adc) - 1);

  std::vector<std::vector<uint16_t>> adcs(DAPHNEEthStreamFrame::s_adcs_per_channel,
                                          std::vector<uint16_t>(DAPHNEEthStreamFrame::s_num_channels));

  for (uint32_t channel = 0; channel < DAPHNEEthStreamFrame::s_num_channels; ++channel) {
    for (uint32_t adc_index = 0; adc_index < DAPHNEEthStreamFrame::s_adcs_per_channel; ++adc_index) {
      adcs[adc_index][channel] = dist(rng);
    }
  }

  DAPHNEEthStreamFrame frame{};

  for (uint32_t channel = 0; channel < DAPHNEEthStreamFrame::s_num_channels; ++channel) {
    for (uint32_t adc_index = 0; adc_index < DAPHNEEthStreamFrame::s_adcs_per_channel; ++adc_index) {
      frame.set_adc(channel, adc_index, adcs[adc_index][channel]);
    }
  }

  for (uint32_t channel = 0; channel < DAPHNEEthStreamFrame::s_num_channels; ++channel) {
    for (uint32_t adc_index = 0; adc_index < DAPHNEEthStreamFrame::s_adcs_per_channel; ++adc_index) {
      BOOST_REQUIRE_EQUAL(frame.get_adc(adc_index, channel), adcs[adc_index][channel]);
    }
  }

  frame.set_adc(0, 0, adcs[0][0]);

  for (uint32_t channel = 0; channel < DAPHNEEthStreamFrame::s_num_channels; ++channel) {
    for (uint32_t adc_index = 0; adc_index < DAPHNEEthStreamFrame::s_adcs_per_channel; ++adc_index) {
      BOOST_REQUIRE_EQUAL(frame.get_adc(adc_index, channel), adcs[adc_index][channel]);
    }
  }
}

BOOST_AUTO_TEST_CASE(DAPHNEEthStreamFrame_IndexAndValueBounds)
{
  using dunedaq::fddetdataformats::DAPHNEEthStreamFrame;

  DAPHNEEthStreamFrame frame{};

  BOOST_CHECK_THROW(frame.get_adc(DAPHNEEthStreamFrame::s_adcs_per_channel, 0), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(0, DAPHNEEthStreamFrame::s_num_channels), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(static_cast<uint32_t>(-1), 0), std::out_of_range);
  BOOST_CHECK_THROW(frame.get_adc(0, static_cast<uint32_t>(-1)), std::out_of_range);

  BOOST_CHECK_THROW(frame.set_adc(DAPHNEEthStreamFrame::s_num_channels, 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, DAPHNEEthStreamFrame::s_adcs_per_channel, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(static_cast<uint32_t>(-1), 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, static_cast<uint32_t>(-1), 123), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_adc(0, 0, static_cast<uint16_t>(1 << DAPHNEEthStreamFrame::s_bits_per_adc)),
                    std::out_of_range);

  BOOST_CHECK_NO_THROW(frame.set_adc(0, 0, static_cast<uint16_t>((1 << DAPHNEEthStreamFrame::s_bits_per_adc) - 1)));
  BOOST_CHECK_EQUAL(frame.get_adc(0, 0), static_cast<uint16_t>((1 << DAPHNEEthStreamFrame::s_bits_per_adc) - 1));
}

BOOST_AUTO_TEST_CASE(DAPHNEEthStreamFrame_BitPackingBoundaryIsolation)
{
  using dunedaq::fddetdataformats::DAPHNEEthStreamFrame;

  DAPHNEEthStreamFrame frame{};
  constexpr auto max_adc = static_cast<uint16_t>((1u << DAPHNEEthStreamFrame::s_bits_per_adc) - 1u);

  constexpr uint32_t boundary_sample = 1;
  constexpr uint32_t boundary_channel = 0;

  frame.set_adc(3, 0, 0x0000u);
  frame.set_adc(boundary_channel, boundary_sample, 0x0000u);
  frame.set_adc(1, boundary_sample, 0x0000u);

  frame.set_adc(boundary_channel, boundary_sample, 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(0, 3), 0x0000u);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, boundary_channel), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, 1), 0x0000u);

  frame.set_adc(3, 0, max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(0, 3), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, boundary_channel), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, 1), 0x0000u);

  frame.set_adc(1, boundary_sample, 0x1555u);
  BOOST_CHECK_EQUAL(frame.get_adc(0, 3), max_adc);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, boundary_channel), 0x2AAAu);
  BOOST_CHECK_EQUAL(frame.get_adc(boundary_sample, 1), 0x1555u);
}

BOOST_AUTO_TEST_CASE(DAPHNEEthStreamFrame_MetadataMutators)
{
  using dunedaq::fddetdataformats::DAPHNEEthStreamFrame;

  DAPHNEEthStreamFrame frame{};

  frame.set_timestamp(0x0123456789ABCDEFuLL);
  BOOST_CHECK_EQUAL(frame.get_timestamp(), 0x0123456789ABCDEFuLL);

  frame.set_channel(0, 10);
  frame.set_channel(1, 20);
  frame.set_channel(2, 30);
  frame.set_channel(3, 40);

  BOOST_CHECK_EQUAL(frame.get_channel(0), 10);
  BOOST_CHECK_EQUAL(frame.get_channel(1), 20);
  BOOST_CHECK_EQUAL(frame.get_channel(2), 30);
  BOOST_CHECK_EQUAL(frame.get_channel(3), 40);

  BOOST_CHECK_EQUAL(frame.get_channel0(), 10);
  BOOST_CHECK_EQUAL(frame.get_channel1(), 20);
  BOOST_CHECK_EQUAL(frame.get_channel2(), 30);
  BOOST_CHECK_EQUAL(frame.get_channel3(), 40);

  BOOST_CHECK_THROW(frame.get_channel(DAPHNEEthStreamFrame::s_num_channels), std::out_of_range);
  BOOST_CHECK_THROW(frame.set_channel(DAPHNEEthStreamFrame::s_num_channels, 50), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
