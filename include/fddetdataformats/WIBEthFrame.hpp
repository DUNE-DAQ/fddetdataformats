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

#include "fddetdataformats/Utils.hpp"

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
class WIBEthFrame
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

  struct WIBEthHeader
  {
    word_t colddata_timestamp_0 : 15;
    word_t pad_0 : 1;
    word_t colddata_timestamp_1 : 15;
    word_t pad_1 : 1;
    word_t cd : 1;
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
  static_assert(sizeof(WIBEthHeader) == 8 + 8); // 8 bytes for the bitfield section, 8 for the extra_data

  const detdataformats::DAQEthHeader& get_daqheader() const {
    return daq_header;
  }

  const WIBEthHeader& get_header() const {
    return header;
  }

  /**
   * @brief Get the i_channel-th ADC value in the i_sample-th time sample
   *
   * The order is: 64 channels repeated for 64 time samples
   * The ADC words are 14 bits long
   *
   */
  uint16_t get_adc(int i_channel, int i_sample = 0) const;

  /// @brief Set the i_channel-th ADC value in the i_sample-th time sample to @p adc_val
  void set_adc(int i_channel, int i_sample, uint16_t adc_val);

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { daq_header.timestamp = new_timestamp; }

  /// @brief Get the channel identifier of the frame
  uint8_t get_channel() const { return header.channel; }

  /// @brief Set the channel identifier of the frame
  void set_channel(const uint8_t new_channel) { header.channel = new_channel; }

  void set_geoid(uint16_t crate_id, uint16_t slot_id, uint16_t stream_id) {
    dunedaq::fddetdataformats::set_geoid(crate_id, slot_id, stream_id, daq_header);
  }

  const word_t* get_adc_words() const {
    return &adc_words[0][0];
  }

private:
  detdataformats::DAQEthHeader daq_header;
  WIBEthHeader header;
  word_t adc_words[s_time_samples_per_frame][s_num_adc_words_per_ts]; // NOLINT

};
static_assert(sizeof(WIBEthFrame) == sizeof(detdataformats::DAQEthHeader) + sizeof(WIBEthFrame::WIBEthHeader) +
                                       sizeof(WIBEthFrame::word_t) * WIBEthFrame::s_time_samples_per_frame *
                                         WIBEthFrame::s_num_adc_words_per_ts);

static_assert(std::endian::native == std::endian::little,
              "The WIBEthFrame bitfield layout assumes little-endian architecture");

static_assert(std::is_trivially_copyable_v<WIBEthFrame>,
              "WIBEthFrame isn't trivially copyable and can't be safely std::memcpy'd");
static_assert(std::is_standard_layout_v<WIBEthFrame>,
              "WIBEthFrame isn't standard layout; reinterpret_cast and offsetof can't safely be used with it");

} // namespace dunedaq::fddetdataformats

#include "detail/WIBEthFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_WIBETHFRAME_HPP_
