include(GenerateExportHeader)


add_library(bfs_path_lib SHARED 
    bfs_path.cpp bfs_path.h
    bfs_result.cpp bfs_result.h
    file.hpp
    string.hpp
)

generate_export_header(bfs_path_lib)

target_include_directories(bfs_path_lib PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )