/**
 * @file DAPHNEFrame.hpp
 *
 *  Contains declaration of DAPHNEFrame, a struct for accessing raw DAPHNE frames, as produced by the DAPHNE boards
 *
 *  The canonical definition of the PDS DAPHNE format is given in EDMS document 2088726:
 *  https://edms.cern.ch/document/2088726/3
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEFRAME_HPP_

#include "fddetdataformats/FrameConcepts.hpp"
#include "fddetdataformats/Utils.hpp"

#include "detdataformats/DAQHeader.hpp"
#include <algorithm> // For std::min
#include <cassert>   // For assert()
#include <cstdint>   // For uint32_t etc
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::out_of_range

namespace dunedaq::fddetdataformats {

// NOLINTBEGIN(build/unsigned)

struct DAPHNEFrame
{
  // The definition of the format is in terms of 32-bit words
  using word_t = uint32_t;

  // Dataframe format version
  static constexpr uint8_t version = 2;

  static constexpr int s_bits_per_adc = 14;
  static constexpr int s_bits_per_word = 8 * sizeof(word_t);
  static constexpr int s_num_adcs = 1024;
  static constexpr int s_num_adc_words = s_num_adcs * s_bits_per_adc / s_bits_per_word;

  struct Header
  {
    word_t channel : 6, algorithm_id : 4, reserved_1 : 5, r1 : 1, trigger_sample_value : 16;
    word_t threshold : 16, baseline : 16;
    word_t get_baseline() { return baseline; }
  };
  static_assert(sizeof(Header) == 8);

  struct PeakDescriptorData
  {
    static constexpr int s_expected_bytes { 13 * sizeof(uint32_t) };

    // Word 1: peak 0 odd
    // Declared in reverse order (LSB first) so that:
    //   - num_subpeaks_0 occupies bits [3:0]
    //   - reserved_0 occupies bits [7:4]
    //   - adc_integral_0 occupies bits [30:8]
    //   - found_0 occupies bit [31]
    word_t num_subpeaks_0 : 4;  // Num_SubPeaks [3:0]
    word_t reserved_0 : 4;      // Reserved      [7:4]
    word_t adc_integral_0 : 23; // ADC_Integral [30:8]
    word_t found_0 : 1;         // Found       [31]

    // Word 2: peak 0 even
    // Declared (LSB first) so that:
    //   - adc_max_0 occupies bits [13:0]
    //   - sample_max_0 occupies bits [22:14]
    //   - samples_over_baseline_0 occupies bits [31:23]
    word_t adc_max_0 : 14;              // ADC Max         [13:0]
    word_t sample_max_0 : 9;            // Time_Peak        [22:14]
    word_t samples_over_baseline_0 : 9; // Time_Over_Baseline [31:23]

    // Word 3: peak 1 odd
    word_t num_subpeaks_1 : 4;  // Num_SubPeaks [3:0]
    word_t reserved_1 : 4;      // Reserved      [7:4]
    word_t adc_integral_1 : 23; // ADC_Integral [30:8]
    word_t found_1 : 1;         // Found       [31]

    // Word 4: peak 1 even
    word_t adc_max_1 : 14;              // ADC Max         [13:0]
    word_t sample_max_1 : 9;            // Time_Peak        [22:14]
    word_t samples_over_baseline_1 : 9; // Time_Over_Baseline [31:23]

    // Word 5: peak 2 odd
    word_t num_subpeaks_2 : 4;  // Num_SubPeaks [3:0]
    word_t reserved_2 : 4;      // Reserved      [7:4]
    word_t adc_integral_2 : 23; // ADC_Integral [30:8]
    word_t found_2 : 1;         // Found       [31]

    // Word 6: peak 2 even
    word_t adc_max_2 : 14;              // ADC Max         [13:0]
    word_t sample_max_2 : 9;            // Time_Peak        [22:14]
    word_t samples_over_baseline_2 : 9; // Time_Over_Baseline [31:23]

    // Word 7: peak 3 odd
    word_t num_subpeaks_3 : 4;  // Num_SubPeaks [3:0]
    word_t reserved_3 : 4;      // Reserved      [7:4]
    word_t adc_integral_3 : 23; // ADC_Integral [30:8]
    word_t found_3 : 1;         // Found       [31]

    // Word 8: peak 3 even
    word_t adc_max_3 : 14;              // ADC Max         [13:0]
    word_t sample_max_3 : 9;            // Time_Peak        [22:14]
    word_t samples_over_baseline_3 : 9; // Time_Over_Baseline [31:23]

    // Word 9: peak 4 odd
    word_t num_subpeaks_4 : 4;  // Num_SubPeaks [3:0]
    word_t reserved_4 : 4;      // Reserved      [7:4]
    word_t adc_integral_4 : 23; // ADC_Integral [30:8]
    word_t found_4 : 1;         // Found       [31]

    // Word 10: peak 4 even
    word_t adc_max_4 : 14;              // ADC Max         [13:0]
    word_t sample_max_4 : 9;            // Time_Peak        [22:14]
    word_t samples_over_baseline_4 : 9; // Time_Over_Baseline [31:23]

    // Word 11: Time_Start fields for indices 0,1,2 and Reserved
    // Declared in LSB-first order:
    //   - samples_start_2 occupies bits [11:2]
    //   - samples_start_1 occupies bits [21:12]
    //   - samples_start_0 occupies bits [31:22]
    //   - reserved_5 occupies bits [1:0]
    word_t samples_start_2 : 10; // Time_Start(2) [11:2]
    word_t samples_start_1 : 10; // Time_Start(1) [21:12]
    word_t samples_start_0 : 10; // Time_Start(0) [31:22]
    word_t reserved_5 : 2;       // Reserved         [1:0]

    // Word 12: Time_Start fields for indices 3,4 and Reserved
    // Declared in LSB-first order:
    //   - reserved_6 occupies bits [11:0]
    //   - samples_start_4 occupies bits [21:12]
    //   - samples_start_3 occupies bits [31:22]
    word_t reserved_6 : 12;      // Reserved         [11:0]
    word_t samples_start_4 : 10; // Time_Start(4) [21:12]
    word_t samples_start_3 : 10; // Time_Start(3) [31:22]

    // Word 13: Trailer word (all 32 bits), typically 0xFFFFFFFF.
    word_t trailer;

    static const uint8_t max_peaks = 5;

    /**
     * @brief Get the Found value for a specific peak (channel) from the trailer.
     *        (Word 2*idx, bit 31)
     */
    bool is_found(int idx) const;

    /// @brief Set the Found value for a specific peak (channel) in the trailer.
    void set_found(uint8_t val, int idx);

    /**
     * @brief Get the ADC_Integral value for a specific peak.
     *        (Word 2*idx, bits [30:8])
     */
    uint32_t get_adc_integral(int idx) const;

    /// @brief Set the ADC_Integral value for a specific peak.
    void set_adc_integral(uint32_t val, int idx);

    /**
     * @brief Get the Num_SubPeaks value for a specific peak.
     *        (Word 2*idx, bits [3:0])
     */
    uint8_t get_num_subpeaks(int idx) const;

    /// @brief Set the Num_SubPeaks value for a specific peak.
    void set_num_subpeaks(uint8_t val, int idx);

    /**
     * @brief Get the Time_Over_Baseline value for a specific peak.
     *        (Word 2*idx+1, bits [8:0])
     */
    uint16_t get_samples_over_baseline(int idx) const;

    // @brief Set the Time_Over_Baseline value for a specific peak.
    void set_samples_over_baseline(uint16_t val, int idx);

    /**
     * @brief Get the Time_Peak value for a specific peak.
     *        (Word 2*idx+1, bits [17:9])
     */
    uint16_t get_sample_max(int idx) const;

    /// @brief Set the Time_Peak value for a specific peak.
    void set_sample_max(uint16_t val, int idx);

    /**
     * @brief Get the ADC Max value for a specific peak.
     *        (Word 2*idx+1, bits [31:18])
     */
    uint16_t get_adc_max(int idx) const;

    /// @brief Set the ADC Max value for a specific peak.
    void set_adc_max(uint16_t val, int idx);

    /**
     * @brief Get the Time_Start value for a given index (0-4).
     *
     * For indices 0,1,2 these are stored in trailer word 11 (index 10):
     *   - index 0: bits [9:0]
     *   - index 1: bits [19:10]
     *   - index 2: bits [29:20]
     *
     * For indices 3,4 these are stored in trailer word 12 (index 11):
     *   - index 3: bits [9:0]
     *   - index 4: bits [19:10]
     */

    uint16_t get_sample_start(int idx) const;

    /**
     * @brief Set the time_start field for Peak index 0–4 using bit shifts.
     *
     * Trailer word 11 (index 10):
     *   - idx 0: bits [31:22]
     *   - idx 1: bits [21:12]
     *   - idx 2: bits [11:2]
     * Trailer word 12 (index 11):
     *   - idx 3: bits [31:22]
     *   - idx 4: bits [21:12]
     */
    void set_sample_start(uint16_t val, int idx);

    // ===============================================================
    // Helper: Reinterpret Trailer as an array of word_t
    // ===============================================================
    const word_t* as_words() const
    {
      return reinterpret_cast<const word_t*>(this); // NOLINT
    }
    word_t* as_words()
    {
      return reinterpret_cast<word_t*>(this); // NOLINT
    }
  };
  static_assert(sizeof(PeakDescriptorData) == PeakDescriptorData::s_expected_bytes);

  static constexpr int s_expected_bytes { sizeof(detdataformats::DAQHeader) + sizeof(Header) +
    s_num_adc_words * sizeof(word_t) + PeakDescriptorData::s_expected_bytes };
  
  /**
   * @brief Get the ith ADC value in the frame
   *
   * The ADC words are 14 bits long, stored packed in the data structure. The order is:
   *
   * - 1024 adc values from one daphne channel
   */
  uint16_t get_adc(int i) const;

  /// @brief Set the ith ADC value in the frame to @p val
  void set_adc(int i, uint16_t val);

  uint8_t get_channel() const { return header.channel; }
  void set_channel(uint8_t val) { header.channel = val & 0x3Fu; }

  /// @brief Get the 64-bit timestamp of the frame
  uint64_t get_timestamp() const { return daq_header.get_timestamp(); }

  void set_timestamp(uint64_t ts) {
    daq_header.timestamp_1 = ts;
    daq_header.timestamp_2 = ts >> 32;
  }
  
  bool operator<(const DAPHNEFrame& other) const {

    if (this->get_timestamp() != other.get_timestamp()) {
      return this->get_timestamp() < other.get_timestamp();
    } else {
      return this->get_channel() < other.get_channel();
    }
  }

  detdataformats::DAQHeader daq_header;
  Header header;
  word_t adc_words[s_num_adc_words]; // NOLINT
  PeakDescriptorData peaks_data;
};

static_assert(std::endian::native == std::endian::little,
              "The DAPHNEFrame bitfield layout assumes little-endian architecture");

  static_assert(AdaptableFrameConcept<DAPHNEFrame>, "DAPHNEFrame does not satisfy the AdaptableFrameConcept");
  
} // namespace dunedaq::fddetdataformats

#include "detail/DAPHNEFrame.hxx"

// NOLINTEND(build/unsigned)

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_DAPHNEFRAME_HPP_
