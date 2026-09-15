cmake_minimum_required(VERSION 3.16)
project(proj_5_cpp_adjustment CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 核心计算库（仅标准库，自研矩阵）
add_library(adjustment_core
    src/matrix.cpp
    src/leveling.cpp
)
target_include_directories(adjustment_core PUBLIC include)

# 命令行入口
add_executable(adjustment_cli src/main_cli.cpp)
target_link_libraries(adjustment_cli PRIVATE adjustment_core)

# 测试
enable_testing()
add_executable(test_matrix tests/test_matrix.cpp)
target_link_libraries(test_matrix PRIVATE adjustment_core)
add_test(NAME test_matrix COMMAND test_matrix)

add_executable(test_leveling tests/test_leveling.cpp)
target_link_libraries(test_leveling PRIVATE adjustment_core)
add_test(NAME test_leveling COMMAND test_leveling)

add_executable(test_leveling_complex tests/test_leveling_complex.cpp)
target_link_libraries(test_leveling_complex PRIVATE adjustment_core)
add_test(NAME test_leveling_complex COMMAND test_leveling_complex)

# 可视化窗口（需要 Qt6，可选）
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    add_executable(adjustment_gui
        gui/main_gui.cpp
        gui/mainwindow.cpp
        gui/networkview.cpp
    )
    target_link_libraries(adjustment_gui PRIVATE adjustment_core Qt6::Widgets)
endif()
