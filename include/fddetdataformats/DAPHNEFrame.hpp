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
#include "trgdataformats/TriggerPrimitivePDS.hpp"
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
    word_t get_baseline(){return baseline;}
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
  
  /**
   * @brief Get the num_peak_ub value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4)
   * @return The number of peaks under baseline for the specified trigger primitive
   */
  uint8_t get_num_peak_ub(int TP) const // NOLINT(build/unsigned)    
  {
    switch (TP) {
      case 0: return trailer.num_peak_ub_0;
      case 1: return trailer.num_peak_ub_1;
      case 2: return trailer.num_peak_ub_2;
      case 3: return trailer.num_peak_ub_3;
      case 4: return trailer.num_peak_ub_4;
      default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  /**
   * @brief Set the num_peak_ub value for a specific trigger primitive in the trailer
   * @param val The number of peaks under baseline (0-15)
   * @param TP The trigger primitive index (0-4)
   */
  void set_num_peak_ub(uint8_t val, int TP) // NOLINT(build/unsigned)
  {
    if (val > 0x0F) {
      throw std::out_of_range("num_peak_ub value out of range (must be 0-15)");
    }
    
    switch (TP) {
    case 0: trailer.num_peak_ub_0 = val & 0x0F; break;
    case 1: trailer.num_peak_ub_1 = val & 0x0F; break;
    case 2: trailer.num_peak_ub_2 = val & 0x0F; break;
    case 3: trailer.num_peak_ub_3 = val & 0x0F; break;
    case 4: trailer.num_peak_ub_4 = val & 0x0F; break;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Get the num_peak_ob value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4)
   * @return The number of peaks over baseline for the specified trigger primitive
   */
  uint8_t get_num_peak_ob(int TP) const // NOLINT(build/unsigned)                    
  {
    switch (TP) {
    case 0: return trailer.num_peak_ob_0;
    case 1: return trailer.num_peak_ob_1;
    case 2: return trailer.num_peak_ob_2;
    case 3: return trailer.num_peak_ob_3;
    case 4: return trailer.num_peak_ob_4;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }

  /**
   * @brief Set the num_peak_ob value for a specific trigger primitive in the trailer
   * @param val The number of peaks over baseline (0-15)
   * @param TP The trigger primitive index (0-4)                   
   */
  void set_num_peak_ob(uint8_t val, int TP) // NOLINT(build/unsigned)
  {
    if (val > 0x0F) {
      throw std::out_of_range("num_peak_ob value out of range (must be 0-15)");
    }
    switch (TP) {
    case 0: trailer.num_peak_ob_0 = val & 0x0F; break;
    case 1: trailer.num_peak_ob_1 = val & 0x0F; break;
    case 2: trailer.num_peak_ob_2 = val & 0x0F; break;
    case 3: trailer.num_peak_ob_3 = val & 0x0F; break;
    case 4: trailer.num_peak_ob_4 = val & 0x0F; break;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Get the charge value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4) as a word_t type
   * @return The charge value for the specified trigger primitive
   */
  uint32_t get_charge(int TP) const // NOLINT(build/unsigned)                    
  {
    switch (TP) {
    case 0: return trailer.charge_0;
    case 1: return trailer.charge_1;
    case 2: return trailer.charge_2;
    case 3: return trailer.charge_3;
    case 4: return trailer.charge_4;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Set the charge value for a specific trigger primitive in the trailer
   * @param val The charge value (0 - 8388607)
   * @param TP The trigger primitive index (0-4)
   */
  void set_charge(uint32_t val, int TP) // NOLINT(build/unsigned)                                                 
  {
    if (val > 0x7FFFFF) {
      throw std::out_of_range("charge value out of range (must be 0-8388607)");
    }
    switch (TP) {
    case 0: trailer.charge_0 = val & 0x7FFFFF; break;
    case 1: trailer.charge_1 = val & 0x7FFFFF; break;
    case 2: trailer.charge_2 = val & 0x7FFFFF; break;
    case 3: trailer.charge_3 = val & 0x7FFFFF; break;
    case 4: trailer.charge_4 = val & 0x7FFFFF; break;
    default: throw std::out_of_range("Tigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Get the da value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4) as a word_t type
   * @return The da value for the specified trigger primitive
   */
  uint8_t get_da(int TP) const // NOLINT(build/unsigned)
  {
    switch (TP) {
    case 0: return trailer.da_0;
    case 1: return trailer.da_1;
    case 2: return trailer.da_2;
    case 3: return trailer.da_3;
    case 4: return trailer.da_4;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Set the da value for a specific trigger primitive in the trailer
   * @param val The da value (true or false)
   * @param TP The ttrigger primitive index (0-4)
   */
  void set_da(uint8_t val, int TP) // NOLINT(build/unsigned) 
  {
    if (val > 0x01) {
      throw std::out_of_range("da value out of range (must be 0-1)");
    }
    
    switch (TP) {
    case 0: trailer.da_0 = val & 0x01; break;
    case 1: trailer.da_1 = val & 0x01; break;
    case 2: trailer.da_2 = val & 0x01; break;
    case 3: trailer.da_3 = val & 0x01; break;
    case 4: trailer.da_4 = val & 0x01; break;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Get the max_peak value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4) as a word_t type                 
   * @return The max_peak value for the specified trigger primitive
   */
  uint16_t get_max_peak(int TP) const // NOLINT(build/unsigned)                 
  {
    switch (TP) {
    case 0: return static_cast<uint16_t>(trailer.max_peak_0);
    case 1: return static_cast<uint16_t>(trailer.max_peak_1);
    case 2: return static_cast<uint16_t>(trailer.max_peak_2);
    case 3: return static_cast<uint16_t>(trailer.max_peak_3);
    case 4: return static_cast<uint16_t>(trailer.max_peak_4);
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Set the max_peak value for a specific trigger primitive in the trailer
   * @param val The max_peak value (0 - 16383)
   * @param TP The trigger priitive index (0-4)
   */
  void set_max_peak(uint16_t val, int TP) // NOLINT(build/unsigned)                                               
  {
    if (val > 0x3FFF) {
      throw std::out_of_range("max_peak value out of range (must be 0-16383)");
    }
    switch (TP) {
    case 0: trailer.max_peak_0 = val & 0x3FFF; break;
    case 1: trailer.max_peak_1 = val & 0x3FFF; break;
    case 2: trailer.max_peak_2 = val & 0x3FFF; break;
    case 3: trailer.max_peak_3 = val & 0x3FFF; break;
    case 4: trailer.max_peak_4 = val & 0x3FFF; break;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  
  /**
   * @brief Get the time_peak value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4)
   * @return The time_peak value for the specified trigger primitive
   */
  uint16_t get_time_peak(int TP) const // NOLINT(build/unsigned)
    
  {
    switch (TP) {
    case 0: return trailer.time_peak_0;
    case 1: return trailer.time_peak_1;
    case 2: return trailer.time_peak_2;
    case 3: return trailer.time_peak_3;
    case 4: return trailer.time_peak_4;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }


  /**
   * @brief Get the time_pulse value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4)
   * @return The time_pulse value for the specified trigger primitive
   */
  uint16_t get_time_pulse(int TP) const // NOLINT(build/unsigned)
  {
    switch (TP) {
    case 0: return trailer.time_pulse_0;
    case 1: return trailer.time_pulse_1;
    case 2: return trailer.time_pulse_2;
    case 3: return trailer.time_pulse_3;
    case 4: return trailer.time_pulse_4;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }

  /**
   * @brief Set the time_pulse value for a specific trigger primitive in the trailer
   * @param val The time_pulse value (0 - 511)
   * @param TP The trigger primitive index (0-4) as a word_t type
   */
  void set_time_pulse(uint16_t val, int TP) // NOLINT(build/unsigned)
  {
    if (val > 0x1FF) {
      throw std::out_of_range("time_pulse value out of range (must be 0-511)");
    }
    switch (TP) {
    case 0: trailer.time_pulse_0 = val & 0x1FF; break;
    case 1: trailer.time_pulse_1 = val & 0x1FF; break;
    case 2: trailer.time_pulse_2 = val & 0x1FF; break;
    case 3: trailer.time_pulse_3 = val & 0x1FF; break;
    case 4: trailer.time_pulse_4 = val & 0x1FF; break;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }

  /**
   * @brief Get the time_pulse_ob value for a specific trigger primitive
   * @param TP The trigger primitive index (0-4)
   * @return The time_pulse_ob value for the specified trigger primitive
   */
  uint16_t get_time_pulse_ob(int TP) const // NOLINT(build/unsigned)
  {
    switch (TP) {
    case 0: return trailer.time_pulse_ob_0;
    case 1: return trailer.time_pulse_ob_1;
    case 2: return trailer.time_pulse_ob_2;
    case 3: return trailer.time_pulse_ob_3;
    case 4: return trailer.time_pulse_ob_4;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-2)");
    }
  }
  /**
   * @brief Set the time_pulse_ob value for a specific trigger primitive in the trailer
   * @param val The time_pulse_ob value (0 - 1023)
   * @param TP The trigger primitive index (0-4)
   */
  void set_time_pulse_ob(uint16_t val, int TP) // NOLINT(build/unsigned)
  {
    if (val > 0x3FF) {
      throw std::out_of_range("time_pulse_ob value out of range (must be 0-1023)");
    }
    switch (TP) {
    case 0: trailer.time_pulse_ob_0 = val & 0x3FF; break;
    case 1: trailer.time_pulse_ob_1 = val & 0x3FF; break;
    case 2: trailer.time_pulse_ob_2 = val & 0x3FF; break;
    case 3: trailer.time_pulse_ob_3 = val & 0x3FF; break;
    case 4: trailer.time_pulse_ob_4 = val & 0x3FF; break;
    default: throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
    }
  }
  /** @brief Get the 64-bit timestamp of the frame
   */
  uint64_t get_timestamp() const // NOLINT(build/unsigned)
  {
    return daq_header.get_timestamp();
  }
  trgdataformats::TriggerPrimitivePDS get_TP(int i)
  {
    trgdataformats::TriggerPrimitivePDS tp;
    tp.set_channel(daq_header.slot_id*100+get_channel());
    tp.set_num_peak_ub(get_num_peak_ub(i));
    tp.set_num_peak_ob(get_num_peak_ob(i));
    tp.set_adc_integral(get_charge(i));
    tp.set_adc_peak(get_max_peak(i));
    tp.set_time_peak(get_timestamp()+64+get_time_peak(i)); //This time need to to be verified!
    tp.set_time_start(get_timestamp()+64); //This time need to to be verified!
    tp.set_time_over_threshold(get_time_peak(i)+get_time_pulse_ob(i)); //This time need to to be verified!
    return tp;
  }
};

} // namespace detdataformats
} // namespace dunedaq

#endif // FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_DAPHNE_DAPHNEFRAME_HPP_

// Local Variables:
// c-basic-offset: 2
// End:
