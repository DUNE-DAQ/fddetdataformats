/**
 * @file DAPHNEEthStreamFrame.hpp
 *
 * Contains declaration of DAPHNEEthStreamFrame, a class for accessing raw DAPHNE eth stream frames, as used in
 * ProtoDUNE-SP-II
 *
 * The canonical definition of the DAPHNE format is given in EDMS document 2088726:
 * https://edms.cern.ch/document/2088726/XXX (XXX a stand-in for the doc version, e.g. 5)
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_

#include "fddetdataformats/FrameConcepts.hpp"
#include "fddetdataformats/Utils.hpp"

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <format>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

// NOLINTBEGIN(build/unsigned)

/**
 *  @brief Class for accessing raw DAPHNE eth stream frames, as used in ProtoDUNE-II
 *
 * The canonical definition of the DAPHNE format is given in EDMS document 2088726:
 * https://edms.cern.ch/document/2088726/XXX (XXX a stand-in for the doc version, e.g. 5)
 *
 */

class DAPHNEEthStreamFrame
{
public:
  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t;

  // Dataframe format version
  static constexpr uint8_t version = 1;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_adcs_per_channel = 280;
  static constexpr int s_num_channels = 4;
  static constexpr int s_num_adc_words = s_num_channels * s_adcs_per_channel * s_bits_per_adc / s_bits_per_word;

  struct ChannelWord
  {
    word_t tbd : 52;
    word_t version : 4;
    word_t channel : 8;
  };
  static_assert(sizeof(ChannelWord) == 8);

  struct Header
  {
    ChannelWord channel_words[s_num_channels]; // NOLINT
  };
  static_assert(sizeof(Header) == sizeof(ChannelWord) * s_num_channels);

  static constexpr int s_expected_bytes = sizeof(detdataformats::DAQEthHeader) + sizeof(Header) +
    sizeof(word_t) * s_num_adc_words;

  const detdataformats::DAQEthHeader& get_daqheader() const {
    return m_daq_header;
  }

  const Header& get_header() const {
    return m_header;
  }

  /// @brief Get the @p i_adc-th ADC value of @p i_channel-th channel in the frame
  uint16_t get_adc(int i_adc, int i_channel) const;

  /// @brief Set the @p i_adc-th ADC value of @p i_channel-th channel in the frame to @p val
  void set_adc(int i_adc, int i_channel, uint16_t val); // NOLINT

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return m_daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { m_daq_header.timestamp = new_timestamp; }

  /// @brief Get the channel identifier of the frame
  uint8_t get_channel(const int i_channel) const;

  /// @brief Set the channel identifier of the frame
  void set_channel(const int i_channel, const uint8_t new_channel_val);

  /// @brief Get the channel 0 from the DAPHNE Stream frame header
  uint8_t get_channel0() const { return m_header.channel_words[0].channel; }

  /// @brief Get the channel 1 from the DAPHNE Stream frame header
  uint8_t get_channel1() const { return m_header.channel_words[1].channel; }

  /// @brief Get the channel 2 from the DAPHNE Stream frame header
  uint8_t get_channel2() const { return m_header.channel_words[2].channel; }

  /// @brief Get the channel 3 from the DAPHNE Stream frame header
  uint8_t get_channel3() const { return m_header.channel_words[3].channel; }

  void set_geoid(uint16_t crate_id, uint16_t slot_id, uint16_t stream_id) {
    fddetdataformats::set_geoid(crate_id, slot_id, stream_id, m_daq_header);
  }

  bool operator<(const DAPHNEEthStreamFrame& other) const {
    return this->get_timestamp() < other.get_timestamp();
  }

private:
  detdataformats::DAQEthHeader m_daq_header;
  Header m_header;
  word_t m_adc_words[s_num_adc_words]; // NOLINT
};

static_assert(std::endian::native == std::endian::little,
              "The DAPHNEEthStreamFrame bitfield layout assumes little-endian architecture");

  static_assert(AdaptableFrameConcept<DAPHNEEthStreamFrame>, "DAPHNEEthStreamFrame does not satisfy the AdaptableFrameConcept");
  
} // namespace dunedaq::fddetdataformats

#include "detail/DAPHNEEthStreamFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_
