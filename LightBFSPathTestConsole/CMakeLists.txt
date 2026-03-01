add_executable(light_bfs_console light_bfs_path_test_console.cpp)

target_link_libraries(light_bfs_console PRIVATE light_bfs_path_lib)

add_custom_command(TARGET light_bfs_console POST_BUILD

COMMAND ${CMAKE_COMMAND} -E copy_if_different

$<TARGET_FILE:light_bfs_path_lib>

$<TARGET_FILE_DIR:light_bfs_console>

)