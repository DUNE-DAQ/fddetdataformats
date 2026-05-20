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
  
  // get_adc handles ADC values whose widths may not align with the
  // C++ type (WordType) containing them. Note that below, the term
  // "column" refers to a physical index (e.g., the third uint32_t in
  // adc_array) and "ADC index" refers to a logical index (e.g., the
  // third ADC value, which may or may not overlay with the third
  // uint32_t)
  
  template<typename WordType, int Columns, int BitsPerADC>
  WordType get_adc(const int i_adc, const WordType (&adc_array)[Columns]) {

    static_assert(std::is_integral_v<WordType> &&
		  std::is_unsigned_v<WordType>,
		  "WordType must be an unsigned integral type");

    constexpr int bits_per_word = std::numeric_limits<WordType>::digits; // Fine since we know integer is unsigned
    static_assert((Columns * bits_per_word) % BitsPerADC == 0);
    static_assert(BitsPerADC >= 0 && BitsPerADC <= bits_per_word);
    
    constexpr int max_adc_index = Columns * bits_per_word / BitsPerADC - 1;

    if (i_adc < 0 || i_adc > max_adc_index) {
      throw std::out_of_range(std::format("Requested index of {} is outside of allowed range 0-{} ({}-bit ADCs stored in {} {}-bit unsigned types)", i_adc, max_adc_index, BitsPerADC, Columns, bits_per_word));
    }

    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_adc / bits_per_word;
    assert(i_word < Columns);

    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_adc) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_from_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    WordType adc_val = adc_array[i_word] >> first_bit_position; // NOLINT(build/unsigned)

    // If we didn't get the full BitsPerADC bits from this word, we need the rest from the next word
    if (bits_from_first_word < BitsPerADC) {
      assert(i_word < Columns - 1);
      adc_val |= adc_array[i_word + 1] << bits_from_first_word;
    }

    // Mask out all but the lowest BitsPerADC bits;
    return adc_val & ((static_cast<WordType>(1) << BitsPerADC) - 1);
  }

  // Rows and Columns refer to physical rows and columns of WordType
  // in the adc_matrix object; i_sample and i_adc refer to logical rows
  // and columns of ADC values in the event. However, logical rows
  // align with physical rows, so this can't be used, e.g., for DAPHNE
  // streaming data.
  
  template<typename WordType, int Rows, int Columns, int BitsPerADC>
  WordType get_adc(const int i_sample, const int i_adc, const WordType (&adc_matrix)[Rows][Columns]) {
    
    if (i_sample < 0 || i_sample >= Rows) {
      throw std::out_of_range(std::format("Requested index of {}th 1-d ADC array is outside of allowed range 0-{}", i_sample, Rows - 1));
    }

    return get_adc<WordType, Columns, BitsPerADC>(i_adc, adc_matrix[i_sample]);
  }


  // See comment above "get_adc" to understand columns vs. adc indices here
  template<typename WordType, int Columns, int BitsPerADC>
  void set_adc(const int i_adc, WordType adc_val, WordType (&adc_matrix)[Columns]) {
    static_assert(std::is_integral_v<WordType> &&
		  std::is_unsigned_v<WordType>,
		  "WordType must be an unsigned integral type");
    
    constexpr int bits_per_word = std::numeric_limits<WordType>::digits; // Fine since we know integer is unsigned
    static_assert((Columns * bits_per_word) % BitsPerADC == 0);
    static_assert(BitsPerADC >= 0 && BitsPerADC <= bits_per_word);

    constexpr int max_adc_index = Columns * bits_per_word / BitsPerADC - 1;

    if (i_adc < 0 || i_adc > max_adc_index) {
      throw std::out_of_range(std::format("Requested channel index of {} is outside of allowed range 0-{} ({}-bit ADCs stored in {} {}-bit unsigned types)", i_adc, max_adc_index, BitsPerADC, Columns, bits_per_word));
    }

    if (BitsPerADC < bits_per_word && adc_val >= static_cast<WordType>(1) << BitsPerADC) {
      throw std::out_of_range(std::format("Desired ADC value of {} exceeds the maximum of {}", adc_val, (static_cast<WordType>(1) << BitsPerADC) - 1));
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
    adc_matrix[i_word] = (adc_matrix[i_word] & ~adc_mask) | ((static_cast<WordType>(adc_val) << first_bit_position) & adc_mask);

    // If we didn't put the full BitsPerADC bits in this word, we need to put the rest in the next word                        
    if (bits_from_first_word < BitsPerADC) {
      assert(i_word < Columns - 1);
      int bits_from_second_word = BitsPerADC - bits_from_first_word;
      WordType adc_mask2 = (static_cast<WordType>(1) << bits_from_second_word) - 1;
      adc_matrix[i_word + 1] = (adc_matrix[i_word + 1] & ~adc_mask2) | ((adc_val >> bits_from_first_word) & adc_mask2);
    }
  }

  // Rows and Columns refer to physical rows and columns of WordType
  // in the adc_matrix object; i_sample and i_adc refer to logical
  // rows and columns of ADC values in the event

  template<typename WordType, int Rows, int Columns, int BitsPerADC>
  void set_adc(const int i_sample, const int i_adc, WordType adc_val, WordType (&adc_matrix)[Rows][Columns]) {
    if (i_sample < 0 || i_sample >= Rows) {
      throw std::out_of_range(std::format("Requested index of {}th 1-d ADC array is outside of allowed range 0-{}", i_sample, Rows - 1));
    }

    set_adc<WordType, Columns, BitsPerADC>(i_adc, adc_val, adc_matrix[i_sample]);
  }

  // get_adc_daphnestream is, as the name suggests, intended for
  // DAPHNE stream frames. It can handle physical 1-d C++ arrays which
  // contain logical 2-d arrays of ADC values. Note that it uses the
  // nomenclature of DAPHNE Streams, where we have adc values
  // organized by channel (so, ADC #0 values for four channels,
  // followed by ADC #1 values for four channels, etc.)
  
  template<typename WordType, int Columns, int BitsPerADC, int ADCSPerChannel, int NChannels>
  WordType get_adc_daphnestream(const int i_adc, const int i_channel, const WordType (&adc_matrix)[Columns]) {
    constexpr int bits_per_word = std::numeric_limits<WordType>::digits;

    static_assert(ADCSPerChannel * NChannels * BitsPerADC == Columns * bits_per_word);
    static_assert(BitsPerADC >= 0 && BitsPerADC <= bits_per_word);

    if (i_channel < 0 || i_channel >= NChannels) {
      throw std::out_of_range(std::format("Requested channel of {} is out of channel range 0-{}", i_channel, NChannels - 1));
    }

    if (i_adc < 0 || i_adc >= ADCSPerChannel) {
      throw std::out_of_range(std::format("Requested ADC index of {} if out of range 0-{}", i_adc, ADCSPerChannel -1));
    }

    // find absolute index in frame
    int i_abs = i_adc * NChannels + i_channel;

    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_abs / bits_per_word;
    assert(i_word < Columns);

    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_abs) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_from_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    WordType adc_val = adc_matrix[i_word] >> first_bit_position; // NOLINT(build/unsigned)

    if (bits_from_first_word < BitsPerADC) {
      assert(i_word < Columns - 1);
      adc_val |= adc_matrix[i_word + 1] << bits_from_first_word;
    }

    // Mask out all but the lowest BitsPerADC bits;
    return adc_val & ((static_cast<WordType>(1) << BitsPerADC) - 1);
  }

    template<typename WordType, int Columns, int BitsPerADC, int ADCSPerChannel, int NChannels>
  void set_adc_daphnestream(const int i_adc, const int i_channel, const WordType adc_val, WordType (&adc_matrix)[Columns]) {
    constexpr int bits_per_word = std::numeric_limits<WordType>::digits;

    static_assert(ADCSPerChannel * NChannels * BitsPerADC == Columns * bits_per_word);
    static_assert(BitsPerADC >= 0 && BitsPerADC <= bits_per_word);

    if (i_channel < 0 || i_channel >= NChannels) {
      throw std::out_of_range(std::format("Requested channel of {} is out of channel range 0-{}", i_channel, NChannels - 1));
    }

    if (i_adc < 0 || i_adc >= ADCSPerChannel) {
      throw std::out_of_range(std::format("Requested ADC index of {} is out of range 0-{}", i_adc, ADCSPerChannel -1));
    }

    if (BitsPerADC < bits_per_word && adc_val >= (1 << BitsPerADC)) {
      throw std::out_of_range(std::format("Requested ADC value of {} exceeds max value of {}", adc_val, (1 << BitsPerADC) - 1));
    }

    // find absolute index in frame
    int i_abs = i_adc * NChannels + i_channel;
    
    // The index of the first (and sometimes only) word containing the required ADC value
    int i_word = BitsPerADC * i_abs / bits_per_word;
    assert(i_word < Columns);
    
    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (BitsPerADC * i_abs) % bits_per_word;

    // How many bits of our desired ADC are located in the `i_word`th word
    int bits_in_first_word = std::min(BitsPerADC, bits_per_word - first_bit_position);

    WordType mask = ((static_cast<WordType>(1) << bits_in_first_word) - 1) << first_bit_position;
    
    adc_matrix[i_word] = (adc_matrix[i_word] & ~mask) |
      ((static_cast<WordType>(adc_val) << first_bit_position) & mask);

    // If we didn't put the full 14 bits in this word, we need to put the rest in the next word
    if (bits_in_first_word < BitsPerADC) {
      assert(i_word < Columns - 1);
      int bits_in_second_word = BitsPerADC - bits_in_first_word;
      WordType mask2 = (static_cast<WordType>(1) << bits_in_second_word) - 1;
      adc_matrix[i_word + 1] = (adc_matrix[i_word + 1] & ~mask2) | ((adc_val >> bits_in_first_word) & mask2);
    }
  }

  
} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_UTILS_HPP_
