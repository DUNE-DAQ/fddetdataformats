/**
 * @file FrameConcepts.hpp
 *
 * Contains declaration of various concepts to which a DAQ frame should adhere, including:
 *
 * -Having getters and setters for timestamps
 *
 * -Having an "<" operator implemented
 *
 * -Having an s_expected_bytes static member which equals the sizeof
 *  of the frame, and which should be assigned a value which is simply
 *  the sum of the sizes of the members - this guards against compiler
 *  padding
 *
 * -Satisfying std::is_trivially_copyable_v (for std::memcpy) and and
 *  std::is_standard_layout_v (members appear in memory in declaration
 *  order + offsetof can be used)
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_FRAMECONCEPTS_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_FRAMECONCEPTS_HPP_

#include "detdataformats/DAQHeader.hpp"
#include "detdataformats/DAQEthHeader.hpp"

#include <concepts>
#include <cstdint>

namespace dunedaq::fddetdataformats {

  
template <typename T>
concept HasGetADC = std::is_member_function_pointer_v<decltype(&T::get_adc)>;

template <typename T>
concept HasSetADC = std::is_member_function_pointer_v<decltype(&T::set_adc)>;
  
template <typename T>
concept HasDAQHeader =
    requires(const T t) {
  t.daq_header;
    } &&
    (
     std::same_as<decltype(std::declval<T>().daq_header), dunedaq::detdataformats::DAQEthHeader> ||
     std::same_as<decltype(std::declval<T>().daq_header), dunedaq::detdataformats::DAQHeader>
    );

  template <typename T>
  concept HasFrameHeader =
    requires(const T t) {
    t.header;
  }; // NOLINT(readability/braces)

  // Instead of std::totally_ordered, this just literally only requires the "<" operator
  template <typename T>
  concept HasLessThan = requires(const T a, const T b) {
    { a < b } -> std::convertible_to<bool>;
  }; // NOLINT(readability/braces)

  template <typename T>
  concept HasNoCompilerPadding =
    requires {
    // "convertible_to" is used rather than "same_as" because s_expected_bytes is typically constexpr
    { T::s_expected_bytes } -> std::convertible_to<std::size_t>;
  }
    &&
  (T::s_expected_bytes == sizeof(T));

template <typename T>  
concept HasGetTimestamp =
  requires(const T ct)
  {
    { ct.get_timestamp() } -> std::same_as<uint64_t>; // NOLINT(build/unsigned)
  };

template <typename T>
concept HasSetTimestamp =
  requires(T t, uint64_t ts) // NOLINT(build/unsigned) 
  {
    { t.set_timestamp(ts) } -> std::same_as<void>;
  };

  // TODO: John Freeman (jcfree@fnal.gov), Jun-17-2026

  // In the next two months, figure out a concept parallel to
  // AdaptableFrameConcept which can be made specific to the type
  // adapters in fdreadoutlibs (as opposed to, e.g.,
  // TriggerPrimitiveTypeAdapter.hpp in trigger). This can include
  // concepts commented out below like "has an ADC getter", etc.)

template <typename T>
concept AdaptableFrameConcept =
  HasNoCompilerPadding<T> &&
  HasLessThan<T> &&
  //  HasGetADC<T> &&
  //  HasSetADC<T> &&
  //  HasDAQHeader<T> &&
  //  HasFrameHeader<T> &&
  HasGetTimestamp<T> &&
  HasSetTimestamp<T> &&
  std::is_standard_layout_v<T> &&
  std::is_trivially_copyable_v<T>;

} // namespace dunedaq::fddetdataformats

#endif  // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_FRAMECONCEPTS_HPP_
