cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(LightBFSPath)

# 添加子目录
add_subdirectory(LightBFSPathLib)
add_subdirectory(LightBFSPathTestConsole)