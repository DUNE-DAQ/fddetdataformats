/**
 * @file Concepts_test.cxx Concepts Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/Concepts.hpp"
#include "fddetdataformats/WIBFrame.hpp"
#include "fddetdataformats/WIB2Frame.hpp"
#include "fddetdataformats/WIBEthFrame.hpp"
#include "fddetdataformats/DAPHNEFrame.hpp"
#include "fddetdataformats/TDE16Frame.hpp"

/**
 * @brief Name of this test module
 */
#define BOOST_TEST_MODULE Concepts_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <cstdint>
#include <type_traits>

using namespace dunedaq::fddetdataformats;

// Test that frame types satisfy the HasTimestamp concept
BOOST_AUTO_TEST_CASE(HasTimestamp_Concept)
{
  static_assert(HasTimestamp<WIBFrame>, "WIBFrame should have timestamp access");
  static_assert(HasTimestamp<WIB2Frame>, "WIB2Frame should have timestamp access");
  static_assert(HasTimestamp<WIBEthFrame>, "WIBEthFrame should have timestamp access");
  static_assert(HasTimestamp<DAPHNEFrame>, "DAPHNEFrame should have timestamp access");
  static_assert(HasTimestamp<TDE16Frame>, "TDE16Frame should have timestamp access");
  
  // Test that header types don't satisfy HasTimestamp (they don't have the interface)
  static_assert(!HasTimestamp<WIB2Frame::Header>, "WIB2Frame::Header should not satisfy HasTimestamp");
  static_assert(!HasTimestamp<DAPHNEFrame::Header>, "DAPHNEFrame::Header should not satisfy HasTimestamp");
}

// Test that frame types satisfy the HasADC concept
BOOST_AUTO_TEST_CASE(HasADC_Concept)
{
  // WIBFrame uses get_channel, not get_adc
  static_assert(!HasADC<WIBFrame>, "WIBFrame uses get_channel, not get_adc");
  
  static_assert(HasADC<WIB2Frame>, "WIB2Frame should have ADC access");
  static_assert(HasADC<WIBEthFrame>, "WIBEthFrame should have ADC access");
  static_assert(HasADC<DAPHNEFrame>, "DAPHNEFrame should have ADC access");
  
  // TDE16Frame uses get_adc_sample instead
  static_assert(!HasADC<TDE16Frame>, "TDE16Frame uses get_adc_sample, not get_adc");
  
  // Test that header types don't satisfy HasADC
  static_assert(!HasADC<WIB2Frame::Header>, "WIB2Frame::Header should not satisfy HasADC");
  static_assert(!HasADC<DAPHNEFrame::Header>, "DAPHNEFrame::Header should not satisfy HasADC");
}

// Test that frame types satisfy the HasADCSample concept
BOOST_AUTO_TEST_CASE(HasADCSample_Concept)
{
  static_assert(HasADCSample<TDE16Frame>, "TDE16Frame should have ADC sample access");
  
  // Other frames use get_adc, not get_adc_sample
  static_assert(!HasADCSample<WIBFrame>, "WIBFrame uses get_channel, not get_adc_sample");
  static_assert(!HasADCSample<WIB2Frame>, "WIB2Frame uses get_adc, not get_adc_sample");
  static_assert(!HasADCSample<WIBEthFrame>, "WIBEthFrame uses get_adc, not get_adc_sample");
  static_assert(!HasADCSample<DAPHNEFrame>, "DAPHNEFrame uses get_adc, not get_adc_sample");
}

// Test that frame types satisfy the HasChannelData concept
BOOST_AUTO_TEST_CASE(HasChannelData_Concept)
{
  static_assert(HasChannelData<WIBFrame>, "WIBFrame should have channel data access");
  
  // Other frames don't have single-parameter get_channel for data access
  static_assert(!HasChannelData<WIB2Frame>, "WIB2Frame doesn't use get_channel for data");
  static_assert(!HasChannelData<WIBEthFrame>, "WIBEthFrame doesn't use get_channel for data");
  static_assert(!HasChannelData<DAPHNEFrame>, "DAPHNEFrame doesn't use get_channel for data");
  static_assert(!HasChannelData<TDE16Frame>, "TDE16Frame doesn't use get_channel for data");
}

// Test that frame types satisfy the HasChannel concept
BOOST_AUTO_TEST_CASE(HasChannel_Concept)
{
  static_assert(HasChannel<WIBEthFrame>, "WIBEthFrame should have channel access");
  static_assert(HasChannel<DAPHNEFrame>, "DAPHNEFrame should have channel access");
  static_assert(HasChannel<TDE16Frame>, "TDE16Frame should have channel access");
  
  // WIBFrame and WIB2Frame don't have get_channel() at the frame level
  static_assert(!HasChannel<WIBFrame>, "WIBFrame should not satisfy HasChannel");
  static_assert(!HasChannel<WIB2Frame>, "WIB2Frame should not satisfy HasChannel");
}

// Test that frame types satisfy the IsFrame concept
BOOST_AUTO_TEST_CASE(IsFrame_Concept)
{
  static_assert(IsFrame<WIBFrame>, "WIBFrame should be a frame");
  static_assert(IsFrame<WIB2Frame>, "WIB2Frame should be a frame");
  static_assert(IsFrame<WIBEthFrame>, "WIBEthFrame should be a frame");
  static_assert(IsFrame<DAPHNEFrame>, "DAPHNEFrame should be a frame");
  static_assert(IsFrame<TDE16Frame>, "TDE16Frame should be a frame");
  
  // Test that header types are not frames
  static_assert(!IsFrame<WIB2Frame::Header>, "WIB2Frame::Header should not be a frame");
  static_assert(!IsFrame<DAPHNEFrame::Header>, "DAPHNEFrame::Header should not be a frame");
}

// Test that appropriate frame types satisfy the IsCompleteFrame concept
BOOST_AUTO_TEST_CASE(IsCompleteFrame_Concept)
{
  static_assert(IsCompleteFrame<WIBEthFrame>, "WIBEthFrame should be a complete frame");
  static_assert(IsCompleteFrame<DAPHNEFrame>, "DAPHNEFrame should be a complete frame");
  static_assert(IsCompleteFrame<TDE16Frame>, "TDE16Frame should be a complete frame");
  
  // These don't have channel access at the frame level
  static_assert(!IsCompleteFrame<WIBFrame>, "WIBFrame should not be a complete frame (no channel access)");
  static_assert(!IsCompleteFrame<WIB2Frame>, "WIB2Frame should not be a complete frame (no channel access)");
}

// Test the HasHeader concept
BOOST_AUTO_TEST_CASE(HasHeader_Concept)
{
  static_assert(HasHeader<WIB2Frame>, "WIB2Frame should have a header type");
  static_assert(HasHeader<DAPHNEFrame>, "DAPHNEFrame should have a header type");
  
  // WIBFrame doesn't define Header as a nested type
  static_assert(!HasHeader<WIBFrame>, "WIBFrame should not satisfy HasHeader (uses WIBHeader separately)");
}

// Test the WordBased concept
BOOST_AUTO_TEST_CASE(WordBased_Concept)
{
  static_assert(WordBased<WIB2Frame>, "WIB2Frame should be word-based");
  static_assert(WordBased<WIBEthFrame>, "WIBEthFrame should be word-based");
  static_assert(WordBased<DAPHNEFrame>, "DAPHNEFrame should be word-based");
}

// Test the BinaryCompatible concept
BOOST_AUTO_TEST_CASE(BinaryCompatible_Concept)
{
  // Headers should be binary compatible
  static_assert(BinaryCompatible<WIB2Frame::Header>, "WIB2Frame::Header should be binary compatible");
  static_assert(BinaryCompatible<WIB2Frame::Trailer>, "WIB2Frame::Trailer should be binary compatible");
  static_assert(BinaryCompatible<WIBHeader>, "WIBHeader should be binary compatible");
  static_assert(BinaryCompatible<ColdataHeader>, "ColdataHeader should be binary compatible");
  static_assert(BinaryCompatible<ColdataSegment>, "ColdataSegment should be binary compatible");
  static_assert(BinaryCompatible<TDEHeader>, "TDEHeader should be binary compatible");
  static_assert(BinaryCompatible<Sample>, "Sample should be binary compatible");
}

// Test the FrameHeader concept
BOOST_AUTO_TEST_CASE(FrameHeader_Concept)
{
  static_assert(FrameHeader<WIB2Frame::Header>, "WIB2Frame::Header should satisfy FrameHeader");
  static_assert(FrameHeader<WIBHeader>, "WIBHeader should satisfy FrameHeader");
  static_assert(FrameHeader<ColdataHeader>, "ColdataHeader should satisfy FrameHeader");
  static_assert(FrameHeader<TDEHeader>, "TDEHeader should satisfy FrameHeader");
  static_assert(FrameHeader<DAPHNEFrame::Header>, "DAPHNEFrame::Header should satisfy FrameHeader");
}

// Generic function template using concepts to demonstrate usage
template<IsFrame F>
uint64_t get_frame_timestamp(const F& frame) {
  return frame.get_timestamp();
}

template<HasADC F>
uint16_t get_frame_adc_value(const F& frame, int index) {
  return frame.get_adc(index);
}

// Test concept-constrained functions
BOOST_AUTO_TEST_CASE(ConceptConstrainedFunctions)
{
  WIB2Frame frame2;
  frame2.set_timestamp(12345);
  BOOST_CHECK_EQUAL(get_frame_timestamp(frame2), 12345);
  
  frame2.set_adc(0, 100);
  BOOST_CHECK_EQUAL(get_frame_adc_value(frame2, 0), 100);
  
  WIBEthFrame frameEth;
  frameEth.set_timestamp(67890);
  BOOST_CHECK_EQUAL(get_frame_timestamp(frameEth), 67890);
  
  frameEth.set_adc(0, 0, 200);
  BOOST_CHECK_EQUAL(get_frame_adc_value(frameEth, 0), 200);
}

// Test that concepts work with template specialization
template<typename T>
struct FrameTraits {
  static constexpr bool is_frame = false;
  static constexpr bool has_channel = false;
};

template<IsFrame T>
struct FrameTraits<T> {
  static constexpr bool is_frame = true;
  static constexpr bool has_channel = HasChannel<T>;
};

BOOST_AUTO_TEST_CASE(ConceptBasedTraits)
{
  static_assert(FrameTraits<WIB2Frame>::is_frame, "WIB2Frame should be recognized as a frame");
  static_assert(!FrameTraits<WIB2Frame>::has_channel, "WIB2Frame doesn't have channel access");
  static_assert(FrameTraits<DAPHNEFrame>::is_frame, "DAPHNEFrame should be recognized as a frame");
  static_assert(FrameTraits<DAPHNEFrame>::has_channel, "DAPHNEFrame has channel access");
  static_assert(!FrameTraits<int>::is_frame, "int should not be recognized as a frame");
}
