/**
 * @file module.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "registrators.hpp"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace dunedaq::fddetdataformats::python {

PYBIND11_MODULE(_daq_fddetdataformats_py, m)
{

  m.doc() = "C++ implementation of the fddetdataformats modules";

  register_wibeth(m);
  register_daphne(m);
  register_daphneeth(m);
  register_daphneethstream(m);
  register_tdeeth(m);
}

} // namespace dunedaq::fddetdataformats::python
