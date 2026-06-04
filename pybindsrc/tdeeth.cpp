/**
 * @file tdeeth.cpp Python bindings for the TDEEthFrame format
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/TDEEthFrame.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

// NOLINTBEGIN(build/unsigned)
  
void
register_tdeeth(py::module& m)
{
  py::class_<TDEEthFrame::TDEEthHeader>(m, "TDEEthHeader")
    .def_property(
      "channel",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t { return self.channel; },
      [](TDEEthFrame::TDEEthHeader& self, uint16_t channel) { self.channel = channel; })
    .def_property(
      "version",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t { return self.version; },
      [](TDEEthFrame::TDEEthHeader& self, uint16_t version) { self.version = version; })
    .def_property(
      "tde_header",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t { return self.tde_header; },
      [](TDEEthFrame::TDEEthHeader& self, uint16_t tde_header) { self.tde_header = tde_header; })
    .def_property(
      "tde_errors",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t { return self.tde_errors; },
      [](TDEEthFrame::TDEEthHeader& self, uint16_t tde_errors) { self.tde_errors = tde_errors; })
    .def_property(
      "TAItime",
      [](TDEEthFrame::TDEEthHeader& self) -> uint64_t { return self.TAItime; },
      [](TDEEthFrame::TDEEthHeader& self, uint64_t TAItime) { self.TAItime = TAItime; })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return TDEEthFrame::TDEEthHeader::s_expected_bytes;
    });

  py::class_<TDEEthFrame>(m, "TDEEthFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
      auto tfp = *static_cast<TDEEthFrame*>(capsule.get_pointer());
      return tfp;
    }))
    .def(py::init([](py::bytes bytes) {
      py::buffer_info info(py::buffer(bytes).request());
      auto tfp = *static_cast<TDEEthFrame*>(info.ptr);
      return tfp;
    }))
    .def(
      "get_daqheader",
      [](TDEEthFrame& self) -> const detdataformats::DAQEthHeader& { return self.get_daqheader(); },
      py::return_value_policy::reference_internal)
    .def(
      "get_tdeheader",
      [](TDEEthFrame& self) -> const TDEEthFrame::TDEEthHeader& { return self.get_header(); },
      py::return_value_policy::reference_internal)
    .def(
      "get_header",
      [](TDEEthFrame& self) -> const TDEEthFrame::TDEEthHeader& { return self.get_header(); },
      py::return_value_policy::reference_internal)
    .def("get_adc", &TDEEthFrame::get_adc)
    .def("set_adc", &TDEEthFrame::set_adc)
    .def("get_timestamp", &TDEEthFrame::get_timestamp)
    .def("set_timestamp", &TDEEthFrame::set_timestamp)
    .def("get_channel", &TDEEthFrame::get_channel)
    .def("set_channel", &TDEEthFrame::set_channel)
    .def("set_geoid", &TDEEthFrame::set_geoid)
    .def("__lt__", [](const TDEEthFrame& lhs, const TDEEthFrame& rhs) { return lhs < rhs; })
    .def_property_readonly_static("s_bits_per_adc", [](py::object /*self*/) {
      return TDEEthFrame::s_bits_per_adc;
    })
    .def_property_readonly_static("s_bits_per_word", [](py::object /*self*/) {
      return TDEEthFrame::s_bits_per_word;
    })
    .def_property_readonly_static("s_time_samples_per_frame", [](py::object /*self*/) {
      return TDEEthFrame::s_time_samples_per_frame;
    })
    .def_property_readonly_static("s_channels_per_half_femb", [](py::object /*self*/) {
      return TDEEthFrame::s_channels_per_half_femb;
    })
    .def_property_readonly_static("s_half_fembs_per_frame", [](py::object /*self*/) {
      return TDEEthFrame::s_half_fembs_per_frame;
    })
    .def_property_readonly_static("s_num_channels", [](py::object /*self*/) {
      return TDEEthFrame::s_num_channels;
    })
    .def_property_readonly_static("s_num_adc_words_per_ts", [](py::object /*self*/) {
      return TDEEthFrame::s_num_adc_words_per_ts;
    })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return TDEEthFrame::s_expected_bytes;
    })
    .def_static("sizeof", []() { return sizeof(TDEEthFrame); })
    .def("get_bytes", [](TDEEthFrame* fr) -> py::bytes {
      return py::bytes(reinterpret_cast<char*>(fr), sizeof(TDEEthFrame)); // NOLINT reinterpret_cast
    });
} // NOLINT (defensible use of "overly long" function)

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats::python
