
/**
 * @file WIBEthFrame_test.cxx WIBEthFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/WIBEthFrame.hpp"

#define BOOST_TEST_MODULE WIBEthFrame_test // NOLINT

#include "boost/test/unit_test.hpp"
#include "logging/Logging.hpp" // For TLOG_DEBUG

#include <random>
#include <string>
#include <vector>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(WIBEthFrame_test)

BOOST_AUTO_TEST_CASE(WIBEthFrame_ADCDataMutators)
{
  // RNG with max ADC-width values
  std::random_device dev;
  std::mt19937 rng(dev());
  int max_adc_value = (static_cast<dunedaq::fddetdataformats::WIBEthFrame::word_t>(1)
                       << dunedaq::fddetdataformats::WIBEthFrame::s_bits_per_adc) -
                      1;
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, max_adc_value);

  // Prepare source vector with ADC samples
  std::vector<std::vector<uint16_t>> v;
  for (int i = 0; i < 64; ++i) {
    v.emplace_back(64); // i.e., emplace back a 64-element 1-d vector
    for (int j = 0; j < 64; ++j) {
      auto rand_val = dist(rng);
      v[i][j] = static_cast<uint16_t>(rand_val);
    }
  }

  // Set ADCs from ADC samples
  dunedaq::fddetdataformats::WIBEthFrame wibethframe{};
  for (std::size_t i = 0; i < v.size(); ++i) {
    for (std::size_t j = 0; j < v[i].size(); ++j) {
      wibethframe.set_adc(i, j, v[i][j]);
    }
  }
  wibethframe.set_adc(
    0,
    0,
    v[0][0]); // Set the first ADC again to check that we can overwrite existing values without affecting other values

  auto original_level = boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_warnings);

  // Get ADCs and compare
  for (std::size_t i = 0; i < v.size(); ++i) {
    for (std::size_t j = 0; j < v[i].size(); ++j) {
      TLOG_DEBUG(1) << "Comparing ADC value for channel " << i << ", sample " << j << ": " << wibethframe.get_adc(i, j)
                    << " vs " << v[i][j];
      BOOST_REQUIRE_EQUAL(wibethframe.get_adc(i, j), v[i][j]);
    }
  }
  boost::unit_test::unit_test_log.set_threshold_level(original_level);
}

BOOST_AUTO_TEST_CASE(WIBEthFrame_IndexAndValueBounds)
{
  using dunedaq::fddetdataformats::WIBEthFrame;

  WIBEthFrame wibethframe{};

  BOOST_CHECK_THROW(wibethframe.get_adc(-1, 0), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.get_adc(WIBEthFrame::s_num_channels, 0), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.get_adc(0, -1), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.get_adc(0, WIBEthFrame::s_time_samples_per_frame), std::out_of_range);

  BOOST_CHECK_THROW(wibethframe.set_adc(-1, 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.set_adc(WIBEthFrame::s_num_channels, 0, 123), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.set_adc(0, -1, 123), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.set_adc(0, WIBEthFrame::s_time_samples_per_frame, 123), std::out_of_range);
  BOOST_CHECK_THROW(wibethframe.set_adc(0, 0, 1 << WIBEthFrame::s_bits_per_adc), std::out_of_range);

  BOOST_CHECK_NO_THROW(wibethframe.set_adc(0, 0, (1 << WIBEthFrame::s_bits_per_adc) - 1));
  BOOST_CHECK_EQUAL(wibethframe.get_adc(0, 0), (1 << WIBEthFrame::s_bits_per_adc) - 1);
}

BOOST_AUTO_TEST_CASE(WIBEthFrame_NeighborIsolationAcrossWordBoundary)
{
  using dunedaq::fddetdataformats::WIBEthFrame;

  WIBEthFrame wibethframe{};
  constexpr auto max_adc = static_cast<uint16_t>((1u << WIBEthFrame::s_bits_per_adc) - 1u);
  constexpr int sample = 5;
  constexpr int boundary_channel = 4;

  wibethframe.set_adc(boundary_channel - 1, sample, 0x0000u);
  wibethframe.set_adc(boundary_channel, sample, 0x0000u);
  wibethframe.set_adc(boundary_channel + 1, sample, 0x0000u);

  wibethframe.set_adc(boundary_channel, sample, 0x2AAAu);

  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel - 1, sample), 0x0000u);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel, sample), 0x2AAAu);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel + 1, sample), 0x0000u);

  wibethframe.set_adc(boundary_channel - 1, sample, max_adc);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel - 1, sample), max_adc);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel, sample), 0x2AAAu);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel + 1, sample), 0x0000u);

  wibethframe.set_adc(boundary_channel + 1, sample, 0x1555u);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel - 1, sample), max_adc);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel, sample), 0x2AAAu);
  BOOST_CHECK_EQUAL(wibethframe.get_adc(boundary_channel + 1, sample), 0x1555u);
}

BOOST_AUTO_TEST_CASE(WIBEthFrame_MetadataMutators)
{
  using dunedaq::fddetdataformats::WIBEthFrame;

  WIBEthFrame wibethframe{};

  wibethframe.set_timestamp(0x0123456789ABCDEFuLL);
  BOOST_CHECK_EQUAL(wibethframe.get_timestamp(), 0x0123456789ABCDEFuLL);

  wibethframe.set_channel(0);
  BOOST_CHECK_EQUAL(wibethframe.get_channel(), 0);

  wibethframe.set_channel(255);
  BOOST_CHECK_EQUAL(wibethframe.get_channel(), 255);
}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
