/**
 * @file Utils_test.cxx Utils Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/Utils.hpp"

#define BOOST_TEST_MODULE Utils_test

#include "boost/test/unit_test.hpp"

#include <iostream>

BOOST_AUTO_TEST_SUITE(Utils_test)

BOOST_AUTO_TEST_CASE(Utils_ADCOperation_SanityChecks)
{
  using wordtype_t = uint32_t;
  constexpr int bits_per_word = std::numeric_limits<wordtype_t>::digits;
  
  constexpr int nrows = 2;
  constexpr int ncols = 3;
  wordtype_t myarr[nrows][ncols] = {{1,2,3}, {1001, 1002, 1003}};

  // First, test get_adc if the ADC representation aligns with the C++ type
  for (int i_r = 0; i_r < nrows; ++i_r) {
    for (int i_c = 0; i_c < ncols; ++i_c) {
      BOOST_REQUIRE_EQUAL(myarr[i_r][i_c], (dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_word>(i_r, i_c, myarr)));
    }
  }

  // Next, test set_adc if the ADC representation aligns with the C++ type

  constexpr wordtype_t inputarr[nrows][ncols] = {{1234, 5678, 9101}, {1121, 3141, 5161}};
  for (int i_r = 0; i_r < nrows; ++i_r) {
    for (int i_c = 0; i_c < ncols; ++i_c) {
      dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_word>(i_r, i_c, inputarr[i_r][i_c], myarr);
    }
  }

  for (int i_r = 0; i_r < nrows; ++i_r) {
    for (int i_c = 0; i_c < ncols; ++i_c) {
      BOOST_REQUIRE_EQUAL(inputarr[i_r][i_c], (dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_word>(i_r, i_c, myarr)));
    }
  }

  // Now let's see what happens when ADC representation doesn't align with the C++ type

  constexpr int bits_per_adc = 6;
  static_assert(bits_per_word % bits_per_adc != 0);
  static_assert((bits_per_word * ncols) % bits_per_adc == 0);

  constexpr int nchannels = nrows;
  constexpr int nadcs = bits_per_word * ncols / bits_per_adc;

  constexpr wordtype_t inputarr2[nchannels][nadcs] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48}
  };

  for (int i_ch = 0; i_ch < nchannels; ++i_ch) {
    for (int i_adc = 0; i_adc < nadcs; ++i_adc) {
      dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_adc>(i_ch, i_adc, inputarr2[i_ch][i_adc], myarr);
    }
  }

  for (int i_ch = 0; i_ch < nchannels; ++i_ch) {
    for (int i_adc = 0; i_adc < nadcs; ++i_adc) {
      BOOST_REQUIRE_EQUAL(inputarr2[i_ch][i_adc], (dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_adc>(i_ch, i_adc, myarr)));      
    }
  }

  // Note that in the range checks below, 0 is an obviously good index and -1 is an obviously bad index
  
  constexpr int bad_adc_index = ncols * bits_per_word / bits_per_adc;
  constexpr wordtype_t good_adc_val = 1;
  constexpr wordtype_t bad_adc_val = static_cast<wordtype_t>(1) << bits_per_adc;
  
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_adc>(-1, 0, myarr)),
		    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_adc>(nrows, 0, myarr)),
		    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_adc>(0, -1, myarr)),
		    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::get_adc<wordtype_t, nrows, ncols, bits_per_adc>(0, bad_adc_index, myarr)),
		    std::out_of_range);

  BOOST_CHECK_THROW((dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_adc>(-1, 0, good_adc_val, myarr)),
		    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_adc>(nrows, 0, good_adc_val, myarr)),
		    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_adc>(0, -1, good_adc_val, myarr)),
		    std::out_of_range);
  BOOST_CHECK_THROW((dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_adc>(0, bad_adc_index, good_adc_val, myarr)),
		    std::out_of_range);

  BOOST_CHECK_THROW((dunedaq::fddetdataformats::set_adc<wordtype_t, nrows, ncols, bits_per_adc>(0, 0, bad_adc_val, myarr)),
		    std::out_of_range);

}

BOOST_AUTO_TEST_SUITE_END()
