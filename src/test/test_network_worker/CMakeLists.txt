set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .  
    ${3RDPARTY_DIR}/catch2/single_include
)

add_executable(test_network_worker

    test_network_worker.cpp

    test_device.hpp
    
    test_device.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
)

target_link_libraries(test_network_worker
    PUBLIC scimon_message
)

# Enforce C++20
set_property(TARGET test_network_worker PROPERTY CXX_STANDARD 20)

# Add some defines
target_compile_definitions(test_network_worker
    # Undefine a WIN function, that would otherwise clash with flatbuffers.
    PUBLIC NOMINMAX=1  
    # Make easylogging++ thread safe
    PUBLIC ELPP_THREAD_SAFE  
    PUBLIC ELPP_FORCE_USE_STD_THREAD
)


# OS specific configuration.
if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    add_compile_definitions(_HAS_STD_BYTE=0)

    # Copy the DLLs to the output directory
    find_library(DLL_ELVEFLOW Elveflow64.dll ${3RDPARTY_DIR}/elveflow/x64)
    add_custom_command(
        TARGET test_network_worker
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_ELVEFLOW}"
            $<TARGET_FILE_DIR:test_network_worker>/Elveflow64.dll
    )
endif (WIN32)
if (UNIX)
    
endif (UNIX)
