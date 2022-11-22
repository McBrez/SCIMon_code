cmake_minimum_required(VERSION 3.10)
project(test_isx3)

set(PROJECT_DIR src/build)
set(TEST_DIR src/test)

add_subdirectory(${PROJECT_DIR}/latency_tester)
add_subdirectory(${TEST_DIR}/test_isx3)
add_subdirectory(${TEST_DIR}/test_ob1)