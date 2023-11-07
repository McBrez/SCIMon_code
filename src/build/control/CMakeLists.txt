cmake_minimum_required(VERSION 3.16)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qwt REQUIRED)
qt_standard_project_setup()

qt_add_executable(control
    control.cpp

    dialog.h
    ${INCLUDE_DIR}/Workers/control_worker/control_worker_wrapper.hpp
    ${INCLUDE_DIR}/Gui/spectroplot.hpp
    ${INCLUDE_DIR}/Gui/spectroplot_data.hpp

    dialog.cpp
    ${SOURCE_DIR}/Workers/control_worker/control_worker_wrapper.cpp
    ${SOURCE_DIR}/Gui/spectroplot.cpp
    ${SOURCE_DIR}/Gui/spectroplot_data.cpp

    dialog.ui
)

target_include_directories(control PUBLIC
    ${INCLUDE_DIR}/Workers/control_worker
    ${INCLUDE_DIR}/Gui
)

target_link_libraries(control PRIVATE
    Qt6::Widgets
    scimon_message
)

# Set up build according to build type.
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    # Link against the release library of Qwt.
    target_link_libraries(control PRIVATE
        Qwt::Qwt
    )
    # This hides the console windows when the binary is started.
    set_target_properties(control PROPERTIES
        WIN32_EXECUTABLE ON
    )
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # Link against the debug library of Qwt.
    target_link_libraries(control PRIVATE
        Qwt::Qwtd
    )
    # This shows a console windows when the binary is started.
    # This console window makes it more convenient to see the log.
    set_target_properties(control PROPERTIES
        WIN32_EXECUTABLE OFF
    )
else()
    message( SEND_ERROR "Unkown build type.")
endif()



# Enforce C++20
set_property(TARGET sentry PROPERTY CXX_STANDARD 20)

# Add some defines
target_compile_definitions(control
    # Undefine a WIN function, that would otherwise clash with flatbuffers.
    PUBLIC NOMINMAX=1
    # Make easylogging++ thread safe
    PUBLIC ELPP_THREAD_SAFE
    PUBLIC ELPP_FORCE_USE_STD_THREAD
    PUBLIC QT_NO_PRINTER
    PUBLIC QWT_DLL
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

       set(INSTALL_DIR $ENV{ProgramFiles}/SCIMon)

endif (WIN32)
if (UNIX)
    
endif (UNIX)

install(TARGETS control
    DESTINATION ${INSTALL_DIR}
)
