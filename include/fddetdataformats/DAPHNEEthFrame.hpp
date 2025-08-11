/**
 * @file DAPHNEEthFrame.hpp
 *
 * Contains declaration of DAPHNEEthFrame, a class for accessing raw WIB eth frames, as used in ProtoDUNE-SP-II
 * 
 * The canonical definition of the DAPHNE format is given in EDMS document 2088726: 
 * https://edms.cern.ch/document/2088726
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

/**
 *  @brief Class for accessing raw WIB eth frames, as used in ProtoDUNE-II
 *
 *  The canonical definition of the WIB format is given in EDMS document 2088713:
 *  https://edms.cern.ch/document/2088726
 */
class DAPHNEEthFrame
{
public:
  // ===============================================================
  // Preliminaries
  // ===============================================================

  // The definition of the format is in terms of 64-bit words
  typedef uint64_t word_t; // NOLINT

  // Dataframe format version
  static constexpr uint8_t version = 1;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 1024;
  static constexpr int s_num_adc_words = s_num_adcs * s_bits_per_adc / s_bits_per_word;

  struct Header
  {	  
    // word_t w0;
    word_t trig_sample : 14;
    word_t rsv_0       : 2;
    word_t threshold   : 14;
    word_t rsv_1       : 2;
    word_t baseline    : 14;
    word_t rsv_2       : 6;
    word_t version     : 4;
    word_t channel     : 8;

    word_t w1;
    word_t w2;
    word_t w3;
    word_t w4;
    word_t w5;
    word_t w6;
  };

  // ===============================================================
  // Data members
  // ===============================================================
  detdataformats::DAQEthHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT

// ===============================================================
// Accessors
// ===============================================================

/**
  * @brief Get the ith ADC value in the frame
  *
  * The ADC words are 14 bits long, stored packed in the data structure. The order is:
  *
  * - 1024 adc values from one daphne channel
  */
uint16_t
get_adc(int i) const // NOLINT
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
void
set_adc(int i, uint16_t val) // NOLINT
{
  if (i < 0 || i >= s_num_adcs)
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

  /** @brief Get the starting 64-bit timestamp of the frame
   */
  uint64_t get_timestamp() const // NOLINT(build/unsigned)
  {
    return daq_header.get_timestamp() ; // NOLINT(build/unsigned)
  }

  /** @brief Set the starting 64-bit timestamp of the frame
   */
  void set_timestamp(const uint64_t new_timestamp) // NOLINT(build/unsigned)
  {
    daq_header.timestamp = new_timestamp;
  }

  /** @brief Get the channel identifier of the frame
   */
  uint8_t get_channel() const // NOLINT(build/unsigned)
  {
    return header.channel ; // NOLINT(build/unsigned)
  }

  /** @brief Set the channel identifier of the frame
   */
  void set_channel(const uint8_t new_channel) // NOLINT(build/unsigned)
  {
    header.channel = new_channel;
  }

};

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
