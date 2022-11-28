cmake_minimum_required(VERSION 3.10)
project(test_utility)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)

include_directories(
    .
    ${INCLUDE_DIR}/Utility
    ${3RDPARTY_DIR}/easyloggingpp/src
    ${3RDPARTY_DIR}/catch2/single_include
)

add_executable(test_utility

    test_utility.cpp
   
    ${INCLUDE_DIR}/Utility/utility.hpp

    ${SOURCE_DIR}/Utility/utility.cpp

    ${3RDPARTY_DIR}/catch2/single_include/catch2/catch.hpp
    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.cc

    ${3RDPARTY_DIR}/easyloggingpp/src/easylogging++.h
    ${3RDPARTY_DIR}/ftd2xx/include/ftd2xx.h
)

# Enforce C++20
set_property(TARGET test_isx3 PROPERTY CXX_STANDARD 20)
