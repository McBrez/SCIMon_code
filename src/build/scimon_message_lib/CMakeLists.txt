cmake_minimum_required(VERSION 3.15)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)
set(RES_DIR ../../res)

include(${3RDPARTY_DIR}/flatbuffers/Cmake/BuildFlatBuffers.cmake)

# The flatbuffer schema files, that shall be translated.
set(
    FB_FILES 
    ${RES_DIR}/flatbuffers/Messages/device_message.fbs 
    ${RES_DIR}/flatbuffers/Messages/write_device_message_content.fbs 
    ${RES_DIR}/flatbuffers/Messages/read_device_message_content.fbs 
    ${RES_DIR}/flatbuffers/Messages/handshake_message_content.fbs 
    ${RES_DIR}/flatbuffers/Messages/init_device_message_content.fbs 
    ${RES_DIR}/flatbuffers/Messages/config_device_message_content.fbs 

    ${RES_DIR}/flatbuffers/Devices/is_payload.fbs 

    ${RES_DIR}/flatbuffers/Devices/elveflow/ob1/ob1_config_payload.fbs
    ${RES_DIR}/flatbuffers/Devices/elveflow/ob1/ob1_init_payload.fbs
    ${RES_DIR}/flatbuffers/Devices/elveflow/ob1/ob1_read_payload.fbs

    ${RES_DIR}/flatbuffers/Devices/sciospec/isx3/isx3_config_payload.fbs
    ${RES_DIR}/flatbuffers/Devices/sciospec/isx3/isx3_init_payload.fbs
    ${RES_DIR}/flatbuffers/Devices/sciospec/isx3/isx3_read_payload.fbs
)
# Set include directories for the schema compiler.
set(
    FB_INCLUDE_DIR
    ${RES_DIR}/flatbuffers/Messages
    ${RES_DIR}/flatbuffers/Devices/elveflow/ob1
    ${RES_DIR}/flatbuffers/Devices/sciospec/isx3
)
# Construct path to the flatbuffer output files.
list(
    TRANSFORM FB_FILES REPLACE .fbs _generated.h
    OUTPUT_VARIABLE FB_FILES_GENERATED)
set(FB_OUT_DIR ${RES_DIR}/flatbuffers)
# Instruct flatbuffers to generate object oriented API.
set(FLATBUFFERS_FLATC_SCHEMA_EXTRA_ARGS 
    ${FLATBUFFERS_FLATC_SCHEMA_EXTRA_ARGS} --gen-object-api)
    # Build the flatbuffer schemas.
build_flatbuffers(
    "${FB_FILES}"
    "${RES_DIR}/flatbuffers"
    scimon_message_lib_fb_headers
    ""
    "${FB_OUT_DIR}"
    ""
    ""
)

add_library(scimon_message STATIC)
target_include_directories(scimon_message PUBLIC
    ${INCLUDE_DIR}/Core
    ${INCLUDE_DIR}/Utilities
    ${INCLUDE_DIR}/Devices
    ${INCLUDE_DIR}/Devices/isx3
    ${INCLUDE_DIR}/Devices/elveflow_ob1
    ${INCLUDE_DIR}/Messages
    ${INCLUDE_DIR}/Messages/elveflow_ob1
    ${INCLUDE_DIR}/Workers
    ${INCLUDE_DIR}/Workers/network_worker

    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/flatbuffers/include
    ${3RDPARTY_DIR}/elveflow/x64/

    ${FB_OUT_DIR}
)
target_sources(scimon_message PUBLIC
    ${INCLUDE_DIR}/Core/common.hpp
    ${INCLUDE_DIR}/Utilities/utilities.hpp
    ${INCLUDE_DIR}/Utilities/socket_wrapper.hpp
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
    ${INCLUDE_DIR}/Messages/elveflow_ob1/write_message_ob1.hpp
    ${INCLUDE_DIR}/Devices/device.hpp
    ${INCLUDE_DIR}/Devices/dummy_device.hpp
    ${INCLUDE_DIR}/Devices/payload.hpp
    ${INCLUDE_DIR}/Devices/status_payload.hpp
    ${INCLUDE_DIR}/Devices/read_payload.hpp
    ${INCLUDE_DIR}/Devices/init_payload.hpp
    ${INCLUDE_DIR}/Devices/configuration_payload.hpp
    ${INCLUDE_DIR}/Devices/is_configuration.hpp
    ${INCLUDE_DIR}/Devices/is_payload.hpp
    ${INCLUDE_DIR}/Devices/id_payload.hpp
    ${INCLUDE_DIR}/Devices/payload_decoder.hpp
    ${INCLUDE_DIR}/Workers/worker.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker_init_payload.hpp
    ${INCLUDE_DIR}/Devices/isx3/com_interface_codec.hpp
    ${INCLUDE_DIR}/Devices/isx3/device_isx3.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_ack_payload.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_cmd_ack_struct.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_command_buffer.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_constants.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_init_payload.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_is_conf_payload.hpp
    ${INCLUDE_DIR}/Devices/isx3/isx3_payload_decoder.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/read_payload_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_constants.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_init_payload.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_conf_payload.hpp
    ${INCLUDE_DIR}/Devices/elveflow_ob1/ob1_payload_decoder.hpp

    ${SOURCE_DIR}/Utilities/utilities.cpp
    ${SOURCE_DIR}/Utilities/socket_wrapper.cpp
    ${SOURCE_DIR}/Utilities/win_socket.cpp
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
    ${SOURCE_DIR}/Messages/elveflow_ob1/write_message_ob1.cpp
    ${SOURCE_DIR}/Devices/device.cpp
    ${SOURCE_DIR}/Devices/dummy_device.cpp
    ${SOURCE_DIR}/Devices/status_payload.cpp
    ${SOURCE_DIR}/Devices/is_payload.cpp
    ${SOURCE_DIR}/Devices/is_configuration.cpp
    ${SOURCE_DIR}/Devices/id_payload.cpp
    ${SOURCE_DIR}/Devices/read_payload.cpp
    ${SOURCE_DIR}/Devices/configuration_payload.cpp
    ${SOURCE_DIR}/Workers/worker.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker_init_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/com_interface_codec.cpp
    ${SOURCE_DIR}/Devices/isx3/device_isx3.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_ack_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_command_buffer.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_init_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_is_conf_payload.cpp
    ${SOURCE_DIR}/Devices/isx3/isx3_payload_decoder.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/ob1_init_payload.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/read_payload_ob1.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/ob1_conf_payload.cpp
    ${SOURCE_DIR}/Devices/elveflow_ob1/ob1_payload_decoder.cpp

    ${3RDPARTY_DIR}/flatbuffers/include/flatbuffers/flatbuffers.h
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc
)

# Add dependency to the generated flatbuffer headers and link against
# flatbuffers library.
add_dependencies(scimon_message scimon_message_lib_fb_headers)
target_link_libraries(scimon_message
    PRIVATE flatbuffers
)

# Generated files shall be cleaned properly.
set_property(
    TARGET scimon_message
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES FB_FILES_GENERATED
)

# Enforce C++20
set_property(TARGET scimon_message PROPERTY CXX_STANDARD 20)

# Add some defines
target_compile_definitions(scimon_message
    # Undefine a WIN function, that would otherwise clash with flatbuffers.
    PUBLIC NOMINMAX=1  
    # Add the version string as a define
    PUBLIC SCIMON_MESSAGE_LIB_VERSION=\"${CMAKE_PROJECT_VERSION}\"
)


# OS specific configuration.
if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    add_compile_definitions(_HAS_STD_BYTE=0)
    
    # Find libraries
    find_library(LIB_ELVEFLOW Elveflow64 ${3RDPARTY_DIR}/elveflow/x64)

    target_link_libraries(scimon_message
      PRIVATE Ws2_32
      PUBLIC ${LIB_ELVEFLOW}
    )
    target_sources(scimon_message PUBLIC
        ${INCLUDE_DIR}/Utilities/win_socket.hpp
        ${INCLUDE_DIR}/Devices/elveflow_ob1/device_ob1_win.hpp

        ${3RDPARTY_DIR}/elveflow/x64/Elveflow64.h

        ${SOURCE_DIR}/Utilities/win_socket.cpp
        ${SOURCE_DIR}/Devices/elveflow_ob1/device_ob1_win.cpp
    )
endif (WIN32)
if (UNIX)
    
endif (UNIX)