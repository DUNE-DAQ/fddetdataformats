/**
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "fddetdataformats/WIBEthFrame.hpp"

namespace dunedaq {
namespace fddetdataformats {

const uint32_t WIBEthFrame::s_hashcode_for_wibeth_datatype = WIBEthFrame::generate_datatype_hashcode();

} // namespace fddetdataformats
} // namespace dunedaq
