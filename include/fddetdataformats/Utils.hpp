/**
 * @file Utils.hpp
 *
 *  As the name would suggest, this header contains utility functions
 *  applicable across the fddetdataformats package
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_

#include <algorithm>
#include <cassert>
#include <format>
#include <limits>
#include <stdexcept> // Provides std::out_of_range

namespace dunedaq::fddetdataformats {

// get_adc_2d_as_1d will fetch an ADC value from a physical 1-d C++
// array of WordTypes which can contain a logical 2-d array of
// ADCs. It exists because the "blob of bytes" in DAPHNE streams
// represent a logical 2-d array of ADC values: starting with ADC #0
// values in four channels side-by-side, followed by ADC #1 values
// in four channels side-by-side, etc.

// Of course, for other readout types, a logical 1-d array of ADCs
// is simply a special case which this function can handle; the
// "NChannels" template parameter just needs to be set to 1. In this
// way, it can handle all of our get_adc needs.

template<typename WordType, int NWords, int BitsPerADC, int ADCSPerChannel, int NChannels>
WordType
get_adc_2d_as_1d(const int i_adc, const int i_channel, const WordType (&adc_matrix)[NWords])  // NOLINT(modernize-avoid-c-arrays)
{

  static_assert(std::is_integral_v<WordType> && std::is_unsigned_v<WordType>,
                "WordType must be an unsigned integral type");

  constexpr int bits_per_word = std::numeric_limits<WordType>::digits; // Fine since we know integer is unsigned

  static_assert(BitsPerADC > 0 && BitsPerADC <= bits_per_word);
  static_assert(ADCSPerChannel * NChannels * BitsPerADC == NWords * bits_per_word);

  if (i_channel < 0 || i_channel >= NChannels) {
    throw std::out_of_range(
      std::format("Requested channel of {} is out of channel range 0-{}", i_channel, NChannels - 1));
  }

  if (i_adc < 0 || i_adc >= ADCSPerChannel) {
    throw std::out_of_range(std::format("Requested ADC index of {} if out of range 0-{}", i_adc, ADCSPerChannel - 1));
  }

  // find absolute index in frame
  int i_abs = i_adc * NChannels + i_channel;

  if constexpr (BitsPerADC == bits_per_word) {
    return adc_matrix[i_abs];
  } else {

    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_abs / bits_per_word;
    assert(i_word < NWords);

    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_abs) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_from_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    WordType adc_val = adc_matrix[i_word] >> first_bit_position; // NOLINT(build/unsigned)

    if (bits_from_first_word < BitsPerADC) {
      assert(i_word < NWords - 1);
      adc_val |= adc_matrix[i_word + 1] << bits_from_first_word;
    }

    // Mask out all but the lowest BitsPerADC bits;
    return adc_val & ((static_cast<WordType>(1) << BitsPerADC) - 1);

  } // if BitsPerADC != bits_per_word
}

// See above comment on "get_adc_2d_as_1d" to understand the structure "set_adc_2d_as_1d" is working with

template<typename WordType, int NWords, int BitsPerADC, int ADCSPerChannel, int NChannels>
void
set_adc_2d_as_1d(const int i_adc, const int i_channel, const WordType adc_val, WordType (&adc_matrix)[NWords]) // NOLINT(modernize-avoid-c-arrays)
{ 
  static_assert(std::is_integral_v<WordType> && std::is_unsigned_v<WordType>,
                "WordType must be an unsigned integral type");

  constexpr int bits_per_word = std::numeric_limits<WordType>::digits; // Fine since we know integer is unsigned

  static_assert(BitsPerADC > 0 && BitsPerADC <= bits_per_word);
  static_assert(ADCSPerChannel * NChannels * BitsPerADC == NWords * bits_per_word);

  if (i_channel < 0 || i_channel >= NChannels) {
    throw std::out_of_range(
      std::format("Requested channel of {} is out of channel range 0-{}", i_channel, NChannels - 1));
  }

  if (i_adc < 0 || i_adc >= ADCSPerChannel) {
    throw std::out_of_range(std::format("Requested ADC index of {} is out of range 0-{}", i_adc, ADCSPerChannel - 1));
  }

  if constexpr (BitsPerADC < bits_per_word) {
    if (adc_val >= (static_cast<WordType>(1) << BitsPerADC)) {
      throw std::out_of_range(std::format(
        "Requested ADC value of {} exceeds max value of {}", adc_val, (static_cast<WordType>(1) << BitsPerADC) - 1));
    }
  }

  // find absolute index in frame
  int i_abs = i_adc * NChannels + i_channel;

  if constexpr (BitsPerADC == bits_per_word) {
    adc_matrix[i_abs] = adc_val;
  } else {

    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_abs / bits_per_word;
    assert(i_word < NWords);

    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_abs) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_in_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    WordType mask = ((static_cast<WordType>(1) << bits_in_first_word) - 1) << first_bit_position;

    adc_matrix[i_word] = (adc_matrix[i_word] & ~mask) | ((static_cast<WordType>(adc_val) << first_bit_position) & mask);

    // If we didn't put the full 14 bits in this word, we need to put the rest in the next word
    if (bits_in_first_word < BitsPerADC) {
      assert(i_word < NWords - 1);
      int bits_in_second_word = BitsPerADC - bits_in_first_word;
      WordType mask2 = (static_cast<WordType>(1) << bits_in_second_word) - 1;
      adc_matrix[i_word + 1] = (adc_matrix[i_word + 1] & ~mask2) | ((adc_val >> bits_in_first_word) & mask2);
    }
  } // BitsPerADC != bits_per_word
}

// This get_adc is a convenience wrapper around get_adc_2d_as_1d for when
// the logical array of ADCs is 1-d rather than 2-d

template<typename WordType, int NWords, int BitsPerADC>
WordType
get_adc_1d(const int i_adc, const WordType (&adc_array)[NWords]) // NOLINT(modernize-avoid-c-arrays)
{

  static_assert(std::is_integral_v<WordType> && std::is_unsigned_v<WordType>,
                "WordType must be an unsigned integral type");

  constexpr int bits_per_word = std::numeric_limits<WordType>::digits; // Fine since we know integer is unsigned

  static_assert(BitsPerADC > 0);
  constexpr int num_adcs = NWords * bits_per_word / BitsPerADC;

  return get_adc_2d_as_1d<WordType, NWords, BitsPerADC, num_adcs, 1>(i_adc, 0, adc_array);
}

// get_adc_2d can be used when the data is represented as a 2-d array.
// Rows and Columns refer to physical rows and columns of WordType
// in the adc_matrix object; i_sample and i_adc refer to logical rows
// and columns of ADC values in the event. However, logical rows
// align with physical rows, so this can't be used, e.g., for DAPHNE
// streaming data - get_adc_2d_as_1d needs to be used for that.

template<typename WordType, int Rows, int Columns, int BitsPerADC>
WordType
get_adc_2d(const int i_sample, const int i_adc, const WordType (&adc_matrix)[Rows][Columns]) // NOLINT(modernize-avoid-c-arrays)
{ 

  if (i_sample < 0 || i_sample >= Rows) {
    throw std::out_of_range(
      std::format("Requested index of {}th 1-d ADC array is outside of allowed range 0-{}", i_sample, Rows - 1));
  }

  return get_adc_1d<WordType, Columns, BitsPerADC>(i_adc, adc_matrix[i_sample]);
}

// This set_adc is a convenience wrapper around set_adc_2d_as_1d for when
// the logical array of ADCs is 1-d rather than 2-d

template<typename WordType, int NWords, int BitsPerADC>
void
set_adc_1d(const int i_adc, WordType adc_val, WordType (&adc_array)[NWords]) // NOLINT(modernize-avoid-c-arrays)
{ 
  static_assert(std::is_integral_v<WordType> && std::is_unsigned_v<WordType>,
                "WordType must be an unsigned integral type");

  constexpr int bits_per_word = std::numeric_limits<WordType>::digits; // Fine since we know integer is unsigned

  static_assert(BitsPerADC > 0 && BitsPerADC <= bits_per_word);
  static_assert((NWords * bits_per_word) % BitsPerADC == 0);

  constexpr int num_adcs = NWords * bits_per_word / BitsPerADC;

  set_adc_2d_as_1d<WordType, NWords, BitsPerADC, num_adcs, 1>(i_adc, 0, adc_val, adc_array);
}

// Rows and Columns refer to physical rows and columns of WordType
// in the adc_matrix object; i_sample and i_adc refer to logical
// rows and columns of ADC values in the event

template<typename WordType, int Rows, int Columns, int BitsPerADC>
void
set_adc_2d(const int i_sample, const int i_adc, WordType adc_val, WordType (&adc_matrix)[Rows][Columns])  // NOLINT(modernize-avoid-c-arrays)
{
  if (i_sample < 0 || i_sample >= Rows) {
    throw std::out_of_range(
      std::format("Requested index of {}th 1-d ADC array is outside of allowed range 0-{}", i_sample, Rows - 1));
  }

  set_adc_1d<WordType, Columns, BitsPerADC>(i_adc, adc_val, adc_matrix[i_sample]);
}

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
