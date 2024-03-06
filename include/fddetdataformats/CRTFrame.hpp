/**
 * @file CRTFrame.hpp
 *
 *  Contains declaration of CRTFrame, a class for accessing raw CRT frames
 *  of fixed size, as produced by
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_CRT_CRTFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_CRT_CRTFRAME_HPP_

#include "detdataformats/DAQEthHeader.hpp"

#include <cassert>   // For assert()
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range
#include <cstdint>  // For uint32_t etc

namespace dunedaq {
namespace fddetdataformats {

class CRTFrame
{
public:
  // ===============================================================
  // Preliminaries
  // ===============================================================

  // The definition of the format is in terms of 32-bit words
  typedef uint32_t word_t; // NOLINT

  static constexpr int s_bits_per_adc = 16;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 64;

  struct Header
  {
    word_t magic_number : 8, hit_count : 8, module_number : 16;
    word_t upper_global_timestamp;
    word_t lower_global_timestamp;
    word_t sixteen_ns_counter_timestamp;
  };

  struct HitWord
  {
    word_t hit_magic_number : 8, hit_channel : 8, hit_adc : 16;
  };

  // ===============================================================
  // Data members
  // ===============================================================
  detdataformats::DAQEthHeader daq_header;
  Header header;
  HitWord adc_words[s_num_adcs]; // NOLINT
  
  // ===============================================================
  // Accessors
  // ===============================================================

  /**
   * @brief Get the ith ADC value in the frame
   *
   * Each ADC word is 32 bits long, with the structure: 
   * - 8 bits "magic number": 0x48 = "H" 
   * - 8 bits channel number: unsigned integer 0-63
   * - 16 bits ADC value: *signed* integer 
   * 
   * There are 64 channels in a module, so 64 ADC words in a frame
   * - Many will have an ADC value of zero
   */
  int16_t get_adc(int i) const // NOLINT
  {
    if (i < 0 || i >= s_num_adcs)
      throw std::out_of_range("ADC index out of range");

    return static_cast<int16_t>(adc_words[i].hit_adc);
  }

  /**
   * @brief Set the ith ADC value in the frame to @p val
   */
  void set_adc(int i, int16_t val) // NOLINT
  {
    if (i < 0 || i >=  s_num_adcs)
      throw std::out_of_range("ADC index out of range");
    if (val >= (1 << s_bits_per_adc))
      throw std::out_of_range("ADC value out of range");

    adc_words[i].hit_adc = val;
  }

  /**
   * @brief Get the channel of the ith hit in the frame
   */
  uint8_t get_channel(int i)
  {
    if (i < 0 || i >=  s_num_adcs)
      throw std::out_of_range("ADC index out of range");
  
    return static_cast<uint8_t>(adc_words[i].hit_channel); 
  }

  /**
   * @brief Set the channel of the ith hit in the frame to @p val
   */
  void set_channel(int i, uint8_t val)
  {
    if (i < 0 || i >=  s_num_adcs)
      throw std::out_of_range("ADC index out of range");
   
    adc_words[i].hit_channel = val; 
  }

  /** @brief Get the module from the CRT frame
   */
  uint16_t get_module() const // NOLINT(build/unsigned) 
  {
    return header.module_number; 
  }

  /** @brief Set the module of the CRT frame
   */
  void set_module( uint16_t val) // NOLINT(build/unsigned) 
  { 
    header.module_number = val; 
  }

  /** @brief Get the 64-bit timestamp of the frame
   */
  uint64_t get_timestamp() const // NOLINT(build/unsigned)
  {
    return daq_header.get_timestamp();
  }

  /** @brief Set the starting 64-bit timestamp of the frame
   */
  void set_timestamp(const uint64_t new_timestamp) // NOLINT(build/unsigned)
  {
    daq_header.timestamp = new_timestamp;
  }
};

} // namespace detdataformats
} // namespace dunedaq

#endif // FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_CRT_CRTFRAME_HPP_

// Local Variables:
// c-basic-offset: 2
// End:
