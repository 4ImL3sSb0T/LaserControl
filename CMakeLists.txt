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
        src/IntervalTimer.cpp
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


function(collect_and_install_dependencies target)
    # 定义一个变量来存储找到的DLL文件列表
    set(dll_files "")

    # 使用 file(GLOB ...) 命令在构建目录中查找所有 DLL 文件
    # ${CMAKE_CURRENT_BINARY_DIR} 是当前的构建目录
    file(GLOB dll_files
            "${CMAKE_CURRENT_BINARY_DIR}/*.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/*.so"
            "${CMAKE_CURRENT_BINARY_DIR}/*.dylib" # 适配 macOS
    )

    # 遍历找到的每个DLL文件
    foreach(dll_path ${dll_files})
        # 提取文件名
        get_filename_component(dll_name ${dll_path} NAME)

        message(STATUS "找到并安装依赖: ${dll_name}")

        # 将 DLL 文件添加到安装列表
        # DESTINATION bin 表示将文件安装到安装目录的 bin 子目录中
        install(FILES ${dll_path} DESTINATION bin)
    endforeach()
endfunction()


# 设置安装路径
install(TARGETS LaserControl
        RUNTIME DESTINATION bin)
collect_and_install_dependencies(LaserControl)

# 设置需要一同安装的依赖文件或目录
# 如果您的程序需要其他资源文件，请取消注释并修改以下行
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
#         DESTINATION resources)

# 包含CPack模块以启用打包功能
include(CPack)


# CPack 配置
set(CPACK_PACKAGE_NAME "LaserControl")
set(CPACK_PACKAGE_VENDOR "AHPU")
set(CPACK_PACKAGE_DESCRIPTION "Laser Control Application")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LaserControl")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

