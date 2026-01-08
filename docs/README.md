# DUNE Far Detector Data Formats

This repository contains bitfields of far detector raw data and utilities used to decode them. For more on this concept, see also [the detdataformats documentation](https://dune-daq-sw.readthedocs.io/en/latest/packages/detdataformats/). Each section below describes the utilities available for different parts of the far detector. Links are provided to the code; be aware, however, that the code you're linked to is taken from the head of this package's `develop` branch and consequently may differ from the code you may be using. 

## C++20 Concepts
[`Concepts.hpp`](https://github.com/DUNE-DAQ/fddetdataformats/blob/develop/include/fddetdataformats/Concepts.hpp)

This package now includes C++20 concepts that define common interfaces for frame types and data structures. These concepts enable compile-time checking of type requirements and can be used to constrain template parameters. The following concepts are available:

- **`HasTimestamp<T>`**: Types with `get_timestamp()` and `set_timestamp()` methods
- **`HasADC<T>`**: Types with `get_adc(int)` and `set_adc(int, uint16_t)` methods
- **`HasADCSample<T>`**: Types with `get_adc_sample(int)` and `set_adc_sample(uint16_t, int)` methods
- **`HasChannelData<T>`**: Types with `get_channel(uint8_t)` and `set_channel(uint8_t, uint16_t)` methods for data access
- **`HasChannel<T>`**: Types with `get_channel()` method for channel identifier access
- **`IsFrame<T>`**: Types that have both timestamp and data access (satisfies `HasTimestamp` and at least one data access concept)
- **`IsCompleteFrame<T>`**: Types with timestamp, data access, and channel identifier
- **`HasHeader<T>`**: Types with a nested `Header` type
- **`WordBased<T>`**: Types that define a `word_t` type alias
- **`BinaryCompatible<T>`**: Types that are standard layout and trivially copyable
- **`FrameHeader<T>`**: Binary compatible header types
- **`ADCData<T>`**: Binary compatible types with ADC data access

Example usage:
```cpp
#include "fddetdataformats/Concepts.hpp"

// Generic function that works with any frame type
template<IsFrame F>
void process_frame(const F& frame) {
  auto timestamp = frame.get_timestamp();
  // ... process frame ...
}

// Function constrained to frames with ADC access
template<HasADC F>
uint16_t read_adc_value(const F& frame, int channel) {
  return frame.get_adc(channel);
}
```

All frame types in this package (`WIBFrame`, `WIB2Frame`, `WIBEthFrame`, `DAPHNEFrame`, `TDE16Frame`) satisfy the `IsFrame` concept with their respective data access interfaces.

## WIB
[`WIBFrame.hpp`](https://github.com/DUNE-DAQ/fddetdataformats/blob/develop/include/fddetdataformats/WIBFrame.hpp)

The `WIBFrame` class contains a nested set of overlay classes and structs. `WIBFrame` includes [private instances](https://dune-daq-sw.readthedocs.io/en/latest/packages/styleguide/#58-access-control) of `WIBHeader` and an array of `ColdataBlock` structs, accessible via getter functions. Each `ColdataBlock` struct contains a [public instance](https://dune-daq-sw.readthedocs.io/en/latest/packages/styleguide/#58-access-control) of a `ColdataHeader` and a public array of `ColdataSegments`. 

Through these classes and structs it's possible to access the value of any channel/ADC combination in a WIB frame, as well as set these values. In the case of both `ColdataBlock` and its component `ColdataSegments`, only the channel and ADC numbers need to be provided; to do this directly with an instance of a `WIBFrame`, the `ColdataBlock` index needs to be provided as well. 

Other useful functions in `WIBFrame` include setters for the timestamp and WIB errors in its `WIBHeader` instance, as well as a streamer which allows developers to easily print the contents of the `WIBFrame` instance using the `<<` operator on the instance. 

## WIB2
[`WIB2Frame.hpp`](https://github.com/DUNE-DAQ/fddetdataformats/blob/develop/include/fddetdataformats/WIB2Frame.hpp)

A `WIB2Frame` instance contains as members instances of the `WIB2Frame::Header` and `WIB2Frame::Trailer` structs as well as an array of ADC values. This reflects the definition of the WIB format given in https://edms.cern.ch/document/2088713/4. It also has getters and setters for ADC values, including options to select among U-channel, V-channel and X-channel ADCs when doing so. It's also possible to easily obtain the timestamp of the frame via `get_timestamp`. 

## DAPHNE
[`DAPHNEFrame.hpp`](https://github.com/DUNE-DAQ/fddetdataformats/blob/develop/include/fddetdataformats/DAPHNEFrame.hpp)

`DAPHNEFrame` is used for working with data produced by DAPHNE front end boards (DAPHNE = Detector electronics for Acquiring PHotons from Neutrinos). Technical details on DAPHNE can be found in https://edms.cern.ch/document/2088726/3. The structure of `DAPHNEFrame` is similar in some ways to the structure of `WIB2Frame`: it consists of an instance of a `DAPHNEFrame::Header` and `DAPHNEFrame::Trailer` struct, as well as an array of ADCs. It also contains a `get_timestamp` function as well as getters and setters for ADC values. 
