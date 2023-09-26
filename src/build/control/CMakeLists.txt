cmake_minimum_required(VERSION 3.16)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(control
    control.cpp

    dialog.h
    ${INCLUDE_DIR}/Workers/control_worker/control_worker_wrapper.hpp

    dialog.cpp
    ${SOURCE_DIR}/Workers/control_worker/control_worker_wrapper.cpp

    dialog.ui
)

target_include_directories(control PUBLIC
    ${INCLUDE_DIR}/Workers/control_worker
)

target_link_libraries(control PRIVATE
    Qt6::Widgets
    scimon_message
)

set_target_properties(control PROPERTIES
    WIN32_EXECUTABLE ON
)

# Enforce C++20
set_property(TARGET sentry PROPERTY CXX_STANDARD 20)

# Add some defines
target_compile_definitions(control
    # Undefine a WIN function, that would otherwise clash with flatbuffers.
    PUBLIC NOMINMAX=1
    # Make easylogging++ thread safe
    PUBLIC ELPP_THREAD_SAFE
    PUBLIC ELPP_FORCE_USE_STD_THREAD
)


# Enforce C++20
set_property(TARGET control PROPERTY CXX_STANDARD 20)

if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    # add_compile_definitions(_HAS_STD_BYTE=0)

    # Copy the DLLs to the output directory
    find_library(DLL_ELVEFLOW Elveflow64.dll ${3RDPARTY_DIR}/elveflow/x64)
    add_custom_command(
        TARGET control
         POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
           "${DLL_ELVEFLOW}"
           $<TARGET_FILE_DIR:control>/Elveflow64.dll)
endif (WIN32)
if (UNIX)
    
endif (UNIX)

