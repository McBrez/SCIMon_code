cmake_minimum_required(VERSION 3.10)
project(SCIMon)

set(PROJECT_DIR src/build)
set(TEST_DIR src/test)

# add_subdirectory(${PROJECT_DIR}/latency_tester)

# add_subdirectory(${TEST_DIR}/test_isx3)
add_subdirectory(${TEST_DIR}/test_ob1)
add_subdirectory(${TEST_DIR}/test_is_measurement)
add_subdirectory(${TEST_DIR}/test_utility)
add_subdirectory(${TEST_DIR}/test_sciospec_com_interface)
add_subdirectory(${TEST_DIR}/test_epix)

enable_testing()
add_test(NAME build_test_utility COMMAND test_utility)
add_test(NAME build_test_ob1 COMMAND test_ob1)
add_test(NAME build_test_sciospec_com_interface COMMAND test_sciospec_com_interface)
add_test(NAME build_test_test_epix COMMAND test_epix)


