add_executable(bfs_console bfs_path_test_console.cpp)

target_link_libraries(bfs_console PRIVATE bfs_path_lib)

add_custom_command(TARGET bfs_console POST_BUILD

COMMAND ${CMAKE_COMMAND} -E copy_if_different

$<TARGET_FILE:bfs_path_lib>

$<TARGET_FILE_DIR:bfs_console>

)