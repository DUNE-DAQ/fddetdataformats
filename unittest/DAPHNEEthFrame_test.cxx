/**
 * @file DAPHNEEthFrame_test.cxx DAPHNEEthFrame class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
**/

#include "fddetdataformats/DAPHNEEthFrame.hpp"

#define BOOST_TEST_MODULE DAPHNEEthFrame_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <algorithm>
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

BOOST_AUTO_TEST_CASE(DAPHNEEthFrame_PeakDescriptorMutators)
{
  constexpr int N_PEAKS = dunedaq::fddetdataformats::DAPHNEEthFrame::s_max_peaks;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint16_t> adc_dist(1, (1 << 14) - 1);
  std::uniform_int_distribution<uint16_t> u10bit(0, 0x3FF);
  std::uniform_int_distribution<uint16_t> u9bit(0, 0x1FF);
  std::uniform_int_distribution<uint16_t> u4bit(0, 0xF);
  std::uniform_int_distribution<uint32_t> u23bit(0, 0x7FFFFF);
  std::uniform_int_distribution<uint16_t> u14bit(0, 0x3FFF);
  std::uniform_int_distribution<uint8_t> u1bit(0, 1);

  dunedaq::fddetdataformats::DAPHNEEthFrame frame{};

  BOOST_CHECK_THROW(frame.get_peaks_data().set_found(true, N_PEAKS), std::out_of_range);


  for (int peak = 0; peak < N_PEAKS; ++peak) {
    uint8_t num_subpeaks = u4bit(gen);
    uint8_t found = u1bit(gen);
    uint32_t adc_integral = u23bit(gen);
    uint16_t adc_max = u14bit(gen);
    uint16_t sample_peak = u9bit(gen);
    uint16_t tob = u9bit(gen);
    uint16_t sample_start = u10bit(gen);

    frame.get_peaks_data().set_num_subpeaks(num_subpeaks, peak);
    frame.get_peaks_data().set_found(found, peak);
    frame.get_peaks_data().set_adc_integral(adc_integral, peak);
    frame.get_peaks_data().set_adc_max(adc_max, peak);
    frame.get_peaks_data().set_sample_max(sample_peak, peak);
    frame.get_peaks_data().set_samples_over_baseline(tob, peak);
    frame.get_peaks_data().set_sample_start(sample_start, peak);
  
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_num_subpeaks(peak), num_subpeaks);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().is_found(peak), found);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_adc_integral(peak), adc_integral);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_adc_max(peak), adc_max);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_sample_max(peak), sample_peak);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_samples_over_baseline(peak), tob);
    BOOST_CHECK_EQUAL(frame.get_peaks_data().get_sample_start(peak), sample_start);
  }

}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
