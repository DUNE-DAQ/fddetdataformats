
#ifndef FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_FRAMECONCEPTS_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_FRAMECONCEPTS_HPP_

#include "detdataformats/DAQHeader.hpp"
#include "detdataformats/DAQEthHeader.hpp"

#include <concepts>
#include <cstdint>

namespace dunedaq::fddetdataformats {

  // ensure ADC functions exist; will think about their different arguments later
  
template <typename T>
concept HasGetADC = std::is_member_function_pointer_v<decltype(&T::get_adc)>;

template <typename T>
concept HasSetADC = std::is_member_function_pointer_v<decltype(&T::set_adc)>;

template <typename T>
concept HasValidDAQHeader =
    requires(const T t) {
  t.get_daqheader();
    } &&
    (
     std::same_as<decltype(std::declval<T>().get_daqheader()), const dunedaq::detdataformats::DAQEthHeader&> ||
     std::same_as<decltype(std::declval<T>().get_daqheader()), const dunedaq::detdataformats::DAQHeader&>
    );

  // Instead of std::totally_ordered, this just literally only requires the "<" operator
  template <typename T>
  concept HasLessThan = requires(const T a, const T b) {
    { a < b } -> std::convertible_to<bool>;
  };

  template <typename T>
  concept HasNoCompilerPadding =
    requires {
    // "convertible_to" is used rather than "same_as" because s_expected_bytes is typically constexpr
    { T::s_expected_bytes } -> std::convertible_to<std::size_t>;
  }
    &&
  (T::s_expected_bytes == sizeof(T));

template <typename T>
concept AdaptableFrameConcept =
  HasNoCompilerPadding<T> &&
  HasLessThan<T> &&
  HasGetADC<T> &&
  HasSetADC<T> &&
  HasValidDAQHeader<T> &&
    requires(T t, const T ct, uint64_t ts)
{
    { ct.get_timestamp() } -> std::same_as<uint64_t>;
    { t.set_timestamp(ts) } -> std::same_as<void>;
};

} // namespace dunedaq::fddetdataformats

#endif  // FDDETDATAFORMATS_INCLUDE_FDDATAFORMATS_FRAMECONCEPTS_HPP_
