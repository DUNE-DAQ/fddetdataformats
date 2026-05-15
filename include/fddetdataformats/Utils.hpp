/**
 * @file Utils.hpp
 *
 *  As the name would suggest, this header contains utility functions
 *  applicable across the fddetdataformats package
 *
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_

namespace dunedaq::fddetdataformats {

  template<typename WordType, int Rows, int Columns>
  void set_adc(const int i_channel, const int i_adc, WordType adc_val, WordType (&adc_arr)[Rows][Columns]) {
    adc_arr[i_channel][i_adc] = adc_val;
  }

  template<typename WordType, int Rows, int Columns>
  WordType get_adc(const int i_channel, const int i_adc, const WordType (&adc_arr)[Rows][Columns]) {
    return adc_arr[i_channel][i_adc];
  }
  
} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
