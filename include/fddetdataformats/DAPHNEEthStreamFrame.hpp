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

#include "Utils.hpp"

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

  detdataformats::DAQEthHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT

  /// @brief Get the @p i_adc-th ADC value of @p i_channel-th channel in the frame
  uint16_t get_adc(uint i_adc, uint i_channel) const;

  /// @brief Set the @p i_adc-th ADC value of @p i_channel-th channel in the frame to @p val

  // WARNING: ORDER OF CHANNEL AND ADC IS REVERSED RELATIVE TO get_adc
  void set_adc(uint i_channel, uint i_adc, uint16_t val); // NOLINT

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { daq_header.timestamp = new_timestamp; }

  /// @brief Get the channel identifier of the frame
  uint8_t get_channel(const uint i_channel) const;

  /// @brief Set the channel identifier of the frame
  void set_channel(const uint i_channel, const uint8_t new_channel_val);

  /// @brief Get the channel 0 from the DAPHNE Stream frame header
  uint8_t get_channel0() const { return header.channel_words[0].channel; }

  /// @brief Get the channel 1 from the DAPHNE Stream frame header
  uint8_t get_channel1() const { return header.channel_words[1].channel; }

  /// @brief Get the channel 2 from the DAPHNE Stream frame header
  uint8_t get_channel2() const { return header.channel_words[2].channel; }

  /// @brief Get the channel 3 from the DAPHNE Stream frame header
  uint8_t get_channel3() const { return header.channel_words[3].channel; }
};
static_assert(sizeof(DAPHNEEthStreamFrame) ==
              sizeof(detdataformats::DAQEthHeader) + sizeof(DAPHNEEthStreamFrame::Header) +
                sizeof(DAPHNEEthStreamFrame::word_t) * DAPHNEEthStreamFrame::s_num_adc_words);

static_assert(std::endian::native == std::endian::little,
              "The DAPHNEEthStreamFrame bitfield layout assumes little-endian architecture");

static_assert(std::is_trivially_copyable_v<DAPHNEEthStreamFrame>,
              "DAPHNEEthStreamFrame isn't trivially copyable and can't be safely std::memcpy'd");
static_assert(std::is_standard_layout_v<DAPHNEEthStreamFrame>,
              "DAPHNEEthStreamFrame isn't standard layout; reinterpret_cast and offsetof can't safely be used with it");

} // namespace dunedaq::fddetdataformats

#include "detail/DAPHNEEthStreamFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHSTREAMFRAME_HPP_
