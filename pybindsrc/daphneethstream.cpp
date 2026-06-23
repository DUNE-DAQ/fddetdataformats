/**
 * @file daphneethstream.cpp Python bindings for the DAPHNEEthStreamFrame format
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEEthStreamFrame.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

// NOLINTBEGIN(build/unsigned)

void
register_daphneethstream(py::module& m)
{

  py::class_<DAPHNEEthStreamFrame::ChannelWord>(m, "DAPHNEEthStreamChannelWord")
    .def_property(
      "tbd",
      [](DAPHNEEthStreamFrame::ChannelWord& self) -> uint64_t { return self.tbd; },
      [](DAPHNEEthStreamFrame::ChannelWord& self, uint64_t tbd) { self.tbd = tbd; })
    .def_property(
      "version",
      [](DAPHNEEthStreamFrame::ChannelWord& self) -> uint64_t { return self.version; },
      [](DAPHNEEthStreamFrame::ChannelWord& self, uint64_t version) { self.version = version; })
    .def_property(
      "channel",
      [](DAPHNEEthStreamFrame::ChannelWord& self) -> uint64_t { return self.channel; },
      [](DAPHNEEthStreamFrame::ChannelWord& self, uint64_t channel) { self.channel = channel; })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return sizeof(DAPHNEEthStreamFrame::ChannelWord);
    });

  py::class_<DAPHNEEthStreamFrame::Header>(m, "DAPHNEEthStreamHeader")
    .def_property(
      "channel_words",
      [](DAPHNEEthStreamFrame::Header& self) -> py::list {
        py::list result;
        for (int i = 0; i < 4; i++) {
          result.append(self.channel_words[i]);
        }
        return result;
      },
      [](DAPHNEEthStreamFrame::Header& self, py::list channel_words) {
        for (int i = 0; i < 4 && i < static_cast<int>(len(channel_words)); i++) {
          self.channel_words[i] = channel_words[i].cast<DAPHNEEthStreamFrame::ChannelWord>();
        }
      })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return sizeof(DAPHNEEthStreamFrame::Header);
    });

  py::class_<DAPHNEEthStreamFrame>(m, "DAPHNEEthStreamFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
      auto wfp = *static_cast<DAPHNEEthStreamFrame*>(capsule.get_pointer());
      return wfp;
    }))
    .def(py::init([](py::bytes bytes) {
      py::buffer_info info(py::buffer(bytes).request());
      auto wfp = *static_cast<DAPHNEEthStreamFrame*>(info.ptr);
      return wfp;
    }))
    .def(
      "get_daqheader",
      [](DAPHNEEthStreamFrame& self) -> const detdataformats::DAQEthHeader& { return self.daq_header; },
      py::return_value_policy::reference_internal)
    .def(
      "get_daphneheader",
      [](DAPHNEEthStreamFrame& self) -> const DAPHNEEthStreamFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def(
      "get_header",
      [](DAPHNEEthStreamFrame& self) -> const DAPHNEEthStreamFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def_property_readonly(
      "daq_header",
      [](DAPHNEEthStreamFrame& self) -> detdataformats::DAQEthHeader& { return self.daq_header; },
      py::return_value_policy::reference_internal)
    .def_property_readonly(
      "header",
      [](DAPHNEEthStreamFrame& self) -> DAPHNEEthStreamFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def("get_adc", &DAPHNEEthStreamFrame::get_adc)
    .def("set_adc", &DAPHNEEthStreamFrame::set_adc)
    .def("get_timestamp", &DAPHNEEthStreamFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEEthStreamFrame::set_timestamp)
    .def("__lt__", [](const DAPHNEEthStreamFrame& lhs, const DAPHNEEthStreamFrame& rhs) { return lhs < rhs; })
    .def("get_channel", &DAPHNEEthStreamFrame::get_channel)
    .def("set_channel", &DAPHNEEthStreamFrame::set_channel)
    .def_property_readonly_static("version", [](py::object /*self*/) { return DAPHNEEthStreamFrame::version; })
    .def_property_readonly_static("s_bits_per_adc", [](py::object /*self*/) {
      return DAPHNEEthStreamFrame::s_bits_per_adc;
    })
    .def_property_readonly_static("s_bits_per_word", [](py::object /*self*/) {
      return DAPHNEEthStreamFrame::s_bits_per_word;
    })
    .def_property_readonly_static("s_adcs_per_channel", [](py::object /*self*/) {
      return DAPHNEEthStreamFrame::s_adcs_per_channel;
    })
    .def_property_readonly_static("s_num_channels", [](py::object /*self*/) {
      return DAPHNEEthStreamFrame::s_num_channels;
    })
    .def_property_readonly_static("s_num_adc_words", [](py::object /*self*/) {
      return DAPHNEEthStreamFrame::s_num_adc_words;
    })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEEthStreamFrame::s_expected_bytes;
    })
    .def("get_channel0", &DAPHNEEthStreamFrame::get_channel0)
    .def("get_channel1", &DAPHNEEthStreamFrame::get_channel1)
    .def("get_channel2", &DAPHNEEthStreamFrame::get_channel2)
    .def("get_channel3", &DAPHNEEthStreamFrame::get_channel3)
    .def_static("sizeof", []() { return sizeof(DAPHNEEthStreamFrame); })
    .def("get_bytes", [](DAPHNEEthStreamFrame* fr) -> py::bytes {
      return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEEthStreamFrame)); // NOLINT reinterpret_cast
    });
} // NOLINT (defensible use of "overly long" function)

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats::python
