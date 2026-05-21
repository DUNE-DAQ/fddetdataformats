/**
 * @file wibeth.cpp Python bindings for the DAPHNEEthFrame format
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


  py::class_<DAPHNEEthFrame::Header>(m, "DAPHNEEthHeader")
    // .def_property("w0",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w0;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t w0) {self.w0 = w0;}
    //   )      
    .def_property("w1",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w1;},
      [](DAPHNEEthFrame::Header& self, uint32_t w1) {self.w1 = w1;}
      )
    .def_property("w2",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w2;},
      [](DAPHNEEthFrame::Header& self, uint32_t w2) {self.w2 = w2;}
      )
    .def_property("w3",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w3;},
      [](DAPHNEEthFrame::Header& self, uint32_t w3) {self.w3 = w3;}
      )
    .def_property("w4",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w4;},
      [](DAPHNEEthFrame::Header& self, uint32_t w4) {self.w4 = w4;}
      )
    .def_property("w5",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w5;},
      [](DAPHNEEthFrame::Header& self, uint32_t w5) {self.w5 = w5;}
      )
    .def_property("w6",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w6;},
      [](DAPHNEEthFrame::Header& self, uint32_t w6) {self.w6 = w6;}
      )
    .def_property("channel",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.channel;},
      [](DAPHNEEthFrame::Header& self, uint32_t channel) {self.channel = channel;}
      )
    .def_property("version",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.version;},
      [](DAPHNEEthFrame::Header& self, uint32_t version) {self.version = version;}
      )
    .def_property("trigger_sample_value",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.trig_sample;},
      [](DAPHNEEthFrame::Header& self, uint32_t trig_sample) {self.trig_sample = trig_sample;}
      )
    .def_property("threshold",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.threshold;},
      [](DAPHNEEthFrame::Header& self, uint32_t threshold) {self.threshold = threshold;}
      )
    .def_property("baseline",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.baseline;},
      [](DAPHNEEthFrame::Header& self, uint32_t baseline) {self.baseline = baseline;}
      )
    ;

  py::class_<DAPHNEEthFrame>(m, "DAPHNEEthFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
        auto wfp = *static_cast<DAPHNEEthFrame*>(capsule.get_pointer());
        return wfp;
    } ))
    .def(py::init([](py::bytes bytes){
        py::buffer_info info(py::buffer(bytes).request());
        auto wfp = *static_cast<DAPHNEEthFrame*>(info.ptr);
        return wfp;
    }))
    .def("get_daqheader", [](DAPHNEEthFrame& self) -> const detdataformats::DAQEthHeader& {return self.daq_header;}, py::return_value_policy::reference_internal)
    .def("get_daphneheader", [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::Header& {return self.header;}, py::return_value_policy::reference_internal)
    .def("get_header", [](DAPHNEEthFrame& self) -> const DAPHNEEthFrame::Header& {return self.header;}, py::return_value_policy::reference_internal)
    .def("get_adc", &DAPHNEEthFrame::get_adc)
    .def("set_adc", &DAPHNEEthFrame::set_adc)
    .def("get_timestamp", &DAPHNEEthFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEEthFrame::set_timestamp)
    .def("get_channel", &DAPHNEEthFrame::get_channel)
    .def("set_channel", &DAPHNEEthFrame::set_channel)
    .def_static("sizeof", [](){ return sizeof(DAPHNEEthFrame); })
    .def("get_bytes",
         [](DAPHNEEthFrame* fr) -> py::bytes {
           return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEEthFrame)); // NOLINT reinterpret_cast
        })
  ;
}

// NOLINTEND(build/unsigned) 
  
} // namespace dunedaq::fddetdataformats::python
