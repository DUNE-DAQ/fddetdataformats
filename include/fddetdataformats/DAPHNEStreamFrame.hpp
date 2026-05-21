/**
 * @file DAPHNEStreamFrame.hpp
 *
 *  Contains declaration of DAPHNEStreamFrame, a class for accessing 
 *  raw DAPHNE streaming version frames, as produced by the DAPHNE boards
 *
 *  The canonical definition of the PDS DAPHNE format is given in EDMS document 2088726:
 *  https://edms.cern.ch/document/2088726/3
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNESTREAMFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNESTREAMFRAME_HPP_

#include "Utils.hpp"

#include "detdataformats/DAQHeader.hpp" // For unified DAQ header

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range
#include <cstdint>  // For uint32_t etc

namespace dunedaq::fddetdataformats {

  // NOLINTBEGIN(build/unsigned)
  
class DAPHNEStreamFrame
{
public:
  // The definition of the format is in terms of 32-bit words
  typedef uint32_t word_t;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_channels_per_frame = 4;
  static constexpr int s_adcs_per_channel = 64;
  static constexpr int s_daphnes_per_frame = 1;
  static constexpr int s_num_adc_words = s_channels_per_frame * s_adcs_per_channel * s_bits_per_adc / s_bits_per_word;

  struct Header
  {
    word_t channel_0 : 6, channel_1 : 6, channel_2 : 6, channel_3 : 6, tbd_0 : 8;
    word_t tbd_1 : 32;
  };
  static_assert(sizeof(Header) == 8);

  struct Trailer
  {
    word_t tbd : 32;
  };
  static_assert(sizeof(Trailer) == 4);

  detdataformats::DAQHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT (a false accusation from the linter that s_num_adc_words is a variable)
  Trailer trailer; 

  uint64_t get_timestamp() const
  {
    return daq_header.get_timestamp();
  }

  /// @brief Set the 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp)
  {
    daq_header.timestamp_1 = new_timestamp;
    daq_header.timestamp_2 = new_timestamp >> 32;
  }


  /// @brief Get the @p i ADC value of @p chn in the frame
  uint16_t get_adc(uint i_adc, uint i_channel) const;

  /// @brief Set the @p i ADC value of @p chn in the frame to @p val
  void set_adc(uint i, uint chn, uint16_t val);

  /// @brief Get the channel 0 from the DAPHNE Stream frame header 
  uint8_t get_channel0() const { return header.channel_0; }
  
  /// @brief Get the channel 1 from the DAPHNE Stream frame header
  uint8_t get_channel1() const { return header.channel_1; }

  /// @brief Get the channel 2 from the DAPHNE Stream frame header 
  uint8_t get_channel2() const { return header.channel_2; }
  
  /// @brief Get the channel 3 from the DAPHNE Stream frame header  
  uint8_t get_channel3() const { return header.channel_3; }
};
  static_assert(sizeof(DAPHNEStreamFrame) == sizeof(detdataformats::DAQHeader) +
		sizeof(DAPHNEStreamFrame::Header) +
		sizeof(DAPHNEStreamFrame::word_t) * DAPHNEStreamFrame::s_num_adc_words +
		sizeof(DAPHNEStreamFrame::Trailer));

  inline uint16_t DAPHNEStreamFrame::get_adc(uint i_adc, uint i_channel) const {
    return static_cast<uint16_t>(
				 dunedaq::fddetdataformats::get_adc_2d_as_1d<
				 word_t,
				 s_num_adc_words,
				 s_bits_per_adc,
				 s_adcs_per_channel,
				 s_channels_per_frame
				 >(
				   static_cast<int>(i_adc),
				   static_cast<int>(i_channel),
				   adc_words
				   )
				 );
  }

  inline void DAPHNEStreamFrame::set_adc(uint i_adc, uint i_channel, uint16_t val) {
    dunedaq::fddetdataformats::set_adc_2d_as_1d<
      word_t,
      s_num_adc_words,
      s_bits_per_adc,
      s_adcs_per_channel,
      s_channels_per_frame
      >(
	static_cast<int>(i_adc),
	static_cast<int>(i_channel),
	val,
	adc_words
	);
  }

  // NOLINTEND(build/unsigned)
  
} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNESTREAMFRAME_HPP_
