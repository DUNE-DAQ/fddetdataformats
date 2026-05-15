/**
 * @file Utils_test.cxx Utils Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/Utils.hpp"

#include "UnitTestUtils.hpp"

#define BOOST_TEST_MODULE Utils_test

#include "boost/test/unit_test.hpp"

#include <iostream>

BOOST_AUTO_TEST_SUITE(Utils_test)

BOOST_AUTO_TEST_CASE(Utils_SanityCheck)
{
  using namespace dunedaq::fddetdataformats;

  constexpr int nrows = 2;
  constexpr int ncols = 3;
  uint32_t myarr[nrows][ncols] = {{1,2,3}, {1001, 1002, 1003}};

  set_adc<uint32_t, nrows, ncols>(0, 1, 4000, myarr);

  for (auto i = 0; i < nrows; ++i)
    for (auto j = 0; j < ncols; ++j)
      std::cout << myarr[i][j] << "\n";

  auto generated_adcs = generate_matrix<uint32_t>(nrows, ncols, 2000);
  for (size_t i_r = 0; i_r < generated_adcs.size(); ++i_r) {
    for (size_t i_c = 0; i_c < generated_adcs[i_r].size(); ++i_c) {
      std::cout << generated_adcs[i_r][i_c] << " ";
      set_adc<uint32_t, nrows, ncols>(i_r, i_c, generated_adcs[i_r][i_c], myarr);
    }
    std::cout << "\n";
  }
  
  for (size_t i_r = 0; i_r < generated_adcs.size(); ++i_r) {
    for (size_t i_c = 0; i_c < generated_adcs[i_r].size(); ++i_c) {
      BOOST_REQUIRE_EQUAL(generated_adcs[i_r][i_c], myarr[i_r][i_c]);
    }
  }

}

BOOST_AUTO_TEST_SUITE_END()
