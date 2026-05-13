cmake_minimum_required(VERSION 3.12)
project(fddetdataformats VERSION 2.7.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(detdataformats REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(logging REQUIRED)

##############################################################################

daq_add_library( LINK_LIBRARIES detdataformats::detdataformats) 

##############################################################################

daq_add_python_bindings(*.cpp ) 

##############################################################################

daq_add_unit_test(WIBEthFrame_test LINK_LIBRARIES fddetdataformats logging::logging) 
daq_add_unit_test(DAPHNEFrame_test LINK_LIBRARIES fddetdataformats)

##############################################################################

daq_install()

