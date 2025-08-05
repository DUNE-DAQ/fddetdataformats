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

void
register_daphneeth(py::module& m)
{


  py::class_<DAPHNEEthFrame::Header>(m, "DAPHNEEthHeader")
    .def_property("w0",
      [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.w0;},
      [](DAPHNEEthFrame::Header& self, uint32_t w0) {self.w0 = w0;}
      )      
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


      

    // .def_property("channel",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.channel;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t channel) {self.channel = channel;}
    //   )
    // .def_property("version",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.version;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t version) {self.version = version;}
    //   )
    // // .def_property("reserved",
    //   // [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.reserved;},
    //   // [](DAPHNEEthFrame::Header& self, uint32_t reserved) {self.reserved = reserved;}
    //   // )
    // .def_property("cd",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.cd;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t cd) {self.version = cd;}
    //   )
    // .def_property("context",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.context;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t context) {self.version = context;}
    //   )
    // .def_property("ready",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.ready;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t ready) {self.ready = ready;}
    //   )
    // .def_property("calibration",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.calibration;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t calibration) {self.calibration = calibration;}
    //   )
    // .def_property("pulser",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.pulser;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t pulser) {self.pulser = pulser;}
    //   )
    // .def_property("femb_sync",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.femb_sync;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t femb_sync) {self.femb_sync = femb_sync;}
    //   )
    // .def_property("wib_sync",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.wib_sync;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t wib_sync) {self.wib_sync = wib_sync;}
    //   )
    // .def_property("lol",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.lol;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t lol) {self.lol = lol;}
    //   )
    // .def_property("link_valid",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.link_valid;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t link_valid) {self.link_valid = link_valid;}
    //   )
    // .def_property("crc_err",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.crc_err;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t crc_err) {self.crc_err = crc_err;}
    //   )
    // .def_property("colddata_timestamp_1",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.colddata_timestamp_1;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t colddata_timestamp_1) {self.lol = colddata_timestamp_1;}
    //   )
    // .def_property("colddata_timestamp_0",
    //   [](DAPHNEEthFrame::Header& self) -> uint32_t {return self.colddata_timestamp_0;},
    //   [](DAPHNEEthFrame::Header& self, uint32_t colddata_timestamp_0) {self.colddata_timestamp_0 = colddata_timestamp_0;}
    //   )
    // .def_property("extra_data",
    //   [](DAPHNEEthFrame::Header& self) -> uint64_t {return self.extra_data;},
    //   [](DAPHNEEthFrame::Header& self, uint64_t extra_data) {self.extra_data = extra_data;}
    //   )
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
    .def("get_adc", &DAPHNEEthFrame::get_adc)
    .def("set_adc", &DAPHNEEthFrame::set_adc)
    .def("get_timestamp", &DAPHNEEthFrame::get_timestamp)
    .def("set_timestamp", &DAPHNEEthFrame::set_timestamp)
    .def("get_channel", &DAPHNEEthFrame::get_channel)
    .def("set_channel", &DAPHNEEthFrame::set_channel)
    .def_static("sizeof", [](){ return sizeof(DAPHNEEthFrame); })
    .def("get_bytes",
         [](DAPHNEEthFrame* fr) -> py::bytes {
           return py::bytes(reinterpret_cast<char*>(fr), sizeof(DAPHNEEthFrame));
        })
  ;
}

} // namespace dunedaq::fddetdataformats::python
