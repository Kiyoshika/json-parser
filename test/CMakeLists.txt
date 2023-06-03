add_executable(json_create json_create.c)
target_include_directories(json_create PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_create json)
add_test(NAME json_create COMMAND json_create)

add_executable(json_add_and_get json_add_and_get.c)
target_include_directories(json_add_and_get PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_add_and_get json)
add_test(NAME json_add_and_get COMMAND json_add_and_get)

add_executable(json_parse_1 json_parse_1.c)
target_include_directories(json_parse_1 PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_1 json)
add_test(NAME json_parse_1 COMMAND json_parse_1)
