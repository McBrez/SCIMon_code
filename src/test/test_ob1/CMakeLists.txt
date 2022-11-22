cmake_minimum_required(VERSION 3.10)
project(test_ob1)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/isx3
    ${INCLUDE_DIR}/Devices/elveflow_ob1
    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/ftd2xx/include
    ${3RDPARTY_DIR}/catch2/single_include
)

link_directories(
    ${3RDPARTY_DIR}/ftd2xx/lib/i386
)

add_executable(test_ob1

    test_ob1.cpp

    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/device_configuration.hpp
    ${INCLUDE_DIR}/Devices/device_message.hpp
    ${INCLUDE_DIR}/Devices/read_device_message.hpp
    ${INCLUDE_DIR}/Devices/read_spectrum_message.hpp
    ${INCLUDE_DIR}/Devices/isx3/ack_message_isx3.hpp
    ${INCLUDE_DIR}/Devices/isx3/device_isx3.hpp
    ${INCLUDE_DIR}/Devices/isx3/init_message_isx3.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_constants.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1_win.hpp
    ${INCLUDE_DIR}/Devices/write_device_message.hpp
    ${INCLUDE_DIR}/Devices/config_device_message.hpp
    ${INCLUDE_DIR}/Devices/config_is_message.hpp
    ${INCLUDE_DIR}/Devices/init_device_message.hpp

    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/device_configuration.cpp
    ${SOURCE_DIR}/Devices/device_message.cpp
    ${SOURCE_DIR}/Devices/isx3/ack_message_isx3.cpp
    ${SOURCE_DIR}/Devices/isx3/device_isx3.cpp    
    ${SOURCE_DIR}/Devices/isx3/init_message_isx3.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1_win.cpp
    ${SOURCE_DIR}/Devices/read_spectrum_message.cpp
    ${SOURCE_DIR}/Devices/config_is_message.cpp
    ${SOURCE_DIR}/Devices/read_device_message.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
    ${3RDPARTY_DIR}/ftd2xx/include/ftd2xx.h
)

if (WIN32)
    target_sources(test_ob1 PUBLIC
        ${INCLUDE_DIR}/Devices/isx3/device_isx3_win.hpp

        ${SOURCE_DIR}/Devices/isx3/device_isx3_win.cpp
    )
endif (WIN32)
if (UNIX)
    
endif (UNIX)



target_link_libraries(test_ob1 
    ftd2xx
)