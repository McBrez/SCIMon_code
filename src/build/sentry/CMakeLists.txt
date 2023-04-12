cmake_minimum_required(VERSION 3.10)
project(sentry)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Core
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/isx3
    ${INCLUDE_DIR}/Devices/elveflow_ob1
    ${INCLUDE_DIR}/Messages
    ${INCLUDE_DIR}/Messages/elveflow_ob1
    ${INCLUDE_DIR}/Utilities
    ${INCLUDE_DIR}/Workers
    ${INCLUDE_DIR}/Workers/sentry_worker
    ${INCLUDE_DIR}/Workers/network_worker

    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/catch2/single_include
    ${3RDPARTY_DIR}/argparse/include/argparse
    ${3RDPARTY_DIR}/elveflow/x64
)
add_executable(sentry

    sentry.cpp
    
    ${INCLUDE_DIR}/Core/common.hpp
    ${INCLUDE_DIR}/Messages/device_message.hpp
    ${INCLUDE_DIR}/Messages/read_device_message.hpp
    ${INCLUDE_DIR}/Messages/write_device_message.hpp
    ${INCLUDE_DIR}/Messages/config_device_message.hpp
    ${INCLUDE_DIR}/Messages/init_device_message.hpp   
    ${INCLUDE_DIR}/Messages/device_status_message.hpp
    ${INCLUDE_DIR}/Messages/handshake_message.hpp
    ${INCLUDE_DIR}/Messages/dummy_message.hpp
    ${INCLUDE_DIR}/Messages/message_interface.hpp
    ${INCLUDE_DIR}/Messages/message_distributor.hpp
    ${INCLUDE_DIR}/Messages/elveflow_ob1/write_message_ob1.hpp
    ${INCLUDE_DIR}/Messages/user_id.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_constants.hpp
    ${INCLUDE_DIR}/Devices/isx3/device_isx3.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_init_payload.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_ack_payload.hpp
    ${INCLUDE_DIR}/Devices/isx3/com_interface_codec.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_command_buffer.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_cmd_ack_struct.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_is_conf_payload.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/read_payload_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_constants.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_init_payload.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_conf_payload.hpp
    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/configuration_payload.hpp
    ${INCLUDE_DIR}/Devices/is_configuration.hpp
    ${INCLUDE_DIR}/Devices/read_payload.hpp
    ${INCLUDE_DIR}/Devices/is_payload.hpp
    ${INCLUDE_DIR}/Devices/id_payload.hpp
    ${INCLUDE_DIR}/Devices/init_payload.hpp
    ${INCLUDE_DIR}/Devices/status_payload.hpp
    ${INCLUDE_DIR}/Utilities/utilities.hpp
    ${INCLUDE_DIR}/Utilities/socket_wrapper.hpp
    ${INCLUDE_DIR}/Utilities/win_socket.hpp
    ${INCLUDE_DIR}/Workers/worker.hpp
    ${INCLUDE_DIR}/Workers/sentry_worker/sentry_worker.hpp
    ${INCLUDE_DIR}/Workers/sentry_worker/sentry_init_payload.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker_init_payload.hpp

    ${SOURCE_DIR}/Messages/device_message.cpp
    ${SOURCE_DIR}/Messages/config_device_message.cpp
    ${SOURCE_DIR}/Messages/read_device_message.cpp
    ${SOURCE_DIR}/Messages/device_status_message.cpp
    ${SOURCE_DIR}/Messages/init_device_message.cpp
    ${SOURCE_DIR}/Messages/write_device_message.cpp
    ${SOURCE_DIR}/Messages/handshake_message.cpp
    ${SOURCE_DIR}/Messages/dummy_message.cpp
    ${SOURCE_DIR}/Messages/message_interface.cpp
    ${SOURCE_DIR}/Messages/message_distributor.cpp
    ${SOURCE_DIR}/Messages/user_id.cpp
    ${SOURCE_DIR}/Messages/elveflow_ob1/write_message_ob1.cpp
    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/configuration_payload.cpp
    ${SOURCE_DIR}/Devices/is_configuration.cpp
    ${SOURCE_DIR}/Devices/read_payload.cpp
    ${SOURCE_DIR}/Devices/is_payload.cpp
    ${SOURCE_DIR}/Devices/id_payload.cpp
    ${SOURCE_DIR}/Devices/status_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/device_isx3.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_init_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/com_interface_codec.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_command_buffer.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_ack_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_is_conf_payload.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/ob1_init_payload.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/read_payload_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/ob1_conf_payload.cpp
    ${SOURCE_DIR}/Utilities/utilities.cpp
    ${SOURCE_DIR}/Utilities/socket_wrapper.cpp
    ${SOURCE_DIR}/Utilities/win_socket.cpp
    ${SOURCE_DIR}/Workers/worker.cpp
    ${SOURCE_DIR}/Workers/sentry_worker/sentry_worker.cpp
    ${SOURCE_DIR}/Workers/sentry_worker/sentry_init_payload.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker_init_payload.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h

    ${3RDPARTY_DIR}/argparse/include/argparse/argparse.hpp
)

# Enforce C++20
set_property(TARGET sentry PROPERTY CXX_STANDARD 20)

if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    add_compile_definitions(_HAS_STD_BYTE=0)

    # Find libraries
    find_library(LIB_ELVEFLOW Elveflow64 ${3RDPARTY_DIR}/elveflow/x64)

    target_link_libraries(sentry
        Ws2_32
        ${LIB_ELVEFLOW}
    )
    target_sources(sentry PRIVATE
        ${INCLUDE_DIR}/Utilities/win_socket.hpp
        ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1_win.hpp

        ${SOURCE_DIR}/Utilities/win_socket.cpp
        ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1_win.cpp
    )

    # Copy the DLLs to the output directory
    find_library(DLL_ELVEFLOW Elveflow64.dll ${3RDPARTY_DIR}/elveflow/x64)
    add_custom_command(
        TARGET sentry
         POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
           "${DLL_ELVEFLOW}"
           $<TARGET_FILE_DIR:sentry>/Elveflow64.dll)
endif (WIN32)
if (UNIX)
    
endif (UNIX)
