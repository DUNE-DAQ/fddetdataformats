/**
 * @file DAPHNEEthFrame.hpp
 *
 * Contains declaration of DAPHNEEthFrame, a class for accessing raw DAPHNE
 * Ethernet frames.
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

#include "Utils.hpp"

#include "detdataformats/DAQEthHeader.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace dunedaq::fddetdataformats {

// NOLINTBEGIN(build/unsigned)

/**
 *  @brief Class for accessing raw DAPHNE Ethernet frames.
 *
 *  The on-wire frame uses one 64-bit header word containing trigger metadata,
 *  followed by six 64-bit words that carry twelve 32-bit peak descriptor words,
 *  and finally 1024 packed ADC samples for a single channel.
 *
 *  The canonical definition of the WIB format is given in EDMS document 2088713:
 *  https://edms.cern.ch/document/2088726/XXX, (XXX a stand-in for the doc version, e.g. 5)
 */
class DAPHNEEthFrame
{
public:

  // The definition of the format is in terms of 64-bit words
  using word_t = uint64_t;
  using descriptor_word_t = uint32_t;

  // Dataframe format version
  static constexpr uint8_t version = 1;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 1024;
  static constexpr int s_num_adc_words = s_num_adcs * s_bits_per_adc / s_bits_per_word;
  static constexpr int s_peak_descriptor_words = 12;
  static constexpr int s_packed_peak_descriptor_words = 6;

  struct PeakDescriptorData
  {
    // Word 1: peak 0 odd
    descriptor_word_t num_subpeaks_0 : 4;
    descriptor_word_t reserved_0 : 4;
    descriptor_word_t adc_integral_0 : 23;
    descriptor_word_t found_0 : 1;

    // Word 2: peak 0 even
    descriptor_word_t adc_max_0 : 14;
    descriptor_word_t sample_max_0 : 9;
    descriptor_word_t samples_over_baseline_0 : 9;

    // Word 3: peak 1 odd
    descriptor_word_t num_subpeaks_1 : 4;
    descriptor_word_t reserved_1 : 4;
    descriptor_word_t adc_integral_1 : 23;
    descriptor_word_t found_1 : 1;

    // Word 4: peak 1 even
    descriptor_word_t adc_max_1 : 14;
    descriptor_word_t sample_max_1 : 9;
    descriptor_word_t samples_over_baseline_1 : 9;

    // Word 5: peak 2 odd
    descriptor_word_t num_subpeaks_2 : 4;
    descriptor_word_t reserved_2 : 4;
    descriptor_word_t adc_integral_2 : 23;
    descriptor_word_t found_2 : 1;

    // Word 6: peak 2 even
    descriptor_word_t adc_max_2 : 14;
    descriptor_word_t sample_max_2 : 9;
    descriptor_word_t samples_over_baseline_2 : 9;

    // Word 7: peak 3 odd
    descriptor_word_t num_subpeaks_3 : 4;
    descriptor_word_t reserved_3 : 4;
    descriptor_word_t adc_integral_3 : 23;
    descriptor_word_t found_3 : 1;

    // Word 8: peak 3 even
    descriptor_word_t adc_max_3 : 14;
    descriptor_word_t sample_max_3 : 9;
    descriptor_word_t samples_over_baseline_3 : 9;

    // Word 9: peak 4 odd
    descriptor_word_t num_subpeaks_4 : 4;
    descriptor_word_t reserved_4 : 4;
    descriptor_word_t adc_integral_4 : 23;
    descriptor_word_t found_4 : 1;

    // Word 10: peak 4 even
    descriptor_word_t adc_max_4 : 14;
    descriptor_word_t sample_max_4 : 9;
    descriptor_word_t samples_over_baseline_4 : 9;

    // Word 11: Time_Start fields for indices 0,1,2 and reserved bits [1:0]
    descriptor_word_t samples_start_2 : 10;
    descriptor_word_t samples_start_1 : 10;
    descriptor_word_t samples_start_0 : 10;
    descriptor_word_t reserved_5 : 2;

    // Word 12: Time_Start fields for indices 3,4 and reserved bits [11:0]
    descriptor_word_t reserved_6 : 12;
    descriptor_word_t samples_start_4 : 10;
    descriptor_word_t samples_start_3 : 10;

    static constexpr uint8_t max_peaks = 5;

    inline bool is_found(int idx) const;
    inline void set_found(uint8_t val, int idx);

    inline uint32_t get_adc_integral(int idx) const;
    inline void set_adc_integral(uint32_t val, int idx);

    inline uint8_t get_num_subpeaks(int idx) const;
    inline void set_num_subpeaks(uint8_t val, int idx);

    inline uint16_t get_samples_over_baseline(int idx) const;
    inline void set_samples_over_baseline(uint16_t val, int idx);

    inline uint16_t get_sample_max(int idx) const;
    inline void set_sample_max(uint16_t val, int idx);

    inline uint16_t get_adc_max(int idx) const;
    inline void set_adc_max(uint16_t val, int idx);

    inline uint16_t get_sample_start(int idx) const;
    inline void set_sample_start(uint16_t val, int idx);

    inline const descriptor_word_t* as_words() const
    {
      return reinterpret_cast<const descriptor_word_t*>(this);
    }

    inline descriptor_word_t* as_words()
    {
      return reinterpret_cast<descriptor_word_t*>(this);
    }
  };

  struct Header
  {
    word_t trig_sample : 14;
    word_t rsv_0 : 2;
    word_t threshold : 14;
    word_t rsv_1 : 2;
    word_t baseline : 14;
    word_t rsv_2 : 6;
    word_t version : 4;
    word_t channel : 8;

    PeakDescriptorData peaks_data;

    descriptor_word_t get_baseline() const
    {
      return static_cast<descriptor_word_t>(baseline);
    }

    word_t get_packed_peak_word(int idx) const
    {
      if (idx < 0 || idx >= s_packed_peak_descriptor_words) {
        throw std::out_of_range("Packed peak word index out of range (must be 0-5)");
      }
      word_t value = 0;
      std::memcpy(&value,
                  reinterpret_cast<const uint8_t*>(&peaks_data) + idx * sizeof(word_t),
                  sizeof(value));
      return value;
    }

    void set_packed_peak_word(word_t value, int idx)
    {
      if (idx < 0 || idx >= s_packed_peak_descriptor_words) {
        throw std::out_of_range("Packed peak word index out of range (must be 0-5)");
      }
      std::memcpy(reinterpret_cast<uint8_t*>(&peaks_data) + idx * sizeof(word_t),
                  &value,
                  sizeof(value));
    }
  };
  static_assert(sizeof(Header) == 7 * sizeof(word_t));

  detdataformats::DAQEthHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT

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

  const PeakDescriptorData& get_peaks_data() const
  {
    return header.peaks_data;
  }

  PeakDescriptorData& get_peaks_data()
  {
    return header.peaks_data;
  }

};
static_assert(sizeof(DAPHNEEthFrame) == sizeof(detdataformats::DAQEthHeader) + sizeof(DAPHNEEthFrame::Header) +
                                          sizeof(DAPHNEEthFrame::word_t) * DAPHNEEthFrame::s_num_adc_words);

static_assert(std::endian::native == std::endian::little,
              "The DAPHNEEthFrame bitfield layout assumes little-endian architecture");

static_assert(std::is_trivially_copyable_v<DAPHNEEthFrame>,
              "DAPHNEEthFrame isn't trivially copyable and can't be safely std::memcpy'd");
static_assert(std::is_standard_layout_v<DAPHNEEthFrame>,
              "DAPHNEEthFrame isn't standard layout; reinterpret_cast and offsetof can't safely be used with it");

static_assert(sizeof(DAPHNEEthFrame::PeakDescriptorData) ==
                DAPHNEEthFrame::s_peak_descriptor_words * sizeof(DAPHNEEthFrame::descriptor_word_t),
              "Unexpected DAPHNEEthFrame::PeakDescriptorData size");
static_assert(sizeof(DAPHNEEthFrame::Header) == 7 * sizeof(DAPHNEEthFrame::word_t),
              "Unexpected DAPHNEEthFrame::Header size");

inline bool
DAPHNEEthFrame::PeakDescriptorData::is_found(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  const descriptor_word_t* tw = as_words();
  return static_cast<uint8_t>((tw[2 * idx] >> 31) & 0x1);
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_found(uint8_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  if (val > 1) {
    throw std::out_of_range("Found value out of range (must be 0-1)");
  }
  descriptor_word_t* tw = as_words();
  tw[2 * idx] = (tw[2 * idx] & ~(descriptor_word_t(1u) << 31)) | ((val & 0x1u) << 31);
}

inline uint32_t
DAPHNEEthFrame::PeakDescriptorData::get_adc_integral(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  const descriptor_word_t* tw = as_words();
  return (tw[2 * idx] >> 8) & 0x7FFFFF;
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_adc_integral(uint32_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  if (val > 0x7FFFFF) {
    throw std::out_of_range("ADC integral value out of range (must be 0-8388607)");
  }
  descriptor_word_t* tw = as_words();
  tw[2 * idx] = (tw[2 * idx] & ~(descriptor_word_t(0x7FFFFFu) << 8)) | ((val & 0x7FFFFFu) << 8);
}

inline uint8_t
DAPHNEEthFrame::PeakDescriptorData::get_num_subpeaks(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  const descriptor_word_t* tw = as_words();
  return static_cast<uint8_t>(tw[2 * idx] & 0xF);
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_num_subpeaks(uint8_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  if (val > 0xF) {
    throw std::out_of_range("Num_SubPeaks value out of range (must be 0-15)");
  }
  descriptor_word_t* tw = as_words();
  tw[2 * idx] = (tw[2 * idx] & ~descriptor_word_t(0xFu)) | (val & 0xFu);
}

inline uint16_t
DAPHNEEthFrame::PeakDescriptorData::get_samples_over_baseline(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  const descriptor_word_t* tw = as_words();
  return static_cast<uint16_t>((tw[2 * idx + 1] >> 23) & 0x1FF);
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_samples_over_baseline(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  if (val > 0x1FF) {
    throw std::out_of_range("Time_Over_Baseline value out of range (must be 0-511)");
  }
  descriptor_word_t* tw = as_words();
  tw[2 * idx + 1] = (tw[2 * idx + 1] & ~(descriptor_word_t(0x1FFu) << 23)) | ((val & 0x1FFu) << 23);
}

inline uint16_t
DAPHNEEthFrame::PeakDescriptorData::get_sample_max(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  const descriptor_word_t* tw = as_words();
  return static_cast<uint16_t>((tw[2 * idx + 1] >> 14) & 0x1FF);
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_sample_max(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  if (val > 0x1FF) {
    throw std::out_of_range("Time_Peak value out of range (must be 0-511)");
  }
  descriptor_word_t* tw = as_words();
  tw[2 * idx + 1] = (tw[2 * idx + 1] & ~(descriptor_word_t(0x1FFu) << 14)) | ((val & 0x1FFu) << 14);
}

inline uint16_t
DAPHNEEthFrame::PeakDescriptorData::get_adc_max(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  const descriptor_word_t* tw = as_words();
  return static_cast<uint16_t>(tw[2 * idx + 1] & 0x3FFF);
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_adc_max(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Peak index out of range (must be 0-4)");
  }
  if (val > 0x3FFF) {
    throw std::out_of_range("ADC Max value out of range (must be 0-16383)");
  }
  descriptor_word_t* tw = as_words();
  tw[2 * idx + 1] = (tw[2 * idx + 1] & ~descriptor_word_t(0x3FFFu)) | (val & 0x3FFFu);
}

inline uint16_t
DAPHNEEthFrame::PeakDescriptorData::get_sample_start(int idx) const
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Time_Start index out of range (must be 0-4)");
  }

  const descriptor_word_t* tw = as_words();
  if (idx < 3) {
    int shift = 22 - 10 * idx;
    return static_cast<uint16_t>((tw[10] >> shift) & 0x3FF);
  }

  int shift = 22 - 10 * (idx - 3);
  return static_cast<uint16_t>((tw[11] >> shift) & 0x3FF);
}

inline void
DAPHNEEthFrame::PeakDescriptorData::set_sample_start(uint16_t val, int idx)
{
  if (idx < 0 || idx > 4) {
    throw std::out_of_range("Time_Start index out of range (must be 0-4)");
  }
  if (val > 0x3FF) {
    throw std::out_of_range("Time_Start value out of range (must be 0-1023)");
  }

  descriptor_word_t* tw = as_words();
  descriptor_word_t mask = 0x3FFu;

  if (idx < 3) {
    int shift = 22 - 10 * idx;
    tw[10] = (tw[10] & ~(mask << shift)) | ((val & mask) << shift);
    return;
  }

  int shift = 22 - 10 * (idx - 3);
  tw[11] = (tw[11] & ~(mask << shift)) | ((val & mask) << shift);
}

} // namespace dunedaq::fddetdataformats

#include "detail/DAPHNEEthFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEETHFRAME_HPP_
