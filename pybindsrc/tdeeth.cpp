/**
 * @file wibeth.cpp Python bindings for the TDEEthFrame format
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

void
register_tdeeth(py::module& m)
{


  // py::class_<TDEEthFrame::TDEHeader>(m, "TDEHeader")
  //   .def_property("channel",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.channel;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t channel) {self.channel = channel;}
  //     )
  //   .def_property("version",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.version;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t version) {self.version = version;}
  //     )
  //   // .def_property("reserved",
  //     // [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.reserved;},
  //     // [](TDEEthFrame::TDEHeader& self, uint32_t reserved) {self.reserved = reserved;}
  //     // )
  //   .def_property("cd",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.cd;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t cd) {self.version = cd;}
  //     )
  //   .def_property("context",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.context;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t context) {self.version = context;}
  //     )
  //   .def_property("ready",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.ready;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t ready) {self.ready = ready;}
  //     )
  //   .def_property("calibration",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.calibration;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t calibration) {self.calibration = calibration;}
  //     )
  //   .def_property("pulser",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.pulser;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t pulser) {self.pulser = pulser;}
  //     )
  //   .def_property("femb_sync",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.femb_sync;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t femb_sync) {self.femb_sync = femb_sync;}
  //     )
  //   .def_property("wib_sync",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.wib_sync;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t wib_sync) {self.wib_sync = wib_sync;}
  //     )
  //   .def_property("lol",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.lol;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t lol) {self.lol = lol;}
  //     )
  //   .def_property("link_valid",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.link_valid;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t link_valid) {self.link_valid = link_valid;}
  //     )
  //   .def_property("crc_err",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.crc_err;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t crc_err) {self.crc_err = crc_err;}
  //     )
  //   .def_property("colddata_timestamp_1",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.colddata_timestamp_1;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t colddata_timestamp_1) {self.lol = colddata_timestamp_1;}
  //     )
  //   .def_property("colddata_timestamp_0",
  //     [](TDEEthFrame::TDEHeader& self) -> uint32_t {return self.colddata_timestamp_0;},
  //     [](TDEEthFrame::TDEHeader& self, uint32_t colddata_timestamp_0) {self.colddata_timestamp_0 = colddata_timestamp_0;}
  //     )
  //   .def_property("extra_data",
  //     [](TDEEthFrame::TDEHeader& self) -> uint64_t {return self.extra_data;},
  //     [](TDEEthFrame::TDEHeader& self, uint64_t extra_data) {self.extra_data = extra_data;}
  //     )
  //   ;

  py::class_<TDEEthFrame::TDEEthHeader>(m, "TDEEthHeader")
    .def_property("channel", 
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t {return self.channel;},
      [](TDEEthFrame::TDEEthHeader& self, uint16_t channel) {self.channel = channel;}
      )
    .def_property("version",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t {return self.version;},
      [](TDEEthFrame::TDEEthHeader& self, uint16_t version) {self.version = version;}
      )
    .def_property("tde_header",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t {return self.tde_header;},
      [](TDEEthFrame::TDEEthHeader& self, uint16_t tde_header) {self.tde_header = tde_header;}
      )
    .def_property("tde_errors",
      [](TDEEthFrame::TDEEthHeader& self) -> uint16_t {return self.tde_errors;},
      [](TDEEthFrame::TDEEthHeader& self, uint16_t tde_errors) {self.tde_errors = tde_errors;}
      )
    .def_property("TAItime",
      [](TDEEthFrame::TDEEthHeader& self) -> uint64_t {return self.TAItime;},
      [](TDEEthFrame::TDEEthHeader& self, uint64_t TAItime) {self.TAItime = TAItime;}
      )
  ;

  
  py::class_<TDEEthFrame>(m, "TDEEthFrame", py::buffer_protocol())
    .def(py::init())
    .def(py::init([](py::capsule capsule) {
        auto tfp = *static_cast<TDEEthFrame*>(capsule.get_pointer());
        return tfp;
    } ))
    .def(py::init([](py::bytes bytes){
        py::buffer_info info(py::buffer(bytes).request());
        auto tfp = *static_cast<TDEEthFrame*>(info.ptr);
        return tfp;
    }))
    .def("get_daqheader", [](TDEEthFrame& self) -> const detdataformats::DAQEthHeader& {return self.daq_header;}, py::return_value_policy::reference_internal)
    .def("get_tdeheader", [](TDEEthFrame& self) -> const TDEEthFrame::TDEEthHeader& {return self.header;}, py::return_value_policy::reference_internal)
    .def("get_adc", &TDEEthFrame::get_adc)
    .def("set_adc", &TDEEthFrame::set_adc)
    .def("get_timestamp", &TDEEthFrame::get_timestamp)
    .def("set_timestamp", &TDEEthFrame::set_timestamp)
    .def("get_channel", &TDEEthFrame::get_channel)
    .def("set_channel", &TDEEthFrame::set_channel)
    .def_static("sizeof", [](){ return sizeof(TDEEthFrame); })
    .def("get_bytes",
         [](TDEEthFrame* fr) -> py::bytes {
           return py::bytes(reinterpret_cast<char*>(fr), sizeof(TDEEthFrame));
        })
  ;
}

} // namespace dunedaq::fddetdataformats::python
