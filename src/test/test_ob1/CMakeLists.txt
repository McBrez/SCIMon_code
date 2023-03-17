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
    ${INCLUDE_DIR}/Messages/elveflow_ob1
    ${INCLUDE_DIR}/Utilities

    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/elveflow/x64
    ${3RDPARTY_DIR}/catch2/single_include
)

add_executable(test_ob1

    test_ob1.cpp

    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/configuration_payload.hpp
    ${INCLUDE_DIR}/Devices/init_payload.hpp
    ${INCLUDE_DIR}/Devices/read_payload.hpp
    ${INCLUDE_DIR}/Devices/status_payload.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/read_payload_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1_win.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_constants.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/init_payload_ob1.hpp
    ${INCLUDE_DIR}/Messages/message_interface.hpp
    ${INCLUDE_DIR}/Messages/user_id.hpp
    ${INCLUDE_DIR}/Messages/device_message.hpp
    ${INCLUDE_DIR}/Messages/read_device_message.hpp
    ${INCLUDE_DIR}/Messages/write_device_message.hpp
    ${INCLUDE_DIR}/Messages/config_device_message.hpp
    ${INCLUDE_DIR}/Messages/init_device_message.hpp
    ${INCLUDE_DIR}/Messages/elveflow_ob1/write_message_ob1.hpp
    ${INCLUDE_DIR}/Utilities/utilities.hpp

    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/status_payload.cpp
    ${SOURCE_DIR}/Devices/configuration_payload.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1_win.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/init_payload_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/read_payload_ob1.cpp
    ${SOURCE_DIR}/Messages/message_interface.cpp
    ${SOURCE_DIR}/Messages/device_message.cpp
    ${SOURCE_DIR}/Messages/user_id.cpp
    ${SOURCE_DIR}/Messages/init_device_message.cpp
    ${SOURCE_DIR}/Messages/read_device_message.cpp
    ${SOURCE_DIR}/Messages/write_device_message.cpp
    ${SOURCE_DIR}/Messages/config_device_message.cpp
    ${SOURCE_DIR}/Messages/elveflow_ob1/write_message_ob1.cpp
    ${SOURCE_DIR}/Utilities/utilities.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
    ${3RDPARTY_DIR}/ftd2xx/include/ftd2xx.h
    ${3RDPARTY_DIR}/elveflow/x64/Elveflow64.h
)

# Find libraries
find_library(LIB_ELVEFLOW Elveflow64 ${3RDPARTY_DIR}/elveflow/x64)

target_link_libraries(test_ob1 PUBLIC
    ${LIB_ELVEFLOW}
)

# Enforce C++20
set_property(TARGET test_ob1 PROPERTY CXX_STANDARD 20)

# Platform-specific settings
if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    add_compile_definitions(_HAS_STD_BYTE=0)

    # Winows specific sources
    target_sources(test_ob1 PRIVATE
        ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1_win.hpp
    
        ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1_win.cpp
    )

    # Add post-build step that copies the DLLs to the output directory. 
    find_file(DLL_ELVEFLOW Elveflow64.dll ${3RDPARTY_DIR}/elveflow/x64)
    add_custom_command(TARGET test_ob1 POST_BUILD       
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${DLL_ELVEFLOW}
        $<TARGET_FILE_DIR:test_ob1>
    ) 
endif (WIN32)
if (UNIX)
    target_sources(test_ob1 PRIVATE)
endif (UNIX)

