/**
 * @file DAPHNEEthFrame.hpp
 *
 * Contains declaration of DAPHNEEthFrame, a struct for accessing raw DAPHNE eth frames
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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

// NOLINTBEGIN(build/unsigned)

/**
 *  @brief Struct for accessing raw DAPHNE eth frames
 *
 *  The canonical definition of the DAPHNE format is given in EDMS document 2088726:
 *  https://edms.cern.ch/document/2088726/XXX, (XXX a stand-in for the doc version, e.g. 5)
 */
struct DAPHNEEthFrame
{
  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t;

  // Dataframe format version
  static constexpr uint8_t version = 1;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 1024;
  static constexpr int s_num_adc_words = s_num_adcs * s_bits_per_adc / s_bits_per_word;
  static constexpr int s_max_peaks = 5;
  static constexpr int s_peak_descriptor_words = 12;
  static constexpr int s_packed_peak_descriptor_words = 6;

  /// @brief Single peak descriptor: two 32-bit words packed into one 64-bit word
  struct PeakDescriptor {
    static constexpr size_t s_expected_bytes { 1 * sizeof(word_t) };

    // Odd word (bits [31:0])
    word_t num_subpeaks : 4;          // Num_SubPeaks  [3:0]
    word_t reserved : 4;             // Reserved      [7:4]
    word_t adc_integral : 23;        // ADC_Integral  [30:8]
    word_t found : 1;                // Found         [31]

    // Even word (bits [63:32])
    word_t adc_max : 14;             // ADC_Max               [13:0]
    word_t sample_max : 9;           // Time_Peak             [22:14]
    word_t samples_over_baseline : 9; // Time_Over_Baseline    [31:23]
  };
  static_assert(sizeof(PeakDescriptor) == PeakDescriptor::s_expected_bytes);

  /// @brief Collection of peak descriptors and their associated time-start fields
  struct PeakDescriptorData {

    static constexpr size_t s_expected_bytes { (DAPHNEEthFrame::s_max_peaks + 1) * sizeof(word_t) };

    PeakDescriptor peaks[DAPHNEEthFrame::s_max_peaks];

    // Time_Start fields span two 32-bit words packed into one 64-bit word.
    // Word 11 (bits [31:0]): samples_start for indices 2, 1, 0
    word_t samples_start_2 : 10;     // Time_Start(2)  [11:2]
    word_t samples_start_1 : 10;     // Time_Start(1)  [21:12]
    word_t samples_start_0 : 10;     // Time_Start(0)  [31:22]
    word_t reserved_5 : 2;           // Reserved       [1:0]

    // Word 12 (bits [63:32]): samples_start for indices 4, 3
    word_t reserved_6 : 12;          // Reserved       [11:0]
    word_t samples_start_4 : 10;     // Time_Start(4)  [21:12]
    word_t samples_start_3 : 10;     // Time_Start(3)  [31:22]

    /// @brief Get the Num_SubPeaks value for peak @p ipdx
    inline uint8_t get_num_subpeaks(int ipdx) const
    { check_range_npeaks_(ipdx); return peaks[ipdx].num_subpeaks; }

    /// @brief Set the Num_SubPeaks value for peak @p ipdx
    inline void set_num_subpeaks(uint8_t val, int ipdx)
    { check_range_npeaks_(ipdx); peaks[ipdx].num_subpeaks = val; }

    /// @brief Get the Found flag for peak @p ipdx
    inline bool is_found(int ipdx) const
    { check_range_npeaks_(ipdx); return peaks[ipdx].found; }

    /// @brief Set the Found flag for peak @p ipdx
    inline void set_found(uint8_t val, int ipdx)
    { check_range_npeaks_(ipdx); peaks[ipdx].found = val; }

    /// @brief Get the ADC_Integral value for peak @p ipdx
    inline uint32_t get_adc_integral(int ipdx) const
    { check_range_npeaks_(ipdx); return peaks[ipdx].adc_integral; }

    /// @brief Set the ADC_Integral value for peak @p ipdx
    inline void set_adc_integral(uint32_t val, int ipdx)
    { check_range_npeaks_(ipdx); peaks[ipdx].adc_integral = val; }

    /// @brief Get the ADC_Max value for peak @p ipdx
    inline uint16_t get_adc_max(int ipdx) const
    { check_range_npeaks_(ipdx); return peaks[ipdx].adc_max; }

    /// @brief Set the ADC_Max value for peak @p ipdx
    inline void set_adc_max(uint16_t val, int ipdx)
    { check_range_npeaks_(ipdx); peaks[ipdx].adc_max = val; }

    /// @brief Get the Time_Peak value for peak @p ipdx
    inline uint16_t get_sample_max(int ipdx) const
    { check_range_npeaks_(ipdx); return peaks[ipdx].sample_max; }

    /// @brief Set the Time_Peak value for peak @p ipdx
    inline void set_sample_max(uint16_t val, int ipdx)
    { check_range_npeaks_(ipdx); peaks[ipdx].sample_max = val; }

    /// @brief Get the Time_Over_Baseline value for peak @p ipdx
    inline uint16_t get_samples_over_baseline(int ipdx) const
    { check_range_npeaks_(ipdx); return peaks[ipdx].samples_over_baseline; }

    /// @brief Set the Time_Over_Baseline value for peak @p ipdx
    inline void set_samples_over_baseline(uint16_t val, int ipdx)
    { check_range_npeaks_(ipdx); peaks[ipdx].samples_over_baseline = val; }

    /// @brief Get the Time_Start value for peak @p ipdx
    inline uint16_t get_sample_start(int ipdx) const;

    /// @brief Set the Time_Start value for peak @p ipdx
    inline void set_sample_start(uint16_t val, int ipdx);

    private:

    /// @brief Throw std::out_of_range if @p ipdx is not in [0, s_max_peaks)
    inline void check_range_npeaks_(int ipdx) const;
  };
  static_assert(sizeof(PeakDescriptorData) == PeakDescriptorData::s_expected_bytes);

  struct Header
  {
    static constexpr size_t s_expected_bytes { 7 * sizeof(word_t) };
    
    // The following bitfields constitute what could be considered "word_t w0;"
    word_t trigger_sample_value : 14;
    word_t rsv_0 : 2;
    word_t threshold : 14;
    word_t rsv_1 : 2;
    word_t baseline : 14;
    word_t rsv_2 : 6;
    word_t version : 4;
    word_t channel : 8;

    PeakDescriptorData peaks_data;

  };
  static_assert(sizeof(Header) == Header::s_expected_bytes);

  static constexpr size_t s_expected_bytes = sizeof(detdataformats::DAQEthHeader) + Header::s_expected_bytes + s_num_adc_words * sizeof(word_t);
  
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
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  /// @brief Set the starting 64-bit timestamp of the frame
  void set_timestamp(const uint64_t new_timestamp) { daq_header.timestamp = new_timestamp; }

  /// @brief Get the channel identifier of the frame
  uint8_t get_channel() const { return header.channel; }

  /// @brief Set the channel identifier of the frame
  void set_channel(const uint8_t new_channel) { header.channel = new_channel; }

  bool operator<(const DAPHNEEthFrame& other) const {
    return std::tuple(this->get_timestamp(), this->get_channel()) < std::tuple(other.get_timestamp(), other.get_channel());
  }
  
  /// @brief Get const reference to the peak descriptor data
  const PeakDescriptorData& get_peaks_data() const { return header.peaks_data; }

  /// @brief Get mutable reference to the peak descriptor data
  PeakDescriptorData& get_peaks_data() { return header.peaks_data; }

  detdataformats::DAQEthHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT
};

  static_assert(std::endian::native == std::endian::little,
              "The DAPHNEEthFrame bitfield layout assumes little-endian architecture");

  static_assert(AdaptableFrameConcept<DAPHNEEthFrame>, "DAPHNEEthFrame does not satisfy the AdaptableFrameConcept");



inline uint16_t
DAPHNEEthFrame::PeakDescriptorData::get_sample_start(int ipdx) const
{
  check_range_npeaks_(ipdx);

  if(ipdx==0)
    return samples_start_0;
  else if(ipdx==1)
    return samples_start_1;
  else if(ipdx==2)
    return samples_start_2;
  else if(ipdx==3)
    return samples_start_3;
  else //if(ipdx==4)
    return samples_start_4;

}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_sample_start(uint16_t val, int ipdx)
{
  check_range_npeaks_(ipdx);

  if(ipdx==0)
    samples_start_0=val & 0x3FFu;
  else if(ipdx==1)
    samples_start_1=val & 0x3FFu;
  else if(ipdx==2)
    samples_start_2=val & 0x3FFu;
  else if(ipdx==3)
    samples_start_3=val & 0x3FFu;
  else if(ipdx==4)
    samples_start_4=val & 0x3FFu;
  
  return;
}

} // namespace dunedaq::fddetdataformats

#include "detail/DAPHNEEthFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
