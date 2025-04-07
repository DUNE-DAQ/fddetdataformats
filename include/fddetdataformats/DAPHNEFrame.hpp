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
#include <cstdint>   // For uint32_t etc

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
    word_t channel : 6, algorithm_id : 4, reserved_1 : 5, r1 : 1, trigger_sample_value : 16;
    word_t threshold : 16, baseline : 16;
    word_t get_baseline() { return baseline; }
  };

  struct Trailer
  {
    // Word 1: TP0 odd
    // Declared in reverse order (LSB first) so that:
    //   - num_peak_0 occupies bits [3:0]
    //   - tbd_0 occupies bits [7:4]
    //   - adc_integral_0 occupies bits [30:8]
    //   - da_0 occupies bit [31]
    word_t num_peak_0       : 4;   // Num_Peak [3:0]
    word_t tbd_0            : 4;   // TBD      [7:4]
    word_t adc_integral_0   : 23;  // ADC_Integral [30:8]
    word_t da_0             : 1;   // DA       [31]

    // Word 2: TP0 even
    // Declared (LSB first) so that:
    //   - adc_peak_0 occupies bits [13:0]
    //   - time_peak_0 occupies bits [22:14]
    //   - time_over_baseline_0 occupies bits [31:23]
    word_t adc_peak_0         : 14;  // ADC_Peak         [13:0]
    word_t time_peak_0        : 9;   // Time_Peak        [22:14]
    word_t time_over_baseline_0 : 9; // Time_Over_Baseline [31:23]

    // Word 3: TP1 odd
    word_t num_peak_1       : 4;   // Num_Peak [3:0]
    word_t tbd_1            : 4;   // TBD      [7:4]
    word_t adc_integral_1   : 23;  // ADC_Integral [30:8]
    word_t da_1             : 1;   // DA       [31]

    // Word 4: TP1 even
    word_t adc_peak_1         : 14; // ADC_Peak         [13:0]
    word_t time_peak_1        : 9;  // Time_Peak        [22:14]
    word_t time_over_baseline_1 : 9;// Time_Over_Baseline [31:23]

    // Word 5: TP2 odd
    word_t num_peak_2       : 4;   // Num_Peak [3:0]
    word_t tbd_2            : 4;   // TBD      [7:4]
    word_t adc_integral_2   : 23;  // ADC_Integral [30:8]
    word_t da_2             : 1;   // DA       [31]

    // Word 6: TP2 even
    word_t adc_peak_2         : 14; // ADC_Peak         [13:0]
    word_t time_peak_2        : 9;  // Time_Peak        [22:14]
    word_t time_over_baseline_2 : 9;// Time_Over_Baseline [31:23]

    // Word 7: TP3 odd
    word_t num_peak_3       : 4;   // Num_Peak [3:0]
    word_t tbd_3            : 4;   // TBD      [7:4]
    word_t adc_integral_3   : 23;  // ADC_Integral [30:8]
    word_t da_3             : 1;   // DA       [31]

    // Word 8: TP3 even
    word_t adc_peak_3         : 14; // ADC_Peak         [13:0]
    word_t time_peak_3        : 9;  // Time_Peak        [22:14]
    word_t time_over_baseline_3 : 9;// Time_Over_Baseline [31:23]

    // Word 9: TP4 odd
    word_t num_peak_4       : 4;   // Num_Peak [3:0]
    word_t tbd_4            : 4;   // TBD      [7:4]
    word_t adc_integral_4   : 23;  // ADC_Integral [30:8]
    word_t da_4             : 1;   // DA       [31]

    // Word 10: TP4 even
    word_t adc_peak_4         : 14; // ADC_Peak         [13:0]
    word_t time_peak_4        : 9;  // Time_Peak        [22:14]
    word_t time_over_baseline_4 : 9;// Time_Over_Baseline [31:23]

    // Word 11: Time_Start fields for indices 0,1,2 and TBD
    // Declared in LSB-first order:
    //   - time_start_2 occupies bits [11:2]
    //   - time_start_1 occupies bits [21:12]
    //   - time_start_0 occupies bits [31:22]
    //   - tbd_5 occupies bits [1:0]
    word_t time_start_2     : 10;  // Time_Start(2) [11:2]
    word_t time_start_1     : 10;  // Time_Start(1) [21:12]
    word_t time_start_0     : 10;  // Time_Start(0) [31:22]
    word_t tbd_5            : 2;   // TBD         [1:0]

    // Word 12: Time_Start fields for indices 3,4 and TBD
    // Declared in LSB-first order:
    //   - tbd_6 occupies bits [11:0]
    //   - time_start_4 occupies bits [21:12]
    //   - time_start_3 occupies bits [31:22]
    word_t tbd_6            : 12;  // TBD         [11:0]
    word_t time_start_4     : 10;  // Time_Start(4) [21:12]
    word_t time_start_3     : 10;  // Time_Start(3) [31:22]

    // Word 13: Trailer word (all 32 bits), typically 0xFFFFFFFF.
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
  // Private Helper: Reinterpret Trailer as an array of word_t
  // ===============================================================
  inline const word_t* get_trailer_words() const {
    return reinterpret_cast<const word_t*>(&trailer);
  }
  inline word_t* get_trailer_words() {
    return reinterpret_cast<word_t*>(&trailer);
  }

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

  /** @brief Get the channel from the DAPHNE frame
   */
  uint8_t get_channel() const { return header.channel; } // NOLINT(build/unsigned)

  /** @brief Set the channel of the DAPHNE frame
  */
  void set_channel( uint8_t val) { header.channel = val & 0x3Fu; } // NOLINT(build/unsigned)
  
  // --- Trailer Accessors (Manual Shift–Mask Extraction) ---

  /**
  * @brief Get the DA value for a specific TP (channel) from the trailer.
  *        (Word 2*TP, bit 31)
  */
  inline uint8_t get_da(int TP) const // TP index 0 to 4
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    const word_t* tw = get_trailer_words();
    // In odd word, DA is in bit 31.
    return static_cast<uint8_t>((tw[2*TP] >> 31) & 0x1);
  }

  /**
  * @brief Set the DA value for a specific TP (channel) in the trailer.
  */
  inline void set_da(uint8_t val, int TP)
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    if (val > 1)
      throw std::out_of_range("DA value out of range (must be 0-1)");
    word_t* tw = get_trailer_words();
    tw[2*TP] = (tw[2*TP] & ~(1u << 31)) | ((val & 0x1) << 31);
  }

  /**
  * @brief Get the ADC_Integral value for a specific TP.
  *        (Word 2*TP, bits [30:8])
  */
  inline uint32_t get_adc_integral(int TP) const
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    const word_t* tw = get_trailer_words();
    return (tw[2*TP] >> 8) & 0x7FFFFF; // Mask 23 bits
  }

  /**
  * @brief Set the ADC_Integral value for a specific TP.
  */
  inline void set_adc_integral(uint32_t val, int TP)
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    if (val > 0x7FFFFF)
      throw std::out_of_range("ADC_Integral value out of range (must be 0-8388607)");
    word_t* tw = get_trailer_words();
    tw[2*TP] = (tw[2*TP] & ~(0x7FFFFFu << 8)) | ((val & 0x7FFFFF) << 8);
  }

  /**
  * @brief Get the Num_Peak value for a specific TP.
  *        (Word 2*TP, bits [3:0])
  */
  inline uint8_t get_num_peak(int TP) const
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    const word_t* tw = get_trailer_words();
    return static_cast<uint8_t>(tw[2*TP] & 0xF);
  }

  /**
  * @brief Set the Num_Peak value for a specific TP.
  */
  inline void set_num_peak(uint8_t val, int TP)
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    if (val > 0xF)
      throw std::out_of_range("Num_Peak value out of range (must be 0-15)");
    word_t* tw = get_trailer_words();
    tw[2*TP] = (tw[2*TP] & ~0xF) | (val & 0xF);
  }

  /**
  * @brief Get the Time_Over_Baseline value for a specific TP.
  *        (Word 2*TP+1, bits [8:0])
  */
  inline uint16_t get_time_over_baseline(int TP) const
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    const word_t* tw = get_trailer_words();
    return static_cast<uint16_t>((tw[2*TP+1] >> 23) & 0x1FF);
  }

  /**
  * @brief Set the Time_Over_Baseline value for a specific TP.
  */
  inline void set_time_over_baseline(uint16_t val, int TP)
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    if (val > 0x1FF)
      throw std::out_of_range("Time_Over_Baseline value out of range (must be 0-511)");
    word_t* tw = get_trailer_words();
    tw[2*TP+1] = (tw[2*TP+1] & ~(0x1FFu << 23)) | ((val & 0x1FF) << 23);
  }

  /**
  * @brief Get the Time_Peak value for a specific TP.
  *        (Word 2*TP+1, bits [17:9])
  */
  inline uint16_t get_time_peak(int TP) const
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    const word_t* tw = get_trailer_words();
    return static_cast<uint16_t>((tw[2*TP+1] >> 14) & 0x1FF);
  }


  /**
  * @brief Set the Time_Peak value for a specific TP.
  */
  inline void set_time_peak(uint16_t val, int TP)
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    if (val > 0x1FF)
      throw std::out_of_range("Time_Peak value out of range (must be 0-511)");
    word_t* tw = get_trailer_words();
    tw[2*TP+1] = (tw[2*TP+1] & ~(0x1FFu << 14)) | ((val & 0x1FF) << 14);
  }

  /**
  * @brief Get the ADC_Peak value for a specific TP.
  *        (Word 2*TP+1, bits [31:18])
  */
  inline uint16_t get_adc_peak(int TP) const
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    const word_t* tw = get_trailer_words();
    // Even word for TP is at index 2*TP+1; ADC_Peak is in bits 13:0.
    return static_cast<uint16_t>(tw[2*TP+1] & 0x3FFF);
  }

  /**
  * @brief Set the ADC_Peak value for a specific TP.
  */
  inline void set_adc_peak(uint16_t val, int TP)
  {
    if (TP < 0 || TP > 4)
      throw std::out_of_range("TP index out of range (must be 0-4)");
    if (val > 0x3FFF)
      throw std::out_of_range("ADC_Peak value out of range (must be 0-16383)");
    word_t* tw = get_trailer_words();
    tw[2*TP+1] = (tw[2*TP+1] & ~0x3FFFu) | (val & 0x3FFF);
  }

  /**
  * @brief Get the Time_Start value for a given index (0-4).
  *
  * For indices 0,1,2 these are stored in trailer word 11 (index 10):
  *   - index 0: bits [9:0]
  *   - index 1: bits [19:10]
  *   - index 2: bits [29:20]
  *
  * For indices 3,4 these are stored in trailer word 12 (index 11):
  *   - index 3: bits [9:0]
  *   - index 4: bits [19:10]
  */
  inline uint16_t get_time_start(int idx) const
  {
    if (idx < 0 || idx > 4)
      throw std::out_of_range("Time_Start index out of range (must be 0-4)");

    const word_t* tw = get_trailer_words();
    if (idx < 3) {
      int shift = 22 - 10 * idx;
      return static_cast<uint16_t>((tw[10] >> shift) & 0x3FF);
    } else {
      int shift = 22 - 10 * (idx - 3);
      return static_cast<uint16_t>((tw[11] >> shift) & 0x3FF);
    }
  }

  /**
  * @brief Set the time_start field for TP index 0–4 using bit shifts.
  * 
  * Trailer word 11 (index 10):
  *   - idx 0: bits [31:22]
  *   - idx 1: bits [21:12]
  *   - idx 2: bits [11:2]
  * Trailer word 12 (index 11):
  *   - idx 3: bits [31:22]
  *   - idx 4: bits [21:12]
  */
  inline void set_time_start(int idx, uint16_t val)
  {
    if (idx < 0 || idx > 4)
      throw std::out_of_range("Time_Start index out of range (must be 0–4)");
    if (val > 0x3FF)
      throw std::out_of_range("Time_Start value out of range (must be 0–1023)");

    word_t* tw = get_trailer_words();
    word_t mask = 0x3FFu;

    if (idx < 3) {
      int shift = 22 - 10 * idx;
      tw[10] = (tw[10] & ~(mask << shift)) | ((val & mask) << shift);
    } else {
      int shift = 22 - 10 * (idx - 3);
      tw[11] = (tw[11] & ~(mask << shift)) | ((val & mask) << shift);
    }
  }

  /** @brief Get the 64-bit timestamp of the frame
  */
  uint64_t get_timestamp() const // NOLINT(build/unsigned)
  {
    return daq_header.get_timestamp();
  }



};

 } // namespace fddetdataformats
 } // namespace dunedaq

 #endif // FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_DAPHNE_DAPHNEFRAME_HPP_

 // Local Variables:
 // c-basic-offset: 2
 // End: