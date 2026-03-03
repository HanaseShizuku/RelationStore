add_executable(relation_console bfs_path_test_console.cpp)

target_link_libraries(relation_console PRIVATE relation_store_lib)

add_custom_command(TARGET relation_console POST_BUILD

COMMAND ${CMAKE_COMMAND} -E copy_if_different

$<TARGET_FILE:relation_store_lib>

$<TARGET_FILE_DIR:relation_console>

)