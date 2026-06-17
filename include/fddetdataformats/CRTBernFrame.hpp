/**
 * @file CRTBernFrame.hpp
 *
 * Contains declaration of CRTBernFrame, a struct for accessing/holding raw CRT data from the 'Bern' panels ProtoDUNE-II
 * VD
 *
 * n.b. CRTBernFrame does *not* satisfy the AdaptableFrameConcept; its
 * size exceeds the sum of its members (i.e., the compiler inserts
 * padding)
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTBERNFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTBERNFRAME_HPP_

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
 *  @brief Struct for accessing/holding raw CRT data from the 'Bern' panels ProtoDUNE-II VD
 *
 */
struct CRTBernFrame
{
  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t; // NOLINT

  static constexpr int s_num_channels = 32;
  static constexpr uint64_t s_DTS_ticks_per_second = 62'500'000;
  static constexpr uint64_t s_ns_per_DTS_tick = 16;

  struct CRTBernData
  {
    static constexpr std::size_t s_expected_bytes { 2 + 2 + 2 + 4 + 4 + 2 * s_num_channels + 4 };

    uint16_t flags = 0;
    uint16_t lostcpu = 0;
    uint16_t lostfpga = 0;
    uint32_t ts0 = 0;
    uint32_t ts1 = 0;
    uint16_t adc[s_num_channels] = { 0 }; // NOLINT
    uint32_t coinc = 0;
  };
  #warning "CRTBernData has padding inserted"
  //static_assert(sizeof(CRTBernData) == CRTBernData::s_expected_bytes);

  static constexpr std::size_t s_expected_bytes { sizeof(detdataformats::DAQEthHeader) + sizeof(uint16_t) +
    CRTBernData::s_expected_bytes};
  
  /// @brief Get the adc value for channel i_ch
  uint16_t get_adc(int i_ch) const
  {
    if (i_ch < 0 || i_ch >= s_num_channels)
      throw std::out_of_range("ADC channel index out of range");

    return data.adc[i_ch];
  }

  /// @brief Set the adc value for channel i_ch to @p val
  void set_adc(int i_ch, uint16_t val)
  {
    if (i_ch < 0 || i_ch >= s_num_channels)
      throw std::out_of_range("ADC channel index out of range");

    data.adc[i_ch] = val; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  /** @brief Set the starting 64-bit timestamp of the frame
   *  also set the underlying ts0 to be consistent
   */
  void set_timestamp(const uint64_t new_timestamp)
  {
    daq_header.timestamp = new_timestamp;
    data.ts0 = (new_timestamp % s_DTS_ticks_per_second) * s_ns_per_DTS_tick;
  }

  uint16_t get_mac5() const { return mac5; }

  void set_mac5(const uint16_t new_mac5) { mac5 = new_mac5; }

  uint16_t get_flags() const { return data.flags; }

  void set_flags(const uint16_t new_flags) { data.flags = new_flags; }

  /// @brief Get the lostcpu counter of the CRTBernData
  uint16_t get_lostcpu() const { return data.lostcpu; }

  /// @brief Set the lostcpu counter of the CRTBernData
  void set_lostcpu(const uint16_t new_lostcpu) { data.lostcpu = new_lostcpu; }

  /// @brief Get the lostfpga counter of the CRTBernData
  uint16_t get_lostfpga() const { return data.lostfpga; }

  /// @brief Set the lostfpga counter of the CRTBernData
  void set_lostfpga(const uint16_t new_lostfpga) { data.lostfpga = new_lostfpga; }

  uint32_t get_ts0() const { return data.ts0; }

  void set_ts0(const uint32_t new_ts0) { data.ts0 = new_ts0; }

  uint32_t get_ts1() const { return data.ts1; }

  void set_ts1(const uint32_t new_ts1) { data.ts1 = new_ts1; }

  uint32_t get_coinc() const { return data.coinc; }

  void set_coinc(const uint32_t new_coinc) { data.coinc = new_coinc; }

  detdataformats::DAQEthHeader daq_header; // Note this is de-facto public thanks to non-const get_daqheader
  uint16_t mac5;
  CRTBernData data;

}; // CRTBernFrame
  #warning "CRTBernFrame has padding inserted"
  // static_assert(sizeof(CRTBernFrame) == CRTBernFrame::s_expected_bytes)

  static_assert(std::endian::native == std::endian::little,
		"The CRTBernFrame bitfield layout assumes little-endian architecture");
  static_assert(std::is_trivially_copyable_v<CRTBernFrame>,
		"CRTBernFrame isn't trivially copyable and can't be safely std::memcpy'd");
  static_assert(std::is_standard_layout_v<CRTBernFrame>,
		"CRTBernFrame isn't standard layout; reinterpret_cast and offsetof can't safely be used with it");
  
} // namespace dunedaq::fddetdataformats

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTBERNFRAME_HPP_
