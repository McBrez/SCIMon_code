cmake_minimum_required(VERSION 3.10)
project(test_isx3)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/isx3
    ${INCLUDE_DIR}/Messages
    ${INCLUDE_DIR}/Messages/isx3
    ${INCLUDE_DIR}/Utility
    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/ftd2xx/include
    ${3RDPARTY_DIR}/catch2/single_include
)

link_directories(
    ${3RDPARTY_DIR}/ftd2xx/lib/amd64
)

add_executable(test_isx3

    test_isx3.cpp
    isx3_software_mocker.cpp
    isx3_software_mocker.hpp
    
    ${INCLUDE_DIR}/Messages/device_message.hpp
    ${INCLUDE_DIR}/Messages/read_device_message.hpp
    ${INCLUDE_DIR}/Messages/read_spectrum_message.hpp
    ${INCLUDE_DIR}/Messages/write_device_message.hpp
    ${INCLUDE_DIR}/Messages/config_device_message.hpp
    ${INCLUDE_DIR}/Messages/config_is_message.hpp
    ${INCLUDE_DIR}/Messages/init_device_message.hpp   
    ${INCLUDE_DIR}/Messages/device_status_message.hpp
    ${INCLUDE_DIR}/Messages/isx3/init_message_isx3.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_constants.hpp
    ${INCLUDE_DIR}/Devices/isx3/device_isx3.hpp
    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/device_configuration.hpp
    ${INCLUDE_DIR}/Utility/utility.hpp

    ${SOURCE_DIR}/Messages/device_message.cpp
    ${SOURCE_DIR}/Messages/isx3/init_message_isx3.cpp
    ${SOURCE_DIR}/Messages/config_device_message.cpp
    ${SOURCE_DIR}/Messages/read_spectrum_message.cpp
    ${SOURCE_DIR}/Messages/config_is_message.cpp
    ${SOURCE_DIR}/Messages/read_device_message.cpp
    ${SOURCE_DIR}/Messages/device_status_message.cpp
    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/device_configuration.cpp
    ${SOURCE_DIR}/Devices/isx3/device_isx3.cpp
    ${SOURCE_DIR}/Utility/utility.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
    ${3RDPARTY_DIR}/ftd2xx/include/ftd2xx.h
)

# Enforce C++20
set_property(TARGET test_isx3 PROPERTY CXX_STANDARD 20)

target_link_libraries(test_isx3 
    ftd2xx
)

if (WIN32)
    target_link_libraries(test_isx3
        Ws2_32
    )
    target_sources(test_isx3 PRIVATE
        ${INCLUDE_DIR}/Devices/isx3/device_isx3_win.hpp

        ${SOURCE_DIR}/Devices/isx3/device_isx3_win.cpp
    )
endif (WIN32)
if (UNIX)
    
endif (UNIX)