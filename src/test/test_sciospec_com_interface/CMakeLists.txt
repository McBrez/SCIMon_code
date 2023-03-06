cmake_minimum_required(VERSION 3.10)
project(test_sciospec_com_interface)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/isx3
    ${INCLUDE_DIR}/Messages

    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/catch2/single_include
)

link_directories(
    ${3RDPARTY_DIR}/ftd2xx/lib/amd64
)

add_executable(test_sciospec_com_interface

    test_sciospec_com_interface.cpp
    
    ${INCLUDE_DIR}/Messages/device_message.hpp
    ${INCLUDE_DIR}/Devices/isx3/com_interface_codec.hpp

    ${SOURCE_DIR}/Messages/device_message.cpp
    ${SOURCE_DIR}/Devices/isx3/com_interface_codec.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
)

# Enforce C++20
set_property(TARGET test_sciospec_com_interface PROPERTY CXX_STANDARD 20)
