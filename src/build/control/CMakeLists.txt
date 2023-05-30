cmake_minimum_required(VERSION 3.16)

set(SOURCE_DIR ../../_shared_/src)
set(INCLUDE_DIR ../../_shared_/include)
set(3RDPARTY_DIR ../../3rd_party)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(control
    control.cpp
    dialog.h
    dialog.ui
    dialog.cpp
)

target_link_libraries(control PRIVATE
    Qt6::Widgets
)

set_target_properties(control PROPERTIES
    WIN32_EXECUTABLE ON
)
