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

#include "Utils.hpp"

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
  static_assert(sizeof(Header) == 7*sizeof(word_t));
  
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
get_adc(int i) const; // NOLINT

/**
  * @brief Set the ith ADC value in the frame to @p val
  */
  void
  set_adc(int i, uint16_t val); // NOLINT

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
  static_assert(sizeof(DAPHNEEthFrame) == sizeof(detdataformats::DAQEthHeader) + sizeof(DAPHNEEthFrame::Header) + sizeof(DAPHNEEthFrame::word_t) * DAPHNEEthFrame::s_num_adc_words);

  inline uint16_t DAPHNEEthFrame::get_adc(int i) const {

    // We're static_casting the returned DAPHNEEthFrame::word_t to a uint16_t, which is fine since
    // the ADC value is guaranteed to be storable in 16 bits

    return static_cast<uint16_t>(
				 dunedaq::fddetdataformats::get_adc<DAPHNEEthFrame::word_t,
				 DAPHNEEthFrame::s_num_adc_words,
				 DAPHNEEthFrame::s_bits_per_adc>(
								 i,
								 adc_words
								 )
				 );
  }

  inline void
  DAPHNEEthFrame::set_adc(int i, uint16_t val) { // NOLINT
    
    dunedaq::fddetdataformats::set_adc<DAPHNEEthFrame::word_t,
				       DAPHNEEthFrame::s_num_adc_words,
				       DAPHNEEthFrame::s_bits_per_adc>(
								    i,
								    val,
								    adc_words
								       );

  }

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
