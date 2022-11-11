cmake_minimum_required(VERSION 3.10)
project(latency_tester)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Control
    ${3RDPARTY_DIR}/easyloggingpp/src
)

add_executable(latency_tester

    latency_tester.cpp

    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/device_configuration.hpp
    ${INCLUDE_DIR}/Devices/device_message.hpp
    ${INCLUDE_DIR}/Devices/message_impedance_spectrometer.hpp
    ${INCLUDE_DIR}/Devices/impedance_spectrometer.hpp
    ${INCLUDE_DIR}/Control/main_control.hpp


    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/device_configuration.cpp
    ${SOURCE_DIR}/Devices/device_message.cpp
    ${SOURCE_DIR}/Devices/message_impedance_spectrometer.cpp
    ${SOURCE_DIR}/Devices/impedance_spectrometer.cpp
    ${SOURCE_DIR}/Control/main_control.cpp

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h

)