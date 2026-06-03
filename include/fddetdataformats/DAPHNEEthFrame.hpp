/**
 * @file DAPHNEEthFrame.hpp
 *
 * Contains declaration of DAPHNEEthFrame, a class for accessing raw WIB eth frames, as used in ProtoDUNE-SP-II
 *
 * The canonical definition of the DAPHNE format is given in EDMS document 2088726:
 * https://edms.cern.ch/document/2088726/XXX (XXX a stand-in for the doc version, e.g. 5)
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_

#include "fddetdataformats/FrameConcepts.hpp"
#include "fddetdataformats/Utils.hpp"

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

// NOLINTBEGIN(build/unsigned)

/**
 *  @brief Class for accessing raw WIB eth frames, as used in ProtoDUNE-II
 *
 *  The canonical definition of the WIB format is given in EDMS document 2088713:
 *  https://edms.cern.ch/document/2088726/XXX, (XXX a stand-in for the doc version, e.g. 5)
 */
class DAPHNEEthFrame
{
public:
  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t;

  // Dataframe format version
  static constexpr uint8_t version = 1;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 1024;
  static constexpr int s_num_adc_words = s_num_adcs * s_bits_per_adc / s_bits_per_word;

  struct Header
  {
    static constexpr size_t s_expected_bytes { 7 * sizeof(word_t) };
    
    // The following bitfields constitute what could be considered "word_t w0;"
    word_t trig_sample : 14;
    word_t rsv_0 : 2;
    word_t threshold : 14;
    word_t rsv_1 : 2;
    word_t baseline : 14;
    word_t rsv_2 : 6;
    word_t version : 4;
    word_t channel : 8;

    word_t w1;
    word_t w2;
    word_t w3;
    word_t w4;
    word_t w5;
    word_t w6;
  };
  static_assert(sizeof(Header) == Header::s_expected_bytes);

  static constexpr size_t s_expected_bytes = sizeof(detdataformats::DAQEthHeader) + Header::s_expected_bytes + s_num_adc_words * sizeof(word_t);

  
  const detdataformats::DAQEthHeader& get_daqheader() const {
    return m_daq_header;
  }

  const Header& get_header() const {
    return m_header;
  }
  
  /**
   * @brief Get the ith ADC value in the frame
   *
   * The ADC words are 14 bits long, stored packed in the data structure. The order is:
   *
   * - 1024 adc values from one daphne channel
   */
  uint16_t get_adc(int i) const; // NOLINT

  /// @brief Set the ith ADC value in the frame to @p val
  void set_adc(int i, uint16_t val); // NOLINT

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

  bool operator<(const DAPHNEEthFrame& other) const {
    return std::tuple(this->get_timestamp(), this->get_channel()) < std::tuple(other.get_timestamp(), other.get_channel());
  }
  
private:
  detdataformats::DAQEthHeader m_daq_header;
  Header m_header;
  word_t m_adc_words[s_num_adc_words]; // NOLINT
};

  static_assert(std::endian::native == std::endian::little,
              "The DAPHNEEthFrame bitfield layout assumes little-endian architecture");

  static_assert(AdaptableFrameConcept<DAPHNEEthFrame>, "DAPHNEEthFrame does not satisfy the AdaptableFrameConcept");

} // namespace dunedaq::fddetdataformats

#include "detail/DAPHNEEthFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
