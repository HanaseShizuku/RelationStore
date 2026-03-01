include(GenerateExportHeader)


add_library(light_bfs_path_lib SHARED 
    light_bfs_path.cpp
)

generate_export_header(light_bfs_path_lib)

target_include_directories(light_bfs_path_lib PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )