cmake_minimum_required(VERSION 3.10)
project(SCIMon)

set(PROJECT_DIR src/build)
set(TEST_DIR src/test)
set(3RDPARTY_DIR src/3rd_party)

# Let CMAKE look for modules in the build folder. 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/src/build)

# ------------------------------------------------------------ Cache Variable --
set(SCIMON_MOCK_DEVICE_EPIX 
    FALSE
    CACHE
    BOOL 
    "Instructs the build system to include a mocked up version of the DeviceEpix class."
)
set(SCIMON_MOCK_DEVICE_OB1
    FALSE
    CACHE
    BOOL 
    "Instructs the build system to include a mocked up version of the DeviceOb1 class."
)

# ------------------------------------------------------------ Pre-requisites --

# Flatbuffers
add_subdirectory(
    ${3RDPARTY_DIR}/flatbuffers
    ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build
)
target_compile_definitions(flatbuffers PUBLIC NOMINMAX=1)

# HighFive
set(HIGHFIVE_USE_BOOST Off)
add_subdirectory(${3RDPARTY_DIR}/HighFive)

# --------------------------------------------------------- Project Libraries --

add_subdirectory(${PROJECT_DIR}/scimon_message_lib)

# ------------------------------------------------------- Project Executables --
add_subdirectory(${PROJECT_DIR}/sentry)
add_subdirectory(${PROJECT_DIR}/control)

# --------------------------------------------------------------------- Tests --
add_subdirectory(${TEST_DIR}/test_ob1)
add_subdirectory(${TEST_DIR}/test_is_measurement)
add_subdirectory(${TEST_DIR}/test_utility)
add_subdirectory(${TEST_DIR}/test_sciospec_com_interface)
add_subdirectory(${TEST_DIR}/test_epix)
add_subdirectory(${TEST_DIR}/test_message_distributor)
add_subdirectory(${TEST_DIR}/test_message_factory)
add_subdirectory(${TEST_DIR}/test_network_worker)
add_subdirectory(${TEST_DIR}/test_data_manager)

enable_testing()
add_test(NAME build_test_utility COMMAND test_utility)
add_test(NAME build_message_distributor COMMAND test_message_distributor)
add_test(NAME build_test_ob1 COMMAND test_ob1)
add_test(NAME build_test_sciospec_com_interface COMMAND test_sciospec_com_interface)
add_test(NAME build_test_test_epix COMMAND test_epix)
add_test(NAME build_test_message_factory COMMAND test_message_factory)
add_test(NAME build_test_network_worker COMMAND test_network_worker)
