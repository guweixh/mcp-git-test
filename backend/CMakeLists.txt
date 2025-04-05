cmake_minimum_required(VERSION 3.14)

project(QCefViewReactDemo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt包
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# 设置QCefView路径
set(QCEF_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/QCefView" CACHE PATH "QCefView root directory")
set(QCEF_INCLUDE_DIR "${QCEF_ROOT_DIR}/inc")
set(QCEF_INSTALL_DIR "${QCEF_ROOT_DIR}/$<CONFIG>")

include_directories(${QCEF_INCLUDE_DIR})
link_directories(${QCEF_INSTALL_DIR})

# 添加资源文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
    ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc
    COPYONLY
)

# 添加源文件
add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    resources.qrc
)

# 链接QCefView和Qt库
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets
    QCefView
)

# 复制前端文件到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/build
    ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/web
)

# 复制QCefView的DLL文件到构建目录（仅适用于Windows）
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QCEF_INSTALL_DIR}/QCefView.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()