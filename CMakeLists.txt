cmake_minimum_required(VERSION 3.10)
project(SCIMon)

set(PROJECT_DIR src/build)
set(TEST_DIR src/test)
set(3RDPARTY_DIR src/3rd_party)

# ------------------------------------------------------------ Pre-requisites --

add_subdirectory(
    ${3RDPARTY_DIR}/flatbuffers
    ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build
)
target_compile_definitions(flatbuffers PUBLIC NOMINMAX=1)

# --------------------------------------------------------- Project Libraries --

add_subdirectory(${PROJECT_DIR}/scimon_message_lib)

# ------------------------------------------------------- Project Executables --
add_subdirectory(${PROJECT_DIR}/sentry)
#add_subdirectory(${PROJECT_DIR}/control)

# --------------------------------------------------------------------- Tests --
add_subdirectory(${TEST_DIR}/test_ob1)
add_subdirectory(${TEST_DIR}/test_is_measurement)
add_subdirectory(${TEST_DIR}/test_utility)
add_subdirectory(${TEST_DIR}/test_sciospec_com_interface)
add_subdirectory(${TEST_DIR}/test_epix)
add_subdirectory(${TEST_DIR}/test_message_distributor)
add_subdirectory(${TEST_DIR}/test_message_factory)
add_subdirectory(${TEST_DIR}/test_network_worker)

enable_testing()
add_test(NAME build_test_utility COMMAND test_utility)
add_test(NAME build_message_distributor COMMAND test_message_distributor)
add_test(NAME build_test_ob1 COMMAND test_ob1)
add_test(NAME build_test_sciospec_com_interface COMMAND test_sciospec_com_interface)
add_test(NAME build_test_test_epix COMMAND test_epix)
add_test(NAME build_test_message_factory COMMAND test_message_factory)
add_test(NAME build_test_network_worker COMMAND test_network_worker)