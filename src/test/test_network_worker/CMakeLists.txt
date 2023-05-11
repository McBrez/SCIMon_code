cmake_minimum_required(VERSION 3.10)
project(test_network_worker)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/Core
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/elveflow_ob1
    ${INCLUDE_DIR}/Messages
    ${INCLUDE_DIR}/Utilities
    ${INCLUDE_DIR}/Workers
    ${INCLUDE_DIR}/Workers/network_worker
    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/catch2/single_include
)
add_executable(test_network_worker

    test_network_worker.cpp
    
    ${INCLUDE_DIR}/Workers/worker.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker_init_payload.hpp
    ${INCLUDE_DIR}/Messages/message_factory.hpp
    ${INCLUDE_DIR}/Messages/message_interface.hpp
    ${INCLUDE_DIR}/Messages/device_message.hpp
    ${INCLUDE_DIR}/Messages/handshake_message.hpp
    ${INCLUDE_DIR}/Messages/write_device_message.hpp
    ${INCLUDE_DIR}/Messages/config_device_message.hpp
    ${INCLUDE_DIR}/Messages/init_device_message.hpp
    ${INCLUDE_DIR}/Messages/device_status_message.hpp
    ${INCLUDE_DIR}/Messages/read_device_message.hpp
    ${INCLUDE_DIR}/Messages/user_id.hpp
    ${INCLUDE_DIR}/Messages/dummy_message.hpp
    ${INCLUDE_DIR}/Messages/message_distributor.hpp
    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/dummy_device.hpp
    ${INCLUDE_DIR}/Devices/status_payload.hpp
    ${INCLUDE_DIR}/Devices/read_payload.hpp
    ${INCLUDE_DIR}/Devices/init_payload.hpp
    ${INCLUDE_DIR}/Devices/configuration_payload.hpp
    ${INCLUDE_DIR}/Devices/is_payload.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/read_payload_ob1.hpp
    ${INCLUDE_DIR}/Utilities/socket_wrapper.hpp

    ${SOURCE_DIR}/Workers/worker.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker_init_payload.cpp
    ${SOURCE_DIR}/Messages/message_distributor.cpp
    ${SOURCE_DIR}/Messages/message_factory.cpp
    ${SOURCE_DIR}/Messages/message_interface.cpp
    ${SOURCE_DIR}/Messages/device_message.cpp
    ${SOURCE_DIR}/Messages/handshake_message.cpp
    ${SOURCE_DIR}/Messages/write_device_message.cpp
    ${SOURCE_DIR}/Messages/read_device_message.cpp
    ${SOURCE_DIR}/Messages/config_device_message.cpp
    ${SOURCE_DIR}/Messages/init_device_message.cpp
    ${SOURCE_DIR}/Messages/device_status_message.cpp
    ${SOURCE_DIR}/Messages/user_id.cpp
    ${SOURCE_DIR}/Messages/dummy_message.cpp
    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/dummy_device.cpp
    ${SOURCE_DIR}/Devices/status_payload.cpp
    ${SOURCE_DIR}/Devices/is_payload.cpp
    ${SOURCE_DIR}/Devices/read_payload.cpp
    ${SOURCE_DIR}/Devices/configuration_payload.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/read_payload_ob1.cpp
    ${SOURCE_DIR}/Utilities/socket_wrapper.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
)

# Enforce C++20
set_property(TARGET test_network_worker PROPERTY CXX_STANDARD 20)

if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    add_compile_definitions(_HAS_STD_BYTE=0)

    target_link_libraries(test_network_worker
        Ws2_32 
    )
    target_sources(test_network_worker PRIVATE
        ${INCLUDE_DIR}/Utilities/win_socket.hpp

        ${SOURCE_DIR}/Utilities/win_socket.cpp
    )
endif (WIN32)
if (UNIX)
    
endif (UNIX)
