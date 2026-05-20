/**
 * @file WIBEthFrame.hpp
 *
 * Contains declaration of WIBEthFrame, a class for accessing raw WIB eth frames, as used in ProtoDUNE-SP-II
 * 
 * The canonical definition of the WIB format is given in EDMS document 2088713: 
 * https://edms.cern.ch/document/2088713
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_WIBETHFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_WIBETHFRAME_HPP_

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
 *  https://edms.cern.ch/document/2088713
 */
class WIBEthFrame
{
public:
  // ===============================================================
  // Preliminaries
  // ===============================================================

  // The definition of the format is in terms of 64-bit words
  typedef uint64_t word_t; // NOLINT

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_time_samples_per_frame = 64;
  static constexpr int s_channels_per_half_femb = 64;
  static constexpr int s_half_fembs_per_frame = 1;
  static constexpr int s_num_channels = s_channels_per_half_femb * s_half_fembs_per_frame;
  static constexpr int s_num_adc_words_per_ts = s_num_channels * s_bits_per_adc / s_bits_per_word;
  static constexpr int s_num_adc_words = s_time_samples_per_frame * s_num_channels * s_bits_per_adc / s_bits_per_word;
  

  struct WIBEthHeader
  {	  
    word_t colddata_timestamp_0 : 15;
    word_t pad_0: 1;
    word_t colddata_timestamp_1 : 15;
    word_t pad_1: 1;
    word_t cd: 1;
    word_t crc_err : 2;
    word_t link_valid : 2;
    word_t lol : 1;
    word_t wib_sync : 1;
    word_t femb_sync : 2;
    word_t pulser : 1;
    word_t calibration : 1;
    word_t ready : 1;
    word_t context : 8;
    word_t version : 4;
    word_t channel : 8;
    word_t extra_data; 
  };

  // ===============================================================
  // Data members
  // ===============================================================
  detdataformats::DAQEthHeader daq_header;
  WIBEthHeader header;
  // word_t adc_words[s_num_adc_words_per_ts][s_time_samples_per_frame]; // NOLINT
  word_t adc_words[s_time_samples_per_frame][s_num_adc_words_per_ts]; // NOLINT

  // ===============================================================
  // Accessors
  // ===============================================================

  /**
   * @brief Get the ith ADC value in the frame
   *
   * The ADC words are 14 bits long;
   * wrod_t stored packed in the data structure. 
   * The order is: 64 channels repeated for 64 time samples
   *
   */
  uint16_t get_adc(int i_channel, int i_sample=0) const; // NOLINT(build/unsigned)

  /**
   * @brief Set the ith ADC value in the frame to @p val
   */
  void set_adc(int i_channel, int i_sample, uint16_t adc_val); // NOLINT(build/unsigned)

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


  inline uint16_t WIBEthFrame::get_adc(int i_channel, int i_sample) const { // NOLINT(build/unsigned)
    
    // Note the generic get_adc function takes the channel and the sample (timeslice) in reverse order
    return static_cast<uint16_t>(
				 dunedaq::fddetdataformats::get_adc<
				 word_t,
				 s_time_samples_per_frame,
				 s_num_adc_words_per_ts,
				 s_bits_per_adc>(
						 i_sample, i_channel,
						 adc_words
						 )
				 );
  }

  inline void WIBEthFrame::set_adc(int i_channel, int i_sample, uint16_t adc_val) { // NOLINT(build/unsigned)

    // Note the generic set_adc function takes the channel and the sample (timeslice) in reverse order
    dunedaq::fddetdataformats::set_adc<
      word_t,
      s_time_samples_per_frame,
      s_num_adc_words_per_ts,
      s_bits_per_adc>(
		      i_sample, i_channel,
		      adc_val,
		      adc_words
		      );
  }
    
} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_WIBETHFRAME_HPP_
