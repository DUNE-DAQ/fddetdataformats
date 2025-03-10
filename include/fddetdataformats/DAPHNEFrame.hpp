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
    // Trailer word 1
    word_t num_peak_ub_0 : 4, num_peak_ob_0 : 4, charge_0 : 23, da_0 : 1;
    // Trailer word 2
    word_t max_peak_0 : 14, time_peak_0 : 9, time_pulse_0 : 9;
    // Trailer word 3
    word_t num_peak_ub_1 : 4, num_peak_ob_1 : 4, charge_1 : 23, da_1 : 1;
    // Trailer word 4
    word_t max_peak_1 : 14, time_peak_1 : 9, time_pulse_1 : 9;
    // Trailer word 5
    word_t num_peak_ub_2 : 4, num_peak_ob_2 : 4, charge_2 : 23, da_2 : 1;
    // Trailer word 6
    word_t max_peak_2 : 14, time_peak_2 : 9, time_pulse_2 : 9;
    // Trailer word 7
    word_t num_peak_ub_3 : 4, num_peak_ob_3 : 4, charge_3 : 23, da_3 : 1;
    // Trailer word 8
    word_t max_peak_3 : 14, time_peak_3 : 9, time_pulse_3 : 9;
    // Trailer word 9
    word_t num_peak_ub_4 : 4, num_peak_ob_4 : 4, charge_4 : 23, da_4 : 1;
    // Trailer word 10
    word_t max_peak_4 : 14, time_peak_4 : 9, time_pulse_4 : 9;
    // Trailer word 11
    word_t reserved_2 : 2, time_pulse_ob_2 : 10, time_pulse_ob_1 : 10, time_pulse_ob_0 : 10;
    // Trailer word 12
    word_t reserved_3 : 12, time_pulse_ob_4 : 10, time_pulse_ob_3 : 10;
    // Trailer word 13
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
   * @brief Get the num_peak_ub value for a specific trigger primitive.
   *        (Under-baseline peaks; TP index in [0,4])
   */
   inline uint8_t get_num_peak_ub(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // num_peak_ub_i is stored in word (2*TP), bits [3:0]
     return static_cast<uint8_t>(tw[2 * TP] & 0xF);
   }
 
   /**
   * @brief Set the num_peak_ub value for a specific trigger primitive.
   */
   inline void set_num_peak_ub(uint8_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x0F)
       throw std::out_of_range("num_peak_ub value out of range (must be 0-15)");
     word_t* tw = get_trailer_words();
     tw[2 * TP] = (tw[2 * TP] & ~0xF) | (val & 0xF);
   }
 
   /**
   * @brief Get the num_peak_ob value for a specific trigger primitive.
   *        (Over-baseline peaks; TP index in [0,4])
   */
   inline uint8_t get_num_peak_ob(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // num_peak_ob_i is stored in word (2*TP), bits [7:4]
     return static_cast<uint8_t>((tw[2 * TP] >> 4) & 0xF);
   }
 
   /**
   * @brief Set the num_peak_ob value for a specific trigger primitive.
   */
   inline void set_num_peak_ob(uint8_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x0F)
       throw std::out_of_range("num_peak_ob value out of range (must be 0-15)");
     word_t* tw = get_trailer_words();
     // Clear bits [7:4] then set the new value
     tw[2 * TP] = (tw[2 * TP] & ~(0xF << 4)) | ((val & 0xF) << 4);
   }
 
   /**
   * @brief Get the charge value for a specific trigger primitive.
   */
   inline uint32_t get_charge(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // charge_i is stored in word (2*TP), bits [30:8]
     return (tw[2 * TP] >> 8) & 0x7FFFFF;
   }
 
   /**
   * @brief Set the charge value for a specific trigger primitive.
   */
   inline void set_charge(uint32_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x7FFFFF)
       throw std::out_of_range("charge value out of range (must be 0-8388607)");
     word_t* tw = get_trailer_words();
     tw[2 * TP] = (tw[2 * TP] & ~(((word_t)0x7FFFFF) << 8)) | ((val & 0x7FFFFF) << 8);
   }
 
   /**
   * @brief Get the da value for a specific trigger primitive.
   */
   inline uint8_t get_da(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // da_i is stored in word (2*TP), bit 31
     return static_cast<uint8_t>((tw[2 * TP] >> 31) & 0x1);
   }
 
   /**
   * @brief Set the da value for a specific trigger primitive.
   */
   inline void set_da(uint8_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x1)
       throw std::out_of_range("da value out of range (must be 0-1)");
     word_t* tw = get_trailer_words();
     tw[2 * TP] = (tw[2 * TP] & ~(1u << 31)) | ((val & 0x1) << 31);
   }
 
   /**
   * @brief Get the max_peak value for a specific trigger primitive.
   */
   inline uint16_t get_max_peak(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // max_peak_i is stored in word (2*TP+1), bits [13:0]
     return static_cast<uint16_t>(tw[2 * TP + 1] & 0x3FFF);
   }
 
   /**
   * @brief Set the max_peak value for a specific trigger primitive.
   */
   inline void set_max_peak(uint16_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x3FFF)
       throw std::out_of_range("max_peak value out of range (must be 0-16383)");
     word_t* tw = get_trailer_words();
     tw[2 * TP + 1] = (tw[2 * TP + 1] & ~0x3FFF) | (val & 0x3FFF);
   }
 
   /**
   * @brief Get the time_peak value for a specific trigger primitive.
   */
   inline uint16_t get_time_peak(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // time_peak_i is stored in word (2*TP+1), bits [22:14]
     return static_cast<uint16_t>((tw[2 * TP + 1] >> 14) & 0x1FF);
   }
 
   /**
   * @brief Get the time_pulse value for a specific trigger primitive.
   */
   inline uint16_t get_time_pulse(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     // time_pulse_i is stored in word (2*TP+1), bits [31:23]
     return static_cast<uint16_t>((tw[2 * TP + 1] >> 23) & 0x1FF);
   }
 
   /**
   * @brief Set the time_pulse value for a specific trigger primitive.
   */
   inline void set_time_pulse(uint16_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x1FF)
       throw std::out_of_range("time_pulse value out of range (must be 0-511)");
     word_t* tw = get_trailer_words();
     tw[2 * TP + 1] = (tw[2 * TP + 1] & ~(((word_t)0x1FF) << 23)) | ((val & 0x1FF) << 23);
   }
 
   /**
   * @brief Get the time_pulse_ob value for a specific trigger primitive.
   *
   * For TP 0,1,2 the value is in trailer word 11 (tw[10]):
   *   - TP0: bits [31:22]
   *   - TP1: bits [21:12]
   *   - TP2: bits [11:2]
   *
   * For TP 3,4 the value is in trailer word 12 (tw[11]):
   *   - TP3: bits [31:22]
   *   - TP4: bits [21:12]
   */
   inline uint16_t get_time_pulse_ob(int TP) const // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     const word_t* tw = get_trailer_words();
     if (TP < 3) {
       if (TP == 0)
         return static_cast<uint16_t>((tw[10] >> 22) & 0x3FF);
       else if (TP == 1)
         return static_cast<uint16_t>((tw[10] >> 12) & 0x3FF);
       else // TP == 2
         return static_cast<uint16_t>((tw[10] >> 2) & 0x3FF);
     } else {
       if (TP == 3)
         return static_cast<uint16_t>((tw[11] >> 22) & 0x3FF);
       else // TP == 4
         return static_cast<uint16_t>((tw[11] >> 12) & 0x3FF);
     }
   }
 
   /**
   * @brief Set the time_pulse_ob value for a specific trigger primitive.
   */
   inline void set_time_pulse_ob(uint16_t val, int TP) // NOLINT(build/unsigned)
   {
     if (TP < 0 || TP > 4)
       throw std::out_of_range("Trigger primitive index out of range (must be 0-4)");
     if (val > 0x3FF)
       throw std::out_of_range("time_pulse_ob value out of range (must be 0-1023)");
     word_t* tw = get_trailer_words();
     if (TP < 3) {
       if (TP == 0)
         tw[10] = (tw[10] & ~(0x3FFu << 22)) | ((val & 0x3FFu) << 22);
       else if (TP == 1)
         tw[10] = (tw[10] & ~(0x3FFu << 12)) | ((val & 0x3FFu) << 12);
       else // TP == 2
         tw[10] = (tw[10] & ~(0x3FFu << 2)) | ((val & 0x3FFu) << 2);
     } else {
       if (TP == 3)
         tw[11] = (tw[11] & ~(0x3FFu << 22)) | ((val & 0x3FFu) << 22);
       else // TP == 4
         tw[11] = (tw[11] & ~(0x3FFu << 12)) | ((val & 0x3FFu) << 12);
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