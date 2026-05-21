/**
 * @file TDEEthFrame.hpp
 *
 * Contains declaration of TDEEthFrame, a class for accessing raw WIB v2 frames, as used in ProtoDUNE-SP-II
 * 
 * The canonical definition of the WIB format is given in EDMS document 2088713: 
 * https://edms.cern.ch/document/2088713
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_TDEETHFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_TDEETHFRAME_HPP_

#include "Utils.hpp"

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

  // NOLINTBEGIN(build/unsigned)
  
/**
 *  @brief Class for accessing raw WIB eth frames, as used in ProtoDUNE-II
 *
 *  The canonical definition of the WIB format is given in EDMS document 2088713:
 *  https://edms.cern.ch/document/2088713
 */
class TDEEthFrame
{
public:

  // The definition of the format is in terms of 64-bit words
  typedef uint64_t word_t; // NOLINT

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_time_samples_per_frame = 64;
  static constexpr int s_channels_per_half_femb = 64;
  static constexpr int s_half_fembs_per_frame = 1;
  static constexpr int s_num_channels = s_channels_per_half_femb * s_half_fembs_per_frame;
  static constexpr int s_num_adc_words_per_ts = s_num_channels * s_bits_per_adc / s_bits_per_word;

  struct TDEEthHeader
  {
    uint64_t reserved : 26;
    uint64_t tde_errors : 16;
    uint64_t tde_header : 10;
    uint64_t version : 4;
    uint64_t channel : 8;
    uint64_t TAItime : 64;
  };
  static_assert(sizeof(TDEEthHeader) == 16);

  
  detdataformats::DAQEthHeader daq_header;
  TDEEthHeader header;
  word_t adc_words[s_time_samples_per_frame][s_num_adc_words_per_ts]; // NOLINT

  /**
   * @brief Get the i_channel-th ADC value in the i_sample-th time sample
   *
   * The order is: 64 channels repeated for 64 time samples
   * The ADC words are 14 bits long
   *
   */
  uint16_t get_adc(int i_channel, int i_sample=0) const;


  /// @brief Set the i_channel-th ADC value in the i_sample-th time sample to @p val
  void set_adc(int i_channel, int i_sample, uint16_t val);

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const {
    return daq_header.get_timestamp() ;
  }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) {
    daq_header.timestamp = new_timestamp;
  }

  /// @brief Get the channel identifier of the frame
  uint8_t get_channel() const {
    return header.channel ;
  }

  /// @brief Set the channel identifier of the frame
  void set_channel(const uint8_t new_channel) {
    header.channel = new_channel;
  }
};
  static_assert(sizeof(TDEEthFrame) == sizeof(detdataformats::DAQEthHeader) +
		sizeof(TDEEthFrame::TDEEthHeader) +
		sizeof(TDEEthFrame::word_t) * TDEEthFrame::s_time_samples_per_frame * TDEEthFrame::s_num_adc_words_per_ts);


inline uint16_t TDEEthFrame::get_adc(int i_channel, int i_sample) const {

    // Note the generic get_adc_2d function takes the channel and the sample (timeslice) in reverse order
    // Also note that the value returned by get_adc_2d is stored in a uint64_t (TDEEthFrame::word_t) but is
    // guaranteed to be storable in a uint16_t since TDEEthFrame::s_bits_per_adc < 16. 
    
    return static_cast<uint16_t>( dunedaq::fddetdataformats::get_adc_2d<
				  TDEEthFrame::word_t,
				  TDEEthFrame::s_time_samples_per_frame,
				  TDEEthFrame::s_num_adc_words_per_ts,
				  TDEEthFrame::s_bits_per_adc>(
							       i_sample, i_channel,
							       adc_words
							       )
				  );
  }

  inline void TDEEthFrame::set_adc(int i_channel, int i_sample, uint16_t adc_val) {
    dunedaq::fddetdataformats::set_adc_2d<
      TDEEthFrame::word_t,
      TDEEthFrame::s_time_samples_per_frame,
      TDEEthFrame::s_num_adc_words_per_ts,
      TDEEthFrame::s_bits_per_adc>(
				   i_sample, i_channel,
				   adc_val,
				   adc_words
				   );

  }

  // NOLINTEND(build/unsigned)
  
} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_TDEETHFRAME_HPP_
