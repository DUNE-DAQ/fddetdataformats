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

#include "fddetdataformats/FrameConcepts.hpp"
#include "fddetdataformats/Utils.hpp"

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <limits>
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
  using word_t = uint64_t;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_time_samples_per_frame = 64;
  static constexpr int s_channels_per_half_femb = 64;
  static constexpr int s_half_fembs_per_frame = 1;
  static constexpr int s_num_channels = s_channels_per_half_femb * s_half_fembs_per_frame;
  static constexpr int s_num_adc_words_per_ts = s_num_channels * s_bits_per_adc / s_bits_per_word;

  struct TDEEthHeader
  {
    static constexpr size_t s_expected_bytes { 8 + 8 }; // bitfields + TAItime

    uint64_t reserved : 26;
    uint64_t tde_errors : 16;
    uint64_t tde_header : 10;
    uint64_t version : 4;
    uint64_t channel : 8;
    uint64_t TAItime { std::numeric_limits<uint64_t>::max() };
  };
  static_assert(sizeof(TDEEthHeader) == TDEEthHeader::s_expected_bytes);

  static constexpr size_t s_expected_bytes = sizeof(detdataformats::DAQEthHeader) + TDEEthHeader::s_expected_bytes + s_time_samples_per_frame * s_num_adc_words_per_ts * sizeof(word_t);

  
  const detdataformats::DAQEthHeader& get_daqheader() const {
    return m_daq_header;
  }

  const TDEEthHeader& get_header() const {
    return m_header;
  }
  
  /**
   * @brief Get the i_channel-th ADC value in the i_sample-th time sample
   *
   * The order is: 64 channels repeated for 64 time samples
   * The ADC words are 14 bits long
   *
   */
  uint16_t get_adc(int i_channel, int i_sample = 0) const;

  /// @brief Set the i_channel-th ADC value in the i_sample-th time sample to @p val
  void set_adc(int i_channel, int i_sample, uint16_t val);

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return m_daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { m_daq_header.timestamp = new_timestamp; }

  /// @brief Get the channel identifier of the frame
  uint8_t get_channel() const { return m_header.channel; }

  /// @brief Set the channel identifier of the frame
  void set_channel(const uint8_t new_channel) { m_header.channel = new_channel; }

  void set_geoid(uint16_t crate_id, uint16_t slot_id, uint16_t stream_id) {
    dunedaq::fddetdataformats::set_geoid(crate_id, slot_id, stream_id, m_daq_header);
  }
  
  const word_t* get_adc_words() const {
    return &m_adc_words[0][0];
  }

  bool operator<(const TDEEthFrame& other) const {
    return this->get_timestamp() < other.get_timestamp();
  }
  
private:  
  detdataformats::DAQEthHeader m_daq_header;
  TDEEthHeader m_header;
  word_t m_adc_words[s_time_samples_per_frame][s_num_adc_words_per_ts]; // NOLINT
};

  static_assert(std::endian::native == std::endian::little,
              "The TDEEthFrame bitfield layout assumes little-endian architecture");

  static_assert(AdaptableFrameConcept<TDEEthFrame>, "TDEEthFrame does not satisfy the AdaptableFrameConcept");

} // namespace dunedaq::fddetdataformats

#include "detail/TDEEthFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_TDEETHFRAME_HPP_
