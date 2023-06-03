add_library(json json.c)
target_include_directories(json PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json m)
