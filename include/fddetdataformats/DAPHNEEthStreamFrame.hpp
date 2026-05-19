/**
 * @file DAPHNEEthStreamFrame.hpp
 *
 * Contains declaration of DAPHNEEthStreamFrame, a class for accessing raw DAPHNE eth stream frames, as used in ProtoDUNE-SP-II
 * 
 * The canonical definition of the DAPHNE format is given in EDMS document 2088726: 
 * https://edms.cern.ch/document/2088726
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_

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
 *  @brief Class for accessing raw DAPHNE eth stream frames, as used in ProtoDUNE-II
 *
 *  The canonical definition of the DAPHNE format is given in EDMS document 2088713:
 *  https://edms.cern.ch/document/2088726
 */
class DAPHNEEthStreamFrame
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
  static constexpr int s_adcs_per_channel = 280;
  static constexpr int s_num_channels = 4;
  static constexpr int s_num_adc_words = s_num_channels * s_adcs_per_channel * s_bits_per_adc / s_bits_per_word;

  struct ChannelWord
  {
    word_t tbd     : 52;
    word_t version : 4;
    word_t channel : 8;
  };

  struct Header
  {    
    ChannelWord channel_words[s_num_channels];
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
 * @brief Get the @p i ADC value of @p chn in the frame
 */
  uint16_t get_adc(uint i_adc, uint i_channel) const; // NOLINT

/**
 * @brief Set the @p i ADC value of @p chn in the frame to @p val
 */

  void set_adc(uint i_channel, uint i_adc, uint16_t val); // NOLINT
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
  uint8_t get_channel(uint ch) const // NOLINT(build/unsigned)
  {
    if (ch >= s_num_channels)
      throw std::out_of_range("Channel index out of range");

    return header.channel_words[ch].channel ; // NOLINT(build/unsigned)
  }

  /** @brief Set the channel identifier of the frame
   */
  void set_channel(uint channel_index, const uint8_t new_channel_val) // NOLINT(build/unsigned)
  {
    if (channel_index >= s_num_channels)
      throw std::out_of_range("Channel index out of range");

    header.channel_words[channel_index].channel = new_channel_val;
  }
  
  /** @brief Get the channel 0 from the DAPHNE Stream frame header                                                                                                           
   */
  uint8_t get_channel0() const { return header.channel_words[0].channel; } // NOLINT(build/unsigned)                                                                                        

  /** @brief Get the channel 1 from the DAPHNE Stream frame header                                                                                                           
   */
  uint8_t get_channel1() const { return header.channel_words[1].channel; } // NOLINT(build/unsigned)                                                                                        

  /** @brief Get the channel 2 from the DAPHNE Stream frame header                                                                                                           
   */
  uint8_t get_channel2() const { return header.channel_words[2].channel; } // NOLINT(build/unsigned)                                                                                        

  /** @brief Get the channel 3 from the DAPHNE Stream frame header                                                                                                           
   */
  uint8_t get_channel3() const { return header.channel_words[3].channel; } // NOLINT(build/unsigned)  

};

inline uint16_t DAPHNEEthStreamFrame::get_adc(uint i_adc, uint i_channel) const { // NOLINT
  return static_cast<uint16_t>(
			       dunedaq::fddetdataformats::get_adc_daphnestream<
			       word_t,
			       s_num_adc_words,
			       s_bits_per_adc,
			       s_adcs_per_channel,
			       s_num_channels
			       >(
				 static_cast<int>(i_adc),
				 static_cast<int>(i_channel),
				 adc_words
				 )
			       );
}

inline void DAPHNEEthStreamFrame::set_adc(uint i_channel, uint i_adc, uint16_t val) { // NOLINT
  dunedaq::fddetdataformats::set_adc_daphnestream<
    word_t,
    s_num_adc_words,
    s_bits_per_adc,
    s_adcs_per_channel,
    s_num_channels
    >(
      static_cast<int>(i_adc),
      static_cast<int>(i_channel),
      val,
      adc_words
      );
}
  
} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_
