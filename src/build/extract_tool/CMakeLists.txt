cmake_minimum_required(VERSION 3.16)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(extract_tool
    extract_tool.cpp
)

target_include_directories(extract_tool PUBLIC
    .
)

target_link_libraries(extract_tool PRIVATE
    scimon_message
    argparse
)

# Add some defines
target_compile_definitions(extract_tool
    # Undefine a WIN function, that would otherwise clash with flatbuffers.
    PUBLIC NOMINMAX=1
    # Make easylogging++ thread safe
    PUBLIC ELPP_THREAD_SAFE
    PUBLIC ELPP_FORCE_USE_STD_THREAD
)

# Enforce C++20
set_property(TARGET extract_tool PROPERTY CXX_STANDARD 20)
