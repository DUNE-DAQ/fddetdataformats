#ifndef FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_
#define FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_

#include <pybind11/pybind11.h>

namespace dunedaq::fddetdataformats {

  namespace wib::python {
    extern void register_wib(pybind11::module &);
  } // namespace wib::python

  namespace daphne::python {
    extern void register_daphne(pybind11::module &);
  }  // namespace daphne::python

  namespace ssp::python {
    extern void register_ssp(pybind11::module &);
  }  // namespace ssp::python

  namespace wibeth::python {
    extern void register_wibeth(pybind11::module &);    
  } // namespace wibeth::python

  namespace tde::python {
    extern void register_tde(pybind11::module &);
  } // namespace tde::python

}  // namespace dunedaq::fddetdataformats

#endif // FDDETDATAFORMATS_PYBINDSRC_SUBMODULES_HPP_
