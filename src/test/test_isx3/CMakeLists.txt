cmake_minimum_required(VERSION 3.10)
project(test_isx3)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

add_executable(test_isx3
    test_isx3.cpp
)

target_include_directories(test_isx3 PRIVATE
    .
    ${3RDPARTY_DIR}/catch2/single_include
)

message("Boost Libraries:" ${Boost_LIBRARIES})

# Enforce C++20
set_property(TARGET test_isx3 PROPERTY CXX_STANDARD 20)

target_link_libraries(test_isx3 
    PRIVATE scimon_message
    PRIVATE Boost::thread
)
