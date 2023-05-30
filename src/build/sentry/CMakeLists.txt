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
    
    ${INCLUDE_DIR}/Workers/sentry_worker/sentry_worker.hpp
    ${INCLUDE_DIR}/Workers/sentry_worker/sentry_init_payload.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker.hpp
    ${INCLUDE_DIR}/Workers/network_worker/network_worker_init_payload.hpp

    ${SOURCE_DIR}/Workers/sentry_worker/sentry_worker.cpp
    ${SOURCE_DIR}/Workers/sentry_worker/sentry_init_payload.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker.cpp
    ${SOURCE_DIR}/Workers/network_worker/network_worker_init_payload.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h

    ${3RDPARTY_DIR}/argparse/include/argparse/argparse.hpp
)

target_link_libraries(sentry scimon_message_lib)

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
