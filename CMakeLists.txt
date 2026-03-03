cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(BFSPath)

# 添加子目录
add_subdirectory(RelationStoreLib)
add_subdirectory(demo)