cmake_minimum_required(VERSION 3.12)
project(fddetdataformats VERSION 2.6.0)

# Require C++20 for concepts support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(detdataformats REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

##############################################################################

daq_add_library( LINK_LIBRARIES detdataformats::detdataformats) 

##############################################################################

daq_add_python_bindings(*.cpp ) 

##############################################################################

daq_add_unit_test(WIBEthFrame_test LINK_LIBRARIES fddetdataformats) 
daq_add_unit_test(WIB2Frame_test   LINK_LIBRARIES fddetdataformats)
daq_add_unit_test(WIBFrame_test    LINK_LIBRARIES fddetdataformats)
daq_add_unit_test(TDE16Frame_test  LINK_LIBRARIES fddetdataformats)
daq_add_unit_test(DAPHNEFrame_test LINK_LIBRARIES fddetdataformats)
daq_add_unit_test(Concepts_test    LINK_LIBRARIES fddetdataformats)

##############################################################################

daq_install()

