/**
 * @file Concepts.hpp
 *
 * Contains C++20 concepts for DUNE Far Detector data format types.
 * These concepts define common interfaces and requirements for frame types,
 * headers, and data structures used in the DUNE DAQ system.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CONCEPTS_HPP_
#define FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CONCEPTS_HPP_

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace dunedaq::fddetdataformats {

/**
 * @brief Concept for types that have timestamp access
 * 
 * A type satisfies HasTimestamp if it provides:
 * - get_timestamp() const -> uint64_t
 * - set_timestamp(uint64_t)
 */
template<typename T>
concept HasTimestamp = requires(T t, const T ct, uint64_t ts) {
  { ct.get_timestamp() } -> std::same_as<uint64_t>;
  { t.set_timestamp(ts) } -> std::same_as<void>;
};

/**
 * @brief Concept for types that have ADC value access
 * 
 * A type satisfies HasADC if it provides:
 * - get_adc(int) const -> uint16_t
 * - set_adc(int, uint16_t)
 * 
 * Note: Some frames like WIBEthFrame require additional parameters.
 * This concept checks for the basic single-parameter interface.
 */
template<typename T>
concept HasADC = requires(T t, const T ct, int idx, uint16_t val) {
  { ct.get_adc(idx) } -> std::same_as<uint16_t>;
  { t.set_adc(idx, val) } -> std::same_as<void>;
};

/**
 * @brief Concept for types that have ADC sample access
 * 
 * A type satisfies HasADCSample if it provides:
 * - get_adc_sample(int) const -> uint16_t
 * - set_adc_sample(uint16_t, int)
 * 
 * This is an alternative interface used by some frame types like TDE16Frame.
 */
template<typename T>
concept HasADCSample = requires(T t, const T ct, int idx, uint16_t val) {
  { ct.get_adc_sample(idx) } -> std::same_as<uint16_t>;
  { t.set_adc_sample(val, idx) } -> std::same_as<void>;
};

/**
 * @brief Concept for types that have channel-based data access
 * 
 * A type satisfies HasChannelData if it provides:
 * - get_channel(uint8_t) const -> uint16_t
 * - set_channel(uint8_t, uint16_t)
 * 
 * This is used by WIBFrame which accesses ADC values via channel index.
 */
template<typename T>
concept HasChannelData = requires(T t, const T ct, uint8_t ch, uint16_t val) {
  { ct.get_channel(ch) } -> std::same_as<uint16_t>;
  { t.set_channel(ch, val) } -> std::same_as<void>;
};

/**
 * @brief Concept for types that have multi-sample ADC access
 * 
 * A type satisfies HasMultiSampleADC if it provides:
 * - get_adc(int, int) const -> uint16_t  (channel, sample)
 * - set_adc(int, int, uint16_t)
 * 
 * This is used by WIBEthFrame which stores multiple time samples per frame.
 */
template<typename T>
concept HasMultiSampleADC = requires(T t, const T ct, int ch, int sample, uint16_t val) {
  { ct.get_adc(ch, sample) } -> std::same_as<uint16_t>;
  { t.set_adc(ch, sample, val) } -> std::same_as<void>;
};

/**
 * @brief Concept for types that have channel access
 * 
 * A type satisfies HasChannel if it provides:
 * - get_channel() const -> convertible to uint16_t
 * 
 * This checks for channel identifier access, not channel data access.
 */
template<typename T>
concept HasChannel = requires(const T ct) {
  { ct.get_channel() } -> std::convertible_to<uint16_t>;
};

/**
 * @brief Concept for types that can be considered frame data structures
 * 
 * A type satisfies IsFrame if it has both timestamp and some form of data access
 * (ADC, ADC sample, channel-based, or multi-sample).
 * This represents the core interface expected of detector frame types.
 */
template<typename T>
concept IsFrame = HasTimestamp<T> && (HasADC<T> || HasADCSample<T> || HasChannelData<T> || HasMultiSampleADC<T>);

/**
 * @brief Concept for complete frame types with all common accessors
 * 
 * A type satisfies IsCompleteFrame if it has timestamp, data access, and channel access.
 */
template<typename T>
concept IsCompleteFrame = HasTimestamp<T> && (HasADC<T> || HasADCSample<T> || HasChannelData<T> || HasMultiSampleADC<T>) && HasChannel<T>;

/**
 * @brief Concept for types that have a header
 * 
 * A type satisfies HasHeader if it provides a header member type and accessor.
 */
template<typename T>
concept HasHeader = requires(T t) {
  typename T::Header;
  requires std::is_class_v<typename T::Header>;
};

/**
 * @brief Concept for types that use word-based data representation
 * 
 * A type satisfies WordBased if it defines a word_t type alias.
 */
template<typename T>
concept WordBased = requires {
  typename T::word_t;
  requires std::is_integral_v<typename T::word_t>;
};

/**
 * @brief Concept for types that are standard layout and trivially copyable
 * 
 * This is important for types that represent binary data formats that
 * will be read from/written to files or network streams.
 */
template<typename T>
concept BinaryCompatible = std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>;

/**
 * @brief Concept for header types that have bit fields
 * 
 * A header type should be binary compatible and contain timestamp information.
 */
template<typename T>
concept FrameHeader = BinaryCompatible<T>;

/**
 * @brief Concept for types that represent ADC data
 * 
 * ADC data types should be binary compatible and provide access to samples.
 */
template<typename T>
concept ADCData = BinaryCompatible<T> && requires(const T ct, int idx) {
  { ct.get_adc(idx) } -> std::convertible_to<uint64_t>;
};

} // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_INCLUDE_FDDETDATAFORMATS_CONCEPTS_HPP_
