/**
 * @file daphneeth.cpp Python bindings for the DAPHNEEthFrame format
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEEthFrame.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

// NOLINTBEGIN(build/unsigned)

void
register_daphneeth(py::module& m)
{

  py::class_<DAPHNEEthFrame::PeakDescriptor>(m, "DAPHNEEthFramePeakDescriptor")

    .def_property(
      "num_subpeaks",
      [](DAPHNEEthFrame::PeakDescriptor& self) -> uint16_t { return self.num_subpeaks; },
      [](DAPHNEEthFrame::PeakDescriptor& self, uint16_t num_subpeaks) { self.num_subpeaks = num_subpeaks; })
    .def_property(
      "adc_integral",
      [](DAPHNEEthFrame::PeakDescriptor& self) -> uint32_t { return self.adc_integral; },
      [](DAPHNEEthFrame::PeakDescriptor& self, uint32_t adc_integral) { self.adc_integral = adc_integral; })
    .def_property(
      "adc_max",
      [](DAPHNEEthFrame::PeakDescriptor& self) -> uint16_t { return self.adc_max; },
      [](DAPHNEEthFrame::PeakDescriptor& self, uint16_t adc_max) { self.adc_max = adc_max; })
    .def_property(
      "sample_max",
      [](DAPHNEEthFrame::PeakDescriptor& self) -> uint16_t { return self.sample_max; },
      [](DAPHNEEthFrame::PeakDescriptor& self, uint16_t sample_max) { self.sample_max = sample_max; })
    .def_property(
      "samples_over_baseline",
      [](DAPHNEEthFrame::PeakDescriptor& self) -> uint16_t { return self.samples_over_baseline; },
      [](DAPHNEEthFrame::PeakDescriptor& self, uint16_t samples_over_baseline) { self.samples_over_baseline = samples_over_baseline; })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEEthFrame::PeakDescriptor::s_expected_bytes;
    });

  py::class_<DAPHNEEthFrame::PeakDescriptorData>(m, "DAPHNEEthFramePeakDescriptorData")

    .def("is_found", &DAPHNEEthFrame::PeakDescriptorData::is_found)
    .def("set_found", &DAPHNEEthFrame::PeakDescriptorData::set_found)

    .def("get_adc_integral", &DAPHNEEthFrame::PeakDescriptorData::get_adc_integral)
    .def("set_adc_integral", &DAPHNEEthFrame::PeakDescriptorData::set_adc_integral)

    .def("get_num_subpeaks", &DAPHNEEthFrame::PeakDescriptorData::get_num_subpeaks)
    .def("set_num_subpeaks", &DAPHNEEthFrame::PeakDescriptorData::set_num_subpeaks)

    .def("get_samples_over_baseline", &DAPHNEEthFrame::PeakDescriptorData::get_samples_over_baseline)
    .def("set_samples_over_baseline", &DAPHNEEthFrame::PeakDescriptorData::set_samples_over_baseline)

    .def("get_adc_max", &DAPHNEEthFrame::PeakDescriptorData::get_adc_max)
    .def("set_adc_max", &DAPHNEEthFrame::PeakDescriptorData::set_adc_max)

    .def("get_sample_max", &DAPHNEEthFrame::PeakDescriptorData::get_sample_max)
    .def("set_sample_max", &DAPHNEEthFrame::PeakDescriptorData::set_sample_max)

    .def("get_sample_start", &DAPHNEEthFrame::PeakDescriptorData::get_sample_start)
    .def("set_sample_start", &DAPHNEEthFrame::PeakDescriptorData::set_sample_start)
    
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEEthFrame::PeakDescriptorData::s_expected_bytes;
    });

  py::class_<DAPHNEEthFrame::Header>(m, "DAPHNEEthHeader")
 
    .def_property(
      "channel",
      [](DAPHNEEthFrame::Header& self) -> uint32_t { return self.channel; },
      [](DAPHNEEthFrame::Header& self, uint32_t channel) { self.channel = channel; })
    .def_property(
      "version",
      [](DAPHNEEthFrame::Header& self) -> uint32_t { return self.version; },
      [](DAPHNEEthFrame::Header& self, uint32_t version) { self.version = version; })
    .def_property(
      "trigger_sample_value",
      [](DAPHNEEthFrame::Header& self) -> uint32_t { return self.trigger_sample_value; },
      [](DAPHNEEthFrame::Header& self, uint32_t trigger_sample_value) { self.trigger_sample_value = trigger_sample_value; })
    .def_property(
      "threshold",
      [](DAPHNEEthFrame::Header& self) -> uint32_t { return self.threshold; },
      [](DAPHNEEthFrame::Header& self, uint32_t threshold) { self.threshold = threshold; })
    .def_property(
      "baseline",
      [](DAPHNEEthFrame::Header& self) -> uint32_t { return self.baseline; },
      [](DAPHNEEthFrame::Header& self, uint32_t baseline) { self.baseline = baseline; })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEEthFrame::Header::s_expected_bytes;
    });

  py::class_<DAPHNEEthFrame>(m, "DAPHNEEthFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
      auto wfp = *static_cast<DAPHNEEthFrame*>(capsule.get_pointer());
      return wfp;
    }))
    .def(py::init([](py::bytes bytes) {
      py::buffer_info info(py::buffer(bytes).request());
      auto wfp = *static_cast<DAPHNEEthFrame*>(info.ptr);
      return wfp;
    }))
    .def(
      "get_daqheader",
      [](DAPHNEEthFrame& self) -> const detdataformats::DAQEthHeader& { return self.daq_header; },
      py::return_value_policy::reference_internal)
    .def(
      "get_daphneheader",
      [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def(
      "get_header",
      [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def_property_readonly(
      "daq_header",
      [](DAPHNEEthFrame& self) -> detdataformats::DAQEthHeader& { return self.daq_header; },
      py::return_value_policy::reference_internal)
    .def_property_readonly(
      "header",
      [](DAPHNEEthFrame& self) -> DAPHNEEthFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def("get_adc", &DAPHNEEthFrame::get_adc)
    .def("set_adc", &DAPHNEEthFrame::set_adc)
    .def("get_timestamp", &DAPHNEEthFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEEthFrame::set_timestamp)
    .def("get_channel", &DAPHNEEthFrame::get_channel)
    .def("set_channel", &DAPHNEEthFrame::set_channel)
    .def("__lt__", [](const DAPHNEEthFrame& lhs, const DAPHNEEthFrame& rhs) { return lhs < rhs; })
    .def_property_readonly_static("version", [](py::object /*self*/) { return DAPHNEEthFrame::version; })
    .def_property_readonly_static("s_bits_per_adc", [](py::object /*self*/) {
      return DAPHNEEthFrame::s_bits_per_adc;
    })
    .def_property_readonly_static("s_bits_per_word", [](py::object /*self*/) {
      return DAPHNEEthFrame::s_bits_per_word;
    })
    .def_property_readonly_static("s_num_adcs", [](py::object /*self*/) { return DAPHNEEthFrame::s_num_adcs; })
    .def_property_readonly_static("s_num_adc_words", [](py::object /*self*/) {
      return DAPHNEEthFrame::s_num_adc_words;
    })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEEthFrame::s_expected_bytes;
    })
    .def_static("sizeof", []() { return sizeof(DAPHNEEthFrame); })
    .def("get_bytes", [](DAPHNEEthFrame* fr) -> py::bytes {
      return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEEthFrame)); // NOLINT reinterpret_cast
    });
} // NOLINT (defensible use of "overly long" function)

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats::python
