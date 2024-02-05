/**
 * @file CRTFrame.hpp CRT bit fields and accessors.
 *
 * This is part of the DUNE DAQ , copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTFRAME_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTFRAME_HPP_

#include "detdataformats/DAQEthHeader.hpp"

#include <bitset>
#include <iostream>
#include <cstdint>
#include <vector>

namespace dunedaq::fddetdataformats {

/**
 * @brief CRT frame
 */
class CRTFrame
{
  public:
  // ===============================================================
  // Preliminaries
  // ===============================================================
  
  // The definition of the format is in terms of 32-bit words
  typedef uint32_t word_t; // NOLINT

  static constexpr int DAQ_HEADER_LEN = 16; // bytes
  static constexpr int CRT_HEADER_LEN = 16; // bytes
  static constexpr int CRT_HIT_LEN = 4;  // bytes

  struct __attribute__((__packed__)) CRTHeader
  {
    word_t magic_number : 8, hit_count : 8, module_number : 16; 
    word_t upper_global_timestamp;
    word_t lower_global_timestamp;
    word_t sixteen_ns_counter_timestamp;

  };
  struct __attribute__((__packed__)) CRTHit
  {
   char hit_magic_number:8;
   uint8_t hit_channel:8;
   int16_t hit_adc:16;
  };

  // ===============================================================
  // Data members
  // ===============================================================

  detdataformats::DAQEthHeader* daq_header;
  CRTHeader* crt_header;

  // ===============================================================
  // Accessors
  // ===============================================================

  /** @brief Gets the ith hit in the CRT frame
   */
  CRTHit* get_crt_hit(void* msg, const uint32_t i) const
  {
    return reinterpret_cast<CRTHit*>(static_cast<uint8_t*>(msg) + DAQ_HEADER_LEN + CRT_HEADER_LEN + CRT_HIT_LEN * i);
  }
  
  /** @brief Get the expected number of hits in the CRT frame
   */
  uint8_t get_expected_hit_count() const //NOLINT
  {
    return crt_header->hit_count;
  }
  
  /** @brief Get the ADC value of the ith hit in the frame
   */
  int16_t get_adc(int i) const // NOLINT
  {
    if(i < 0 || i >= crt_header->hit_count)
      throw std::out_of_range("ADC index out of range");

    return (get_crt_hit((void*)this, i))->hit_adc;
  }
  
  /** @brief Set the ADC value of the ith hit in the frame
   */
  void set_adc(int i, int16_t val) // NOLINT
  {
    if(i < 0 || i >= crt_header->hit_count)
      throw std::out_of_range("ADC index out of range");

    CRTHit* thishit = get_crt_hit((void*)this, i);
    thishit->hit_adc = val;
  }
  
  /** @brief Get the channel number of the ith hit
   */
  uint8_t get_channel(int i) const // NOLINT
  {
    if(i < 0 || i >= crt_header->hit_count)
      throw std::out_of_range("Channel index out of range");

    return (get_crt_hit((void*)this, i))->hit_channel;
  }
  
  /** @brief Set the channel number of the ith hit
   */
  void set_channel(int i, uint8_t val) // NOLINT
  {
    if(i < 0 || i >= crt_header->hit_count)
      throw std::out_of_range("Channel index out of range");

    CRTHit* thishit = get_crt_hit((void*)this, i);
    thishit->hit_channel = val;
  }
  
  /** @brief Get the module from the CRT frame
   */
  uint16_t get_module() const // NOLINT(build/unsigned) 
  {
    return crt_header->module_number;
  }

  /** @brief Set the module of the CRT frame
   */
  void set_module( uint16_t val) // NOLINT(build/unsigned) 
  {
    crt_header->module_number = val;
  }

  /** @brief Get the 64-bit timestamp of the frame
   */
  uint64_t get_timestamp() const // NOLINT(build/unsigned)
  {
    return daq_header->get_timestamp();
  }

  /** @brief Set the starting 64-bit timestamp of the frame
   */
  void set_timestamp(const uint64_t new_timestamp) // NOLINT(build/unsigned)
  {
    daq_header->timestamp = new_timestamp;
  }

  friend std::ostream& operator<<(std::ostream& o, CRTFrame const& frame);

private:
};

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CRTFRAME_HPP_
