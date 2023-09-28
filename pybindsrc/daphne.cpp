/**
 * @file daphne.cpp Python bindings for the DAPHNEFrame format
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/DAPHNEFrame.hpp"
#include "fddetdataformats/DAPHNEStreamFrame.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

void
register_daphne(py::module& m)
{

  py::class_<DAPHNEFrame>(m, "DAPHNEFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
        auto wfp = *static_cast<DAPHNEFrame*>(capsule.get_pointer());
        return wfp;
    } ))
    .def(py::init([](py::bytes bytes){
      py::buffer_info info(py::buffer(bytes).request());
      auto wfp = *static_cast<DAPHNEFrame*>(info.ptr);
      return wfp;
    }))
    .def("get_adc", static_cast<uint16_t (DAPHNEFrame::*)(const int) const>(&DAPHNEFrame::get_adc))
    .def("get_timestamp", &DAPHNEFrame::get_timestamp)
    .def("get_channel", &DAPHNEFrame::get_channel)
    .def("get_daqheader", [](DAPHNEFrame& self) -> const detdataformats::DAQHeader& {return self.daq_header;}, py::return_value_policy::reference_internal)
    .def("get_header",    [](DAPHNEFrame& self) -> const DAPHNEFrame::Header& {return self.header;}, py::return_value_policy::reference_internal)
    .def_static("sizeof", [](){ return sizeof(DAPHNEFrame); })
  ;

  
  py::class_<DAPHNEFrame::Header>(m, "DAPHNEHeader")
    .def_property_readonly("channel", [](DAPHNEFrame::Header& self) -> uint32_t {return self.channel;})
    .def_property_readonly("trigger_sample_value", [](DAPHNEFrame::Header& self) -> uint32_t {return self.trigger_sample_value;})
    .def_property_readonly("threshold", [](DAPHNEFrame::Header& self) -> uint32_t {return self.threshold;})
    .def_property_readonly("baseline", [](DAPHNEFrame::Header& self) -> uint32_t {return self.baseline;})
  ;

  py::class_<DAPHNEStreamFrame::Header>(m, "DAPHNEStreamHeader")
    .def_property("channel_0", 
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_0; }, 
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_0) { self.channel_0 = channel_0; } 
      )
    .def_property("channel_1", 
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_1; }, 
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_1) { self.channel_1 = channel_1; } 
      )
    .def_property("channel_2", 
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_2; }, 
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_2) { self.channel_2 = channel_2; } 
      )
    .def_property("channel_3", 
      [](DAPHNEStreamFrame::Header& self) -> uint32_t { return self.channel_3; }, 
      [](DAPHNEStreamFrame::Header& self, uint32_t channel_3) { self.channel_3 = channel_3; } 
      )
  ;


  py::class_<DAPHNEStreamFrame>(m, "DAPHNEStreamFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
        auto wfp = *static_cast<DAPHNEStreamFrame*>(capsule.get_pointer());
        return wfp;
    } ))
    .def("get_daqheader", [](DAPHNEStreamFrame& self) -> const detdataformats::DAQHeader& {return self.daq_header;}, py::return_value_policy::reference_internal)
    .def("get_header", [](DAPHNEStreamFrame& self) -> const DAPHNEStreamFrame::Header& {return self.header;}, py::return_value_policy::reference_internal)
    //.def("get_trailer", [](DAPHNEStreamFrame& self) -> const DAPHNEStreamFrame::Trailer& {return self.trailer;}, py::return_value_policy::reference_internal)
    .def("get_timestamp", &DAPHNEStreamFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEStreamFrame::set_timestamp)
    .def("get_adc", &DAPHNEStreamFrame::get_adc)
    .def("set_adc", &DAPHNEStreamFrame::set_adc)
    .def_static("sizeof", [](){ return sizeof(DAPHNEStreamFrame); })
    .def("get_bytes",
         [](DAPHNEStreamFrame* fr) -> py::bytes {
           return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEStreamFrame));
        }
    )
  ;
}

} // namespace dunedaq::fddetdataformats::python
