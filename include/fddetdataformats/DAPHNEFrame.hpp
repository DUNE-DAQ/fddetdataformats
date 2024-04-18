/**
 * @file DAPHNEFrame.hpp
 *
 *  Contains declaration of DAPHNEFrame, a class for accessing raw DAPHNE frames, as produced by the DAPHNE boards
 *
 *  The canonical definition of the PDS DAPHNE format is given in EDMS document 2088726:
 *  https://edms.cern.ch/document/2088726/3
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_DAPHNE_DAPHNEFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_DAPHNE_DAPHNEFRAME_HPP_

#include "detdataformats/DAQHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range
#include <cstdint>  // For uint32_t etc

namespace dunedaq {
namespace fddetdataformats {

class DAPHNEFrame
{
public:
  // ===============================================================
  // Preliminaries
  // ===============================================================

  // The definition of the format is in terms of 32-bit words
  typedef uint32_t word_t; // NOLINT

  // Dataframe format version
  static constexpr uint8_t version = 2;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 1024;
  static constexpr int s_num_adc_words = s_num_adcs * s_bits_per_adc / s_bits_per_word;

  struct Header
  {
    word_t channel : 6, algorithm_id : 4, reserved_1 : 5, r1: 1, trigger_sample_value : 16;
    word_t threshold : 16, baseline : 16;
  };

  struct Trailer
  {
    // Trailer word 1
    word_t num_peak_ub_0 : 4, num_peak_ob_0 : 4, charge_0 : 23, da_0 : 1;
    // Trailer word 2
    word_t max_peak_0 : 14, time_peak_0 : 9, time_pulse_0 : 9;
    // Trailer word 2
    word_t num_peak_ub_1 : 4, num_peak_ob_1 : 4, charge_1 : 23, da_1 : 1;
    // Trailer word 3
    word_t max_peak_1 : 14, time_peak_1 : 9, time_pulse_1 : 9;
    // Trailer word 4
    word_t num_peak_ub_2 : 4, num_peak_ob_2 : 4, charge_2 : 23, da_2 : 1;
    // Trailer word 5
    word_t max_peak_2 : 14, time_peak_2 : 9, time_pulse_2 : 9;
    // Trailer word 6
    word_t num_peak_ub_3 : 4, num_peak_ob_3 : 4, charge_3 : 23, da_3 : 1;
    // Trailer word 7
    word_t max_peak_3 : 14, time_peak_3 : 9, time_pulse_3 : 9;
    // Trailer word 7
    word_t num_peak_ub_4 : 4, num_peak_ob_4 : 4, charge_4 : 23, da_4 : 1;
    // Trailer word 8
    word_t max_peak_4 : 14, time_peak_4 : 9, time_pulse_4 : 9;
    // Trailer word 9
    word_t reserved_2 : 2, time_pulse_ob_2 : 10, time_pulse_ob_1 : 10, time_pulse_ob_0 : 10;
    word_t reserved_3 : 12, time_pulse_ob_4 : 10, time_pulse_ob_3 : 10;

    word_t trailer;
  };

  // ===============================================================
  // Data members
  // ===============================================================
  detdataformats::DAQHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT
  Trailer trailer;
  
  // ===============================================================
  // Accessors
  // ===============================================================

  /**
   * @brief Get the ith ADC value in the frame
   *
   * The ADC words are 14 bits long, stored packed in the data structure. The order is:
   *
   * - 1024 adc values from one daphne channels
   */
  uint16_t get_adc(int i) const // NOLINT
  {
    if (i < 0 || i >= s_num_adcs)
      throw std::out_of_range("ADC index out of range");

    // The index of the first (and sometimes only) word containing the required ADC value
    int word_index = s_bits_per_adc * i / s_bits_per_word;
    assert(word_index < s_num_adc_words);
    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (s_bits_per_adc * i) % s_bits_per_word;
    // How many bits of our desired ADC are located in the `word_index`th word
    int bits_from_first_word = std::min(s_bits_per_adc, s_bits_per_word - first_bit_position);
    uint16_t adc = adc_words[word_index] >> first_bit_position; // NOLINT
    // If we didn't get the full 14 bits from this word, we need the rest from the next word
    if (bits_from_first_word < s_bits_per_adc) {
      assert(word_index + 1 < s_num_adc_words);
      adc |= adc_words[word_index + 1] << bits_from_first_word;
    }
    // Mask out all but the lowest 14 bits;
    return adc & 0x3FFFu;
  }

  /**
   * @brief Set the ith ADC value in the frame to @p val
   */
  void set_adc(int i, uint16_t val) // NOLINT
  {
    if (i < 0 || i >=  s_num_adcs)
      throw std::out_of_range("ADC index out of range");
    if (val >= (1 << s_bits_per_adc))
      throw std::out_of_range("ADC value out of range");

    // The index of the first (and sometimes only) word containing the required ADC value
    int word_index = s_bits_per_adc * i / s_bits_per_word;
    assert(word_index < s_num_adc_words);
    // Where in the word the lowest bit of our ADC value is located
    int first_bit_position = (s_bits_per_adc * i) % s_bits_per_word;
    // How many bits of our desired ADC are located in the `word_index`th word
    int bits_in_first_word = std::min(s_bits_per_adc, s_bits_per_word - first_bit_position);
    uint32_t mask = (1 << (first_bit_position)) - 1;
    adc_words[word_index] = ((val << first_bit_position) & ~mask) | (adc_words[word_index] & mask);
    // If we didn't put the full 14 bits in this word, we need to put the rest in the next word
    if (bits_in_first_word < s_bits_per_adc) {
      assert(word_index + 1 < s_num_adc_words);
      mask = (1 << (s_bits_per_adc - bits_in_first_word)) - 1;
      adc_words[word_index + 1] = ((val >> bits_in_first_word) & mask) | (adc_words[word_index + 1] & ~mask);
    }
  }

  /** @brief Get the channel from the DAPHNE frame
   */
  uint8_t get_channel() const { return header.channel; } // NOLINT(build/unsigned)

  /** @brief Set the channel of the DAPHNE frame
   */
  void set_channel( uint8_t val) { header.channel = val& 0x3Fu; } // NOLINT(build/unsigned)

  /** @brief Get the 64-bit timestamp of the frame
   */
  uint64_t get_timestamp() const // NOLINT(build/unsigned)
  {
    return daq_header.get_timestamp();
  }

};

} // namespace detdataformats
} // namespace dunedaq

#endif // FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_DAPHNE_DAPHNEFRAME_HPP_

// Local Variables:
// c-basic-offset: 2
// End:
