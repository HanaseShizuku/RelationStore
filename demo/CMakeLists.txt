add_executable(demo demo.cpp)

target_link_libraries(demo PRIVATE relation_store_lib)

add_custom_command(TARGET demo POST_BUILD

COMMAND ${CMAKE_COMMAND} -E copy_if_different

$<TARGET_FILE:relation_store_lib>

$<TARGET_FILE_DIR:demo>

)