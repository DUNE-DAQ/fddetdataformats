#ifndef FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_
#define FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_

#include <pybind11/pybind11.h>

namespace dunedaq::fddetdataformats::python {

  extern void register_wib(pybind11::module &);
  extern void register_daphne(pybind11::module &);
  extern void register_ssp(pybind11::module &);
  extern void register_wibeth(pybind11::module &);    
  extern void register_tde(pybind11::module &);

}  // namespace dunedaq::fddetdataformats::python

#endif // FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_
