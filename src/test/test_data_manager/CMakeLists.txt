set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

add_executable(test_data_manager
    test_data_manager.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
)

include_directories(
    ${3RDPARTY_DIR}/catch2/single_include
)

target_link_libraries(test_data_manager
    PUBLIC scimon_message
)

# Add some defines
target_compile_definitions(test_data_manager
    # Undefine a WIN function, that would otherwise clash with flatbuffers.
    PUBLIC NOMINMAX=1  
    # Make easylogging++ thread safe
    PUBLIC ELPP_THREAD_SAFE  
    PUBLIC ELPP_FORCE_USE_STD_THREAD
)


# Enforce C++20
set_property(TARGET test_data_manager PROPERTY CXX_STANDARD 20)

if (WIN32)
    # Disable the "byte" type, introduced by MSVC in newer versions. Otherwise, 
    # it would clash with the std::byte type. 
    # add_compile_definitions(_HAS_STD_BYTE=0)

    # Copy the DLLs to the output directory
    find_library(DLL_ELVEFLOW Elveflow64.dll ${3RDPARTY_DIR}/elveflow/x64)
    add_custom_command(
        TARGET test_data_manager
         POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
           "${DLL_ELVEFLOW}"
           $<TARGET_FILE_DIR:sentry>/Elveflow64.dll)
endif (WIN32)
if (UNIX)
    
endif (UNIX)
