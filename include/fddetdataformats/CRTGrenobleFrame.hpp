/**
 * @file CRTGrenobleFrame.hpp
 *
 * Contains declaration of CRTGrenobleFrame, a struct for accessing/holding raw CRT data from the 'Grenoble' panels
 * ProtoDUNE-II VD
 *
 * n.b. CRTGrenobleFrame does *not* satisfy the AdaptableFrameConcept; its
 * size exceeds the sum of its members (i.e., the compiler inserts
 * padding)
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTGRENOBLEFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTGRENOBLEFRAME_HPP_

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

///  @brief Struct for accessing/holding raw CRT data from the 'Grenoble' panels ProtoDUNE-II VD
struct CRTGrenobleFrame
{
  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t;

  static constexpr int s_num_channels = 32;
  static constexpr uint64_t s_DTS_ticks_per_second = 62'500'000;
  static constexpr uint64_t s_ns_per_DTS_tick = 16;

  struct TGpsDateStruct
  {
    // "/ 8" below -> 8 bits to a byte
    static constexpr int s_expected_size { (8 + 8 + 8 + 8 + 16 + 12 + 3 + 1) / 8 }; 

    uint32_t seconds : 8;
    uint32_t minutes : 8;
    uint32_t hours : 8;
    uint32_t year : 8;

    uint32_t day : 16;
    uint32_t new_date_cnt : 12;
    uint32_t irigb_dec_ver : 3;
    uint32_t irigb_valid : 1;
  };
  static_assert(sizeof(TGpsDateStruct) == TGpsDateStruct::s_expected_size);

  struct STChannel
  {
    static constexpr int s_expected_size { sizeof(int) + sizeof(uint16_t) + sizeof(float) + sizeof(uint16_t) }; // NOLINT(runtime/int,google-runtime-int)

    int qTot = 0;            ///< Total charge.
    uint16_t n_zc = 0; ///< CFD time.
    float cfd = 0.;          ///< CFD value
    uint16_t flag = 0; ///< Flag containing trigger, trigger sum and overflow information.
  };

#warning "CRTGrenobleFrame::STChannel has padding inserted"
  //      static_assert(sizeof(STChannel) == STChannel::s_expected_size);

  struct STEvent
  {
    static constexpr int s_expected_size { 3 * sizeof(uint32_t) + TGpsDateStruct::s_expected_size + 2 * sizeof(uint32_t) + STChannel::s_expected_size * s_num_channels };
    
    uint32_t eventID = 0;      ///< Event ID.
    uint32_t dateInSec = 0;    ///< Event date in seconds.
    uint32_t timestamp = 0;    ///< Timestamp (4 ns) -> used to compute dt between events.
    TGpsDateStruct gpsDate;        ///< TGPS date
    uint32_t pps_interval = 0; ///< IRIG-B subdivision in a second, expressed in 100 ns clock ticks.
    uint32_t FIFO_AF_duration =
      0; ///< FIFO AF duration (4 ns) -> integration of Almost full fifo since last accepted trigger

    struct STChannel channels[s_num_channels];
  };
#warning "CRTGrenobleFrame::STEvent has padding inserted"
  // static_assert(sizeof(STEvent) == STEvent::s_expected_size);

  static constexpr int s_expected_bytes { sizeof(detdataformats::DAQEthHeader) + CRTGrenobleFrame::STEvent::s_expected_size };
  
  /// @brief Get the adc value for channel i_ch
  int get_adc(const int i_ch) const
  {
    if (i_ch < 0 || i_ch >= s_num_channels)
      throw std::out_of_range("ADC channel index out of range");

    return event.channels[i_ch].qTot;
  }

  /// @brief Set the adc value for channel i_ch to @p val
  void set_adc(const int i_ch, const int val)
  {
    if (i_ch < 0 || i_ch >= s_num_channels)
      throw std::out_of_range("ADC channel index out of range");

    event.channels[i_ch].qTot = val; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { daq_header.timestamp = new_timestamp; }

  detdataformats::DAQEthHeader daq_header; // Formally private, but has a non-const accessor
  STEvent event;
  
}; // CRTGrenobleFrame
#warning "CRTGrenobleFrame has padding inserted"
// static_assert(sizeof(CRTGrenobleFrame) == CRTGrenobleFrame::s_expected_size);

static_assert(std::endian::native == std::endian::little,
              "The CRTGrenobleFrame bitfield layout assumes little-endian architecture");
static_assert(std::is_trivially_copyable_v<CRTGrenobleFrame>,
              "CRTGrenobleFrame isn't trivially copyable and can't be safely std::memcpy'd");
static_assert(std::is_standard_layout_v<CRTGrenobleFrame>,
              "CRTGrenobleFrame isn't standard layout; reinterpret_cast and offsetof can't safely be used with it");

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTGRENOBLEFRAME_HPP_
