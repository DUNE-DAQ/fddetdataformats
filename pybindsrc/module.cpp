/**
 * @file module.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "submodules.hpp"

namespace py = pybind11;

namespace dunedaq::fddetdataformats::python {

PYBIND11_MODULE(_daq_fddetdataformats_py, m)
{

  m.doc() = "C++ implementation of the fddetdataformats modules";

  py::module_ wib_module = m.def_submodule("wib");
  wib::python::register_wib(wib_module);

  py::module_ daphne_module = m.def_submodule("daphne");
  daphne::python::register_daphne(daphne_module);

  py::module_ ssp_module = m.def_submodule("ssp");
  ssp::python::register_ssp(ssp_module);

  py::module_ wibeth_module = m.def_submodule("wibeth");
  wibeth::python::register_wibeth(wibeth_module);

  py::module_ tde_module = m.def_submodule("tde");
  tde::python::register_tde(tde_module);
}

} // namespace dunedaq::fddetdataformats::python
