cmake_minimum_required(VERSION 3.21)

project(NexusOffice
    VERSION 0.1.0
    DESCRIPTION "A modern cross-platform office suite"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(nexus-office
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
)

target_link_libraries(nexus-office PRIVATE Qt6::Widgets)

set_target_properties(nexus-office PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
