cmake_minimum_required(VERSION 3.10)
project(FastNoisePreviewAppQt)

set(CMAKE_CXX_STANDARD 11)

######
# Qt5
find_package(Qt5 REQUIRED Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/FastNoise/Cpp
)

add_executable(
    FastNoisePreviewAppQt
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    NoiseCanvasWidget.cpp NoiseCanvasWidget.h)

target_link_libraries(
    FastNoisePreviewAppQt
    Qt5::Widgets
)
