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

void
register_daphneeth(py::module& m)
{
  py::class_<DAPHNEEthFrame::Header>(m, "DAPHNEEthHeader")
    .def_property("w1",
      [](DAPHNEEthFrame::Header& self) -> uint64_t { return self.get_packed_peak_word(0); },
      [](DAPHNEEthFrame::Header& self, uint64_t w1) { self.set_packed_peak_word(w1, 0); }
      )
    .def_property("w2",
      [](DAPHNEEthFrame::Header& self) -> uint64_t { return self.get_packed_peak_word(1); },
      [](DAPHNEEthFrame::Header& self, uint64_t w2) { self.set_packed_peak_word(w2, 1); }
      )
    .def_property("w3",
      [](DAPHNEEthFrame::Header& self) -> uint64_t { return self.get_packed_peak_word(2); },
      [](DAPHNEEthFrame::Header& self, uint64_t w3) { self.set_packed_peak_word(w3, 2); }
      )
    .def_property("w4",
      [](DAPHNEEthFrame::Header& self) -> uint64_t { return self.get_packed_peak_word(3); },
      [](DAPHNEEthFrame::Header& self, uint64_t w4) { self.set_packed_peak_word(w4, 3); }
      )
    .def_property("w5",
      [](DAPHNEEthFrame::Header& self) -> uint64_t { return self.get_packed_peak_word(4); },
      [](DAPHNEEthFrame::Header& self, uint64_t w5) { self.set_packed_peak_word(w5, 4); }
      )
    .def_property("w6",
      [](DAPHNEEthFrame::Header& self) -> uint64_t { return self.get_packed_peak_word(5); },
      [](DAPHNEEthFrame::Header& self, uint64_t w6) { self.set_packed_peak_word(w6, 5); }
      )
    .def_property("channel",
      [](DAPHNEEthFrame::Header& self) -> uint8_t { return self.channel; },
      [](DAPHNEEthFrame::Header& self, uint8_t channel) { self.channel = channel; }
      )
    .def_property("version",
      [](DAPHNEEthFrame::Header& self) -> uint8_t { return self.version; },
      [](DAPHNEEthFrame::Header& self, uint8_t version) { self.version = version; }
      )
    .def_property("trigger_sample_value",
      [](DAPHNEEthFrame::Header& self) -> uint16_t { return self.trig_sample; },
      [](DAPHNEEthFrame::Header& self, uint16_t trig_sample) { self.trig_sample = trig_sample; }
      )
    .def_property("threshold",
      [](DAPHNEEthFrame::Header& self) -> uint16_t { return self.threshold; },
      [](DAPHNEEthFrame::Header& self, uint16_t threshold) { self.threshold = threshold; }
      )
    .def_property("baseline",
      [](DAPHNEEthFrame::Header& self) -> uint16_t { return self.baseline; },
      [](DAPHNEEthFrame::Header& self, uint16_t baseline) { self.baseline = baseline; }
      )
  ;

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

    .def_property("num_subpeaks_0",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_0; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_0 = val; }
      )
    .def_property("adc_integral_0",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_0; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_0 = val; }
      )
    .def_property("found_0",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.found_0; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.found_0 = val; }
      )
    .def_property("adc_max_0",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_0; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_0 = val; }
      )
    .def_property("sample_max_0",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_0; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_0 = val; }
      )
    .def_property("samples_over_baseline_0",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_0; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_0 = val; }
      )

    .def_property("num_subpeaks_1",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_1; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_1 = val; }
      )
    .def_property("adc_integral_1",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_1; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_1 = val; }
      )
    .def_property("found_1",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.found_1; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.found_1 = val; }
      )
    .def_property("adc_max_1",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_1; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_1 = val; }
      )
    .def_property("sample_max_1",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_1; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_1 = val; }
      )
    .def_property("samples_over_baseline_1",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_1; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_1 = val; }
      )

    .def_property("num_subpeaks_2",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_2; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_2 = val; }
      )
    .def_property("adc_integral_2",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_2; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_2 = val; }
      )
    .def_property("found_2",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.found_2; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.found_2 = val; }
      )
    .def_property("adc_max_2",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_2; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_2 = val; }
      )
    .def_property("sample_max_2",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_2; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_2 = val; }
      )
    .def_property("samples_over_baseline_2",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_2; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_2 = val; }
      )

    .def_property("num_subpeaks_3",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_3; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_3 = val; }
      )
    .def_property("adc_integral_3",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_3; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_3 = val; }
      )
    .def_property("found_3",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.found_3; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.found_3 = val; }
      )
    .def_property("adc_max_3",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_3; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_3 = val; }
      )
    .def_property("sample_max_3",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_3; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_3 = val; }
      )
    .def_property("samples_over_baseline_3",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_3; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_3 = val; }
      )

    .def_property("num_subpeaks_4",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.num_subpeaks_4; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.num_subpeaks_4 = val; }
      )
    .def_property("adc_integral_4",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint32_t { return self.adc_integral_4; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint32_t val) { self.adc_integral_4 = val; }
      )
    .def_property("found_4",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint8_t { return self.found_4; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint8_t val) { self.found_4 = val; }
      )
    .def_property("adc_max_4",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.adc_max_4; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.adc_max_4 = val; }
      )
    .def_property("sample_max_4",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.sample_max_4; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.sample_max_4 = val; }
      )
    .def_property("samples_over_baseline_4",
      [](DAPHNEEthFrame::PeakDescriptorData& self) -> uint16_t { return self.samples_over_baseline_4; },
      [](DAPHNEEthFrame::PeakDescriptorData& self, uint16_t val) { self.samples_over_baseline_4 = val; }
      )
  ;

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
    .def("get_daqheader", [](DAPHNEEthFrame& self) -> const detdataformats::DAQEthHeader& { return self.daq_header; }, py::return_value_policy::reference_internal)
    .def("get_daphneheader", [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::Header& { return self.header; }, py::return_value_policy::reference_internal)
    .def("get_header", [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::Header& { return self.header; }, py::return_value_policy::reference_internal)
    .def("get_peaks_data", [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::PeakDescriptorData& { return self.get_peaks_data(); }, py::return_value_policy::reference_internal)
    .def("get_adc", &DAPHNEEthFrame::get_adc)
    .def("set_adc", &DAPHNEEthFrame::set_adc)
    .def("get_timestamp", &DAPHNEEthFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEEthFrame::set_timestamp)
    .def("get_channel", &DAPHNEEthFrame::get_channel)
    .def("set_channel", &DAPHNEEthFrame::set_channel)
    .def_static("sizeof", []() { return sizeof(DAPHNEEthFrame); })
    .def("get_bytes",
         [](DAPHNEEthFrame* fr) -> py::bytes {
           return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEEthFrame));
         })
  ;
}

} // namespace dunedaq::fddetdataformats::python
