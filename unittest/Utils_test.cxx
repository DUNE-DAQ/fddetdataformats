/**
 * @file Utils_test.cxx Utils Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/Utils.hpp"

#define BOOST_TEST_MODULE Utils_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <iostream>
#include <limits>

// NOLINTBEGIN(build/unsigned)

BOOST_AUTO_TEST_SUITE(Utils_test)

BOOST_AUTO_TEST_CASE(Utils_ADCOperation_LowLevelChecks)
{
  // Start with something really low level

  constexpr int tiny_adc_width = 3;

  // Three bytes chosen since adc_width needs to evenly divide into the total bits of the array
  constexpr int nbytes = 3;
  uint8_t tiny_adc_arr[nbytes] = { 0, 0, 0 }; // NOLINT(modernize-avoid-c-arrays)

  // the third (index 2) _logical_ 3-bit ADC entry of 7 (111 in binary) means
  // the last two bits of the first byte and the first bit of the
  // second byte should be flipped on:

  // 00000011 10000000 00000000

  dunedaq::fddetdataformats::set_adc_1d<uint8_t, nbytes, tiny_adc_width>(2, 7, tiny_adc_arr);
  BOOST_REQUIRE_EQUAL(tiny_adc_arr[0], 192);
  BOOST_REQUIRE_EQUAL(tiny_adc_arr[1], 1);
  BOOST_REQUIRE_EQUAL(tiny_adc_arr[2], 0);

  BOOST_REQUIRE_EQUAL(7, (dunedaq::fddetdataformats::get_adc_1d<uint8_t, nbytes, tiny_adc_width>(2, tiny_adc_arr)));

  tiny_adc_arr[0] = tiny_adc_arr[1] = tiny_adc_arr[2] = 0;

  constexpr int adcs_per_channel = 4;
  constexpr int nchannels = 2;
  dunedaq::fddetdataformats::set_adc_2d_as_1d<uint8_t, nbytes, tiny_adc_width, adcs_per_channel, nchannels>(
    1, 0, 7, tiny_adc_arr);
  BOOST_REQUIRE_EQUAL(tiny_adc_arr[0], 192);
  BOOST_REQUIRE_EQUAL(tiny_adc_arr[1], 1);
  BOOST_REQUIRE_EQUAL(tiny_adc_arr[2], 0);
  BOOST_REQUIRE_EQUAL(
    7,
    (dunedaq::fddetdataformats::get_adc_2d_as_1d<uint8_t, nbytes, tiny_adc_width, adcs_per_channel, nchannels>(
      1, 0, tiny_adc_arr)));
}

BOOST_AUTO_TEST_CASE(Utils_ADCOperation_BasicChecks)
{

  using wordtype_t = uint32_t;
  constexpr int bits_per_word = std::numeric_limits<wordtype_t>::digits;

  constexpr int nrows = 2;
  constexpr int ncols = 3;
  wordtype_t myarr[nrows][ncols] = { { 1, 2, 3 }, { 1001, 1002, 1003 } }; // NOLINT(modernize-avoid-c-arrays)

  // First, test get_adc_2d if the ADC representation aligns with the C++ type
  for (int i_r = 0; i_r < nrows; ++i_r) {
    for (int i_c = 0; i_c < ncols; ++i_c) {
      BOOST_REQUIRE_EQUAL(myarr[i_r][i_c],
                          (dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_word>(
                            i_r, i_c, myarr))); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
  }

  // Next, test set_adc_1d if the ADC representation aligns with the C++ type

  constexpr wordtype_t inputarr[nrows][ncols] = { { 1234, 5678, 9101 }, // NOLINT(modernize-avoid-c-arrays)
                                                  { 1121, 3141, 5161 } };
  for (int i_r = 0; i_r < nrows; ++i_r) {
    for (int i_c = 0; i_c < ncols; ++i_c) {
      dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_word>(
        i_r, i_c, inputarr[i_r][i_c], myarr); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
  }

  for (int i_r = 0; i_r < nrows; ++i_r) {
    for (int i_c = 0; i_c < ncols; ++i_c) {
      BOOST_REQUIRE_EQUAL(inputarr[i_r][i_c],
                          (dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_word>(
                            i_r, i_c, myarr))); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
  }

  // Now let's see what happens when ADC representation doesn't align with the C++ type

  constexpr int bits_per_adc = 6;
  static_assert(bits_per_word % bits_per_adc != 0);
  static_assert((bits_per_word * ncols) % bits_per_adc == 0);

  constexpr int nsamples = nrows;
  constexpr int nchannels = bits_per_word * ncols / bits_per_adc;

  constexpr wordtype_t inputarr2[nsamples][nchannels] = { // NOLINT(modernize-avoid-c-arrays)
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48 }
  };

  for (int i_smp = 0; i_smp < nsamples; ++i_smp) {
    for (int i_ch = 0; i_ch < nchannels; ++i_ch) {
      dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(
        i_smp, i_ch, inputarr2[i_smp][i_ch], myarr); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
  }

  for (int i_smp = 0; i_smp < nsamples; ++i_smp) {
    for (int i_ch = 0; i_ch < nchannels; ++i_ch) {
      BOOST_REQUIRE_EQUAL(
        inputarr2[i_smp][i_ch], // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        (dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(i_smp, i_ch, myarr)));
    }
  }

  // Note that in the range checks below, 0 is an obviously good index and -1 is an obviously bad index

  constexpr int bad_channel_index = ncols * bits_per_word / bits_per_adc;
  constexpr wordtype_t good_adc_val = 1;
  constexpr wordtype_t bad_adc_val = static_cast<wordtype_t>(1) << bits_per_adc;

  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(-1, 0, myarr)),
                    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(nrows, 0, myarr)),
                    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(0, -1, myarr)),
                    std::out_of_range);
  BOOST_CHECK_THROW(
    (dunedaq::fddetdataformats::get_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(0, bad_channel_index, myarr)),
    std::out_of_range);

  BOOST_CHECK_THROW(
    (dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(-1, 0, good_adc_val, myarr)),
    std::out_of_range);
  BOOST_CHECK_THROW(
    (dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(nrows, 0, good_adc_val, myarr)),
    std::out_of_range);
  BOOST_CHECK_THROW(
    (dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(0, -1, good_adc_val, myarr)),
    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(
                      0, bad_channel_index, good_adc_val, myarr)),
                    std::out_of_range);

  BOOST_CHECK_THROW(
    (dunedaq::fddetdataformats::set_adc_2d<wordtype_t, nrows, ncols, bits_per_adc>(0, 0, bad_adc_val, myarr)),
    std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()

// NOLINTEND(build/unsigned)
