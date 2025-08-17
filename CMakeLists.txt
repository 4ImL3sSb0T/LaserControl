cmake_minimum_required(VERSION 3.31)
project(LaserControl)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_library(SERIAL_STATIC serial ${PROJECT_SOURCE_DIR}/lib/static)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCES)

# 生成可执行文件
add_executable(LaserControl ${SOURCES} 
        src/Application.cpp
        src/LaserControl.cpp
        src/Logger.cpp
        src/PIDControllor.cpp
        src/opencv/track_target.cpp
        src/opencv/roi_extractor.cpp
)

target_include_directories(LaserControl PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
)

target_link_libraries(LaserControl PRIVATE
        ${SERIAL_STATIC}
)

target_link_libraries(LaserControl PRIVATE
        opencv_ml
        opencv_dnn
        opencv_core
        opencv_flann
        opencv_videoio
        opencv_highgui
        opencv_imgproc
        sfml-system
        sfml-network
        sfml-graphics
        sfml-window
        imgui::imgui
        ImGui-SFML::ImGui-SFML
        fmt::fmt
        spdlog::spdlog
)
