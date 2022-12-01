cmake_minimum_required(VERSION 3.10)
project(test_ob1)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/elveflow_ob1
    ${INCLUDE_DIR}/Messages
    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/ftd2xx/include
    ${3RDPARTY_DIR}/catch2/single_include
)

link_directories(
    ${3RDPARTY_DIR}/ftd2xx/lib/amd64
)

add_executable(test_ob1

    test_ob1.cpp

    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/device_configuration.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1_win.hpp
    ${INCLUDE_DIR}/Messages/device_message.hpp
    ${INCLUDE_DIR}/Messages/read_device_message.hpp
    ${INCLUDE_DIR}/Messages/write_device_message.hpp
    ${INCLUDE_DIR}/Messages/config_device_message.hpp
    ${INCLUDE_DIR}/Messages/init_device_message.hpp

    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/device_configuration.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1_win.cpp
    ${SOURCE_DIR}/Messages/device_message.cpp
    ${SOURCE_DIR}/Messages/read_device_message.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
    ${3RDPARTY_DIR}/ftd2xx/include/ftd2xx.h
)

# Enforce C++20
set_property(TARGET test_ob1 PROPERTY CXX_STANDARD 20)

if (WIN32)
    target_sources(test_ob1 PUBLIC)
endif (WIN32)
if (UNIX)
    
endif (UNIX)

target_link_libraries(test_ob1 
    ftd2xx
)