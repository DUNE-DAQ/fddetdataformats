/**
 * @file daphne.cpp Python bindings for the DAPHNEFrame format
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEFrame.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

// NOLINTBEGIN(build/unsigned)

void
register_daphne(py::module& m)
{

  py::class_<DAPHNEFrame>(m, "DAPHNEFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
      auto wfp = *static_cast<DAPHNEFrame*>(capsule.get_pointer());
      return wfp;
    }))
    .def(py::init([](py::bytes bytes) {
      py::buffer_info info(py::buffer(bytes).request());
      auto wfp = *static_cast<DAPHNEFrame*>(info.ptr);
      return wfp;
    }))
    .def(
      "get_daqheader",
      [](DAPHNEFrame& self) -> const detdataformats::DAQHeader& { return self.daq_header; },
      py::return_value_policy::reference_internal)
    .def(
      "get_header",
      [](DAPHNEFrame& self) -> const DAPHNEFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def_property_readonly(
      "daq_header",
      [](DAPHNEFrame& self) -> detdataformats::DAQHeader& { return self.daq_header; },
      py::return_value_policy::reference_internal)
    .def_property_readonly(
      "header",
      [](DAPHNEFrame& self) -> DAPHNEFrame::Header& { return self.header; },
      py::return_value_policy::reference_internal)
    .def("peaks_data",
      [](DAPHNEFrame& self) -> DAPHNEFrame::PeakDescriptorData& { return self.peaks_data; },
      py::return_value_policy::reference_internal)
    .def("get_adc", static_cast<uint16_t (DAPHNEFrame::*)(const int) const>(&DAPHNEFrame::get_adc))
    .def("set_adc", &DAPHNEFrame::set_adc)
    .def("get_timestamp", &DAPHNEFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEFrame::set_timestamp)
    .def("get_channel", &DAPHNEFrame::get_channel)
    .def("set_channel", &DAPHNEFrame::set_channel)
    .def("__lt__", [](const DAPHNEFrame& lhs, const DAPHNEFrame& rhs) { return lhs < rhs; })
    .def_property_readonly_static("version", [](py::object /*self*/) { return DAPHNEFrame::version; })
    .def_property_readonly_static("s_bits_per_adc", [](py::object /*self*/) {
      return DAPHNEFrame::s_bits_per_adc;
    })
    .def_property_readonly_static("s_bits_per_word", [](py::object /*self*/) {
      return DAPHNEFrame::s_bits_per_word;
    })
    .def_property_readonly_static("s_num_adcs", [](py::object /*self*/) { return DAPHNEFrame::s_num_adcs; })
    .def_property_readonly_static("s_num_adc_words", [](py::object /*self*/) {
      return DAPHNEFrame::s_num_adc_words;
    })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEFrame::s_expected_bytes;
    })
    .def_static("sizeof", []() { return sizeof(DAPHNEFrame); })
    .def("get_bytes", [](DAPHNEFrame* fr) -> py::bytes {
      return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEFrame)); // NOLINT
    });

  py::class_<DAPHNEFrame::Header>(m, "DAPHNEFrameHeader")
    .def_property(
      "channel",
      [](DAPHNEFrame::Header& self) -> uint8_t { return self.channel; },
      [](DAPHNEFrame::Header& self, uint8_t channel) { self.channel = channel; })
    .def_property(
      "algorithm_id",
      [](DAPHNEFrame::Header& self) -> uint8_t { return self.algorithm_id; },
      [](DAPHNEFrame::Header& self, uint8_t algorithm_id) { self.algorithm_id = algorithm_id; })
    .def_property(
      "r1",
      [](DAPHNEFrame::Header& self) -> uint8_t { return self.r1; },
      [](DAPHNEFrame::Header& self, uint8_t r1) { self.r1 = r1; })
    .def_property(
      "trigger_sample_value",
      [](DAPHNEFrame::Header& self) -> uint16_t { return self.trigger_sample_value; },
      [](DAPHNEFrame::Header& self, uint16_t tsv) { self.trigger_sample_value = tsv; })
    .def_property(
      "threshold",
      [](DAPHNEFrame::Header& self) -> uint16_t { return self.threshold; },
      [](DAPHNEFrame::Header& self, uint16_t threshold) { self.threshold = threshold; })
    .def_property(
      "baseline",
      [](DAPHNEFrame::Header& self) -> uint16_t { return self.baseline; },
      [](DAPHNEFrame::Header& self, uint16_t baseline) { self.baseline = baseline; })
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return sizeof(DAPHNEFrame::Header);
    });

  py::class_<DAPHNEFrame::PeakDescriptorData>(m, "DAPHNEFramePeakDescriptorData")
    .def_property_readonly_static("s_expected_bytes", [](py::object /*self*/) {
      return DAPHNEFrame::PeakDescriptorData::s_expected_bytes;
    })
    .def_property_readonly_static("max_peaks", [](py::object /*self*/) {
      return DAPHNEFrame::PeakDescriptorData::max_peaks;
    })
    .def("is_found", &DAPHNEFrame::PeakDescriptorData::is_found)
    .def("set_found", &DAPHNEFrame::PeakDescriptorData::set_found)

    .def("get_adc_integral", &DAPHNEFrame::PeakDescriptorData::get_adc_integral)
    .def("set_adc_integral", &DAPHNEFrame::PeakDescriptorData::set_adc_integral)

    .def("get_num_subpeaks", &DAPHNEFrame::PeakDescriptorData::get_num_subpeaks)
    .def("set_num_subpeaks", &DAPHNEFrame::PeakDescriptorData::set_num_subpeaks)

    .def("get_samples_over_baseline", &DAPHNEFrame::PeakDescriptorData::get_samples_over_baseline)
    .def("set_samples_over_baseline", &DAPHNEFrame::PeakDescriptorData::set_samples_over_baseline)

    .def("get_sample_max", &DAPHNEFrame::PeakDescriptorData::get_sample_max)
    .def("set_sample_max", &DAPHNEFrame::PeakDescriptorData::set_sample_max)

    .def("get_adc_max", &DAPHNEFrame::PeakDescriptorData::get_adc_max)
    .def("set_adc_max", &DAPHNEFrame::PeakDescriptorData::set_adc_max)

    .def("get_sample_start", &DAPHNEFrame::PeakDescriptorData::get_sample_start)
    .def("set_sample_start", &DAPHNEFrame::PeakDescriptorData::set_sample_start)

    .def_property(
      "num_subpeaks_0",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_0; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_0 = val; })
    .def_property(
      "adc_integral_0",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_0; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_0 = val; })
    .def_property(
      "found_0",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.found_0; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.found_0 = val; })
    .def_property(
      "adc_max_0",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_0; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_0 = val; })
    .def_property(
      "sample_max_0",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_0; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_0 = val; })
    .def_property(
      "samples_over_baseline_0",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_0; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_0 = val; })

    .def_property(
      "num_subpeaks_1",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_1; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_1 = val; })
    .def_property(
      "adc_integral_1",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_1; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_1 = val; })
    .def_property(
      "found_1",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.found_1; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.found_1 = val; })
    .def_property(
      "adc_max_1",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_1; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_1 = val; })
    .def_property(
      "sample_max_1",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_1; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_1 = val; })
    .def_property(
      "samples_over_baseline_1",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_1; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_1 = val; })

    .def_property(
      "num_subpeaks_2",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_2; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_2 = val; })
    .def_property(
      "adc_integral_2",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_2; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_2 = val; })
    .def_property(
      "found_2",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.found_2; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.found_2 = val; })
    .def_property(
      "adc_max_2",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_2; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_2 = val; })
    .def_property(
      "sample_max_2",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_2; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_2 = val; })
    .def_property(
      "samples_over_baseline_2",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_2; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_2 = val; })

    .def_property(
      "num_subpeaks_3",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_3; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_3 = val; })
    .def_property(
      "adc_integral_3",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_3; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_3 = val; })
    .def_property(
      "found_3",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.found_3; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.found_3 = val; })
    .def_property(
      "adc_max_3",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_3; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_3 = val; })
    .def_property(
      "sample_max_3",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_3; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_3 = val; })
    .def_property(
      "samples_over_baseline_3",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_3; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_3 = val; })

    .def_property(
      "num_subpeaks_4",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_4; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_4 = val; })
    .def_property(
      "adc_integral_4",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_4; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_4 = val; })
    .def_property(
      "found_4",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint8_t { return self.found_4; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint8_t val) { self.found_4 = val; })
    .def_property(
      "adc_max_4",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_4; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_4 = val; })
    .def_property(
      "sample_max_4",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_4; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_4 = val; })
    .def_property(
      "samples_over_baseline_4",
      [](DAPHNEFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_4; },
      [](DAPHNEFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_4 = val; });

} // NOLINT function length

// NOLINTEND(build/unsigned)

} // namespace dunedaq::fddetdataformats::python
