/**
 * @file Utils.hpp
 *
 *  As the name would suggest, this header contains utility functions
 *  applicable across the fddetdataformats package
 *
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_

#include <cassert>
#include <format>
#include <stdexcept> // Provides std::out_of_range

namespace dunedaq::fddetdataformats {
  
  // Rows and Columns refer to physical rows and columns of WordType
  // in the adc_matrix object; i_channel and i_adc refer to logical
  // rows and columns of ADC values in the event
  
  template<typename WordType, int Rows, int Columns, int BitsPerADC>
  WordType get_adc(const int i_channel, const int i_adc, const WordType (&adc_matrix)[Rows][Columns]) {

    static_assert(std::is_integral_v<WordType> &&
		  std::is_unsigned_v<WordType>,
		  "WordType must be an unsigned integral type");
    constexpr int bits_per_word = std::numeric_limits<WordType>::digits;
    constexpr int max_adc_index = Columns * bits_per_word / BitsPerADC - 1;
    
    if (i_channel < 0 || i_channel >= Rows) {
      throw std::out_of_range(std::format("Requested channel index of {} is outside of allowed range 0-{}", i_channel, Rows - 1));
    }

    if (i_adc < 0 || i_adc > max_adc_index) {
      throw std::out_of_range(std::format("Requested adc index of {} is outside of allowed range 0-{}", i_adc, max_adc_index));
    }

    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_adc / bits_per_word;
    assert(i_word < Columns);

    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_adc) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_from_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    WordType adc = adc_matrix[i_channel][i_word] >> first_bit_position; // NOLINT(build/unsigned)

    // If we didn't get the full BitsPerADC bits from this word, we need the rest from the next word
    if (bits_from_first_word < BitsPerADC) {
      assert(i_word < Columns - 1);
      adc |= adc_matrix[i_channel][i_word + 1] << bits_from_first_word;
    }
    
    // Mask out all but the lowest BitsPerADC bits;
    return adc & ((static_cast<WordType>(1) << BitsPerADC) - 1);
  }

  // Rows and Columns refer to physical rows and columns of WordType
  // in the adc_matrix object; i_channel and i_adc refer to logical
  // rows and columns of ADC values in the event

  template<typename WordType, int Rows, int Columns, int BitsPerADC>
  void set_adc(const int i_channel, const int i_adc, WordType adc_val, WordType (&adc_matrix)[Rows][Columns]) {
    static_assert(std::is_integral_v<WordType> &&
		  std::is_unsigned_v<WordType>,
		  "WordType must be an unsigned integral type");

    constexpr int bits_per_word = std::numeric_limits<WordType>::digits;
    constexpr int max_adc_index = Columns * bits_per_word / BitsPerADC - 1;

    if (i_channel < 0 || i_channel >= Rows) {
      throw std::out_of_range(std::format("Requested channel index of {} is outside of allowed range 0-{}", i_channel, Rows - 1));
    }

    if (i_adc < 0 || i_adc > max_adc_index) {
      throw std::out_of_range(std::format("Requested adc index of {} is outside of allowed range 0-{}", i_adc, max_adc_index));
    }

    if (BitsPerADC < bits_per_word && adc_val >= static_cast<WordType>(1) << BitsPerADC) {
      throw std::out_of_range(std::format("Requested adc value of {} exceeds the maximum of {}", adc_val, (static_cast<WordType>(1) << BitsPerADC) - 1));
    }

    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_adc / bits_per_word;
    assert(i_word < Columns);

    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_adc) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_from_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    // Create a mask that covers exactly the bits we want to modify (bits_from_first_word bits starting at first_bit_position)
    WordType adc_mask = ((static_cast<WordType>(1) << bits_from_first_word) - 1) << first_bit_position;
    adc_matrix[i_channel][i_word] = (adc_matrix[i_channel][i_word] & ~adc_mask) | ((static_cast<WordType>(adc_val) << first_bit_position) & adc_mask);

    // If we didn't put the full BitsPerADC bits in this word, we need to put the rest in the next word                        
    if (bits_from_first_word < BitsPerADC) {
      assert(i_word < Columns - 1);
      int bits_from_second_word = BitsPerADC - bits_from_first_word;
      WordType adc_mask2 = (static_cast<WordType>(1) << bits_from_second_word) - 1;
      adc_matrix[i_channel][i_word + 1] = (adc_matrix[i_channel][i_word + 1] & ~adc_mask2) | ((adc_val >> bits_from_first_word) & adc_mask2);
    }
  }

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
