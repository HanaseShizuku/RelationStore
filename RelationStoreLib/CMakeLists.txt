include(GenerateExportHeader)


add_library(relation_store_lib SHARED 
    relation_store.cpp include/relation_store.h
    algorithm/bfs_result.cpp include/bfs_result.h
    utils/file.hpp
    utils/string.hpp
)

generate_export_header(relation_store_lib)

target_include_directories(relation_store_lib PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )