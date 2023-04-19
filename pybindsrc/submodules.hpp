#ifndef FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_
#define FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_

#include <pybind11/pybind11.h>

namespace dunedaq::fddetdataformats {

  namespace wibeth::python {
    extern void register_wibeth(pybind11::module &);    
  } // namespace wibeth::python

}  // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_
