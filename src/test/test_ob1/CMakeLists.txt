set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .
    ${INCLUDE_DIR}

    ${3RDPARTY_DIR}/catch2/single_include
)

add_executable(test_ob1

    test_ob1.cpp
)

# Find libraries
find_library(LIB_ELVEFLOW Elveflow64 ${3RDPARTY_DIR}/elveflow/x64)

target_link_libraries(test_ob1
    PUBLIC ${LIB_ELVEFLOW}
    PUBLIC scimon_message
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
