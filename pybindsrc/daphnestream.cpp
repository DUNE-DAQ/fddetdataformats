/**
 * @file daphnestream.cpp Python bindings for the DAPHNEStreamFrame format
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEStreamFrame.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

// NOLINTBEGIN(build/unsigned)

void
register_daphnestream(py::module& m)
{

  py::class_<DAPHNEStreamFrame::Header>(m, "DAPHNEStreamHeader")
    .def_property(
      "channel_0",
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_0; },
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_0) { self.channel_0 = channel_0; })
    .def_property(
      "channel_1",
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_1; },
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_1) { self.channel_1 = channel_1; })
    .def_property(
      "channel_2",
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_2; },
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_2) { self.channel_2 = channel_2; })
    .def_property(
      "channel_3",
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_3; },
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_3) { self.channel_3 = channel_3; })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return sizeof(DAPHNEStreamFrame::Header);
    });

  py::class_<DAPHNEStreamFrame>(m, "DAPHNEStreamFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
      auto wfp = *static_cast<DAPHNEStreamFrame*>(capsule.get_pointer());
      return wfp;
    }))
    .def(py::init([](py::bytes bytes) {
      py::buffer_info info(py::buffer(bytes).request());
      auto wfp = *static_cast<DAPHNEStreamFrame*>(info.ptr);
      return wfp;
    }))
    .def(
      "get_daqheader",
      [](DAPHNEStreamFrame& self) -> const detdataformats::DAQHeader& { return self.get_daqheader(); },
      py::return_value_policy::reference_internal)
    .def(
      "get_header",
      [](DAPHNEStreamFrame& self) -> const DAPHNEStreamFrame::Header& { return self.get_header(); },
      py::return_value_policy::reference_internal)
    .def("get_timestamp", &DAPHNEStreamFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEStreamFrame::set_timestamp)
    .def("get_adc", &DAPHNEStreamFrame::get_adc)
    .def("set_adc", &DAPHNEStreamFrame::set_adc)
    .def_property_readonly_static("s_bits_per_adc", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_bits_per_adc;
    })
    .def_property_readonly_static("s_bits_per_word", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_bits_per_word;
    })
    .def_property_readonly_static("s_channels_per_frame", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_channels_per_frame;
    })
    .def_property_readonly_static("s_adcs_per_channel", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_adcs_per_channel;
    })
    .def_property_readonly_static("s_daphnes_per_frame", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_daphnes_per_frame;
    })
    .def_property_readonly_static("s_num_adc_words", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_num_adc_words;
    })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEStreamFrame::s_expected_bytes;
    })
    .def("get_channel0", &DAPHNEStreamFrame::get_channel0)
    .def("get_channel1", &DAPHNEStreamFrame::get_channel1)
    .def("get_channel2", &DAPHNEStreamFrame::get_channel2)
    .def("get_channel3", &DAPHNEStreamFrame::get_channel3)
    .def_static("sizeof", []() { return sizeof(DAPHNEStreamFrame); })
    .def("get_bytes", [](DAPHNEStreamFrame* fr) -> py::bytes {
      return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEStreamFrame)); // NOLINT
    });
}

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats::python
