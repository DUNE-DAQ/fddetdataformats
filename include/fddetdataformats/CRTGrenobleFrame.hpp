/**
 * @file CRTGrenobleFrame.hpp
 *
 * Contains declaration of CRTGrenobleFrame, a class for accessing/holding raw CRT data from the 'Grenoble' panels
 * ProtoDUNE-II VD
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTGRENOBLEFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTGRENOBLEFRAME_HPP_

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

// NOLINTBEGIN(build/unsigned)

///  @brief Class for accessing/holding raw CRT data from the 'Grenoble' panels ProtoDUNE-II VD
class CRTGrenobleFrame
{
public:
  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t;

  static constexpr int s_num_channels = 32;
  static constexpr uint64_t s_DTS_ticks_per_second = 62'500'000;
  static constexpr uint64_t s_ns_per_DTS_tick = 16;

  struct TGpsDateStruct
  {
    unsigned int seconds : 8;
    unsigned int minutes : 8;
    unsigned int hours : 8;
    unsigned int year : 8;

    unsigned int day : 16;
    unsigned int new_date_cnt : 12;
    unsigned int irigb_dec_ver : 3;
    unsigned int irigb_valid : 1;
  };
  static_assert(sizeof(TGpsDateStruct) == 8);

  struct STChannel
  {
    int qTot = 0;            ///< Total charge.
    unsigned short n_zc = 0; ///< CFD time.
    float cfd = 0.;          ///< CFD value
    unsigned short flag = 0; ///< Flag containing trigger, trigger sum and overflow information.
  };

#warning "CRTGrenobleFrame::STChannel has padding inserted"
  //      static_assert(sizeof(STChannel) == sizeof(int) + sizeof(unsigned short) + sizeof(float) + sizeof(unsigned
  //      short));

  struct STEvent
  {
    unsigned int eventID = 0;      ///< Event ID.
    unsigned int dateInSec = 0;    ///< Event date in seconds.
    unsigned int timestamp = 0;    ///< Timestamp (4 ns) -> used to compute dt between events.
    TGpsDateStruct gpsDate;        ///< TGPS date
    unsigned int pps_interval = 0; ///< IRIG-B subdivision in a second, expressed in 100 ns clock ticks.
    unsigned int FIFO_AF_duration =
      0; ///< FIFO AF duration (4 ns) -> integration of Almost full fifo since last accepted trigger

    struct STChannel channels[s_num_channels];
  };

#warning "CRTGrenobleFrame::STEvent has padding inserted"
  // static_assert(sizeof(STEvent) == 3 * sizeof(unsigned int) + sizeof(TGpsDateStruct) + 2 * sizeof(unsigned int) +
  // sizeof(STChannel) * s_num_channels);

  detdataformats::DAQEthHeader daq_header;
  STEvent event;

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

    event.channels[i_ch].qTot = val;
  }

  /// @brief Get the starting 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { daq_header.timestamp = new_timestamp; }

}; // CRTGrenobleFrame
#warning "CRTGrenobleFrame has padding inserted"
// static_assert(sizeof(CRTGrenobleFrame) == sizeof(detdataformats::DAQEthHeader) + sizeof(CRTGrenobleFrame::STEvent));

static_assert(std::endian::native == std::endian::little,
              "The CRTGrenobleFrame bitfield layout assumes little-endian architecture");
static_assert(std::is_trivially_copyable_v<CRTGrenobleFrame>,
              "CRTGrenobleFrame isn't trivially copyable and can't be safely std::memcpy'd");
static_assert(std::is_standard_layout_v<CRTGrenobleFrame>,
              "CRTGrenobleFrame isn't standard layout; reinterpret_cast and offsetof can't safely be used with it");

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTGRENOBLEFRAME_HPP_
