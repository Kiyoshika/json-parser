add_executable(json_create json_create.c)
target_include_directories(json_create PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_create json)
add_test(NAME json_create COMMAND json_create)

add_executable(json_add_and_get json_add_and_get.c)
target_include_directories(json_add_and_get PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_add_and_get json)
add_test(NAME json_add_and_get COMMAND json_add_and_get)

add_executable(json_parse_string_1 json_parse_string_1.c)
target_include_directories(json_parse_string_1 PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_string_1 json)
add_test(NAME json_parse_string_1 COMMAND json_parse_string_1)

add_executable(json_parse_string_2 json_parse_string_2.c)
target_include_directories(json_parse_string_2 PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_string_2 json)
add_test(NAME json_parse_string_2 COMMAND json_parse_string_2)

add_executable(json_parse_array_primitives json_parse_array_primitives.c)
target_include_directories(json_parse_array_primitives PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_array_primitives json)
add_test(NAME json_parse_array_primitives COMMAND json_parse_array_primitives)

add_executable(json_parse_array_objects json_parse_array_objects.c)
target_include_directories(json_parse_array_objects PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_array_objects json)
add_test(NAME json_parse_array_objects COMMAND json_parse_array_objects)

add_executable(json_parse_array_arrays json_parse_array_arrays.c)
target_include_directories(json_parse_array_arrays PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_array_arrays json)
add_test(NAME json_parse_array_arrays COMMAND json_parse_array_arrays)

add_executable(json_parse_string_nested json_parse_string_nested.c)
target_include_directories(json_parse_string_nested PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_parse_string_nested json)
add_test(NAME json_parse_string_nested COMMAND json_parse_string_nested)

add_executable(json_numeric_whitespace json_numeric_whitespace.c)
target_include_directories(json_numeric_whitespace PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_numeric_whitespace json)
add_test(NAME json_numeric_whitespace COMMAND json_numeric_whitespace)

add_executable(json_string_whitespace json_string_whitespace.c)
target_include_directories(json_string_whitespace PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_string_whitespace json)
add_test(NAME json_string_whitespace COMMAND json_string_whitespace)

add_executable(json_missing_delimiter json_missing_delimiter.c)
target_include_directories(json_missing_delimiter PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_missing_delimiter json)
add_test(NAME json_missing_delimiter COMMAND json_missing_delimiter)

add_executable(json_empty_array json_empty_array.c)
target_include_directories(json_empty_array PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_empty_array json)
add_test(NAME json_empty_array COMMAND json_empty_array)

add_executable(json_bool json_bool.c)
target_include_directories(json_bool PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_bool json)
add_test(NAME json_bool COMMAND json_bool)

add_executable(json_numbers_in_key json_numbers_in_key.c)
target_include_directories(json_numbers_in_key PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_numbers_in_key json)
add_test(NAME json_numbers_in_key COMMAND json_numbers_in_key)

add_executable(json_empty_objects json_empty_objects.c)
target_include_directories(json_empty_objects PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_empty_objects json)
add_test(NAME json_empty_objects COMMAND json_empty_objects)

add_executable(json_missing_colon json_missing_colon.c)
target_include_directories(json_missing_colon PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_missing_colon json)
add_test(NAME json_missing_colon COMMAND json_missing_colon)

add_executable(json_duplicate_key json_duplicate_key.c)
target_include_directories(json_duplicate_key PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_duplicate_key json)
add_test(NAME json_duplicate_key COMMAND json_duplicate_key)

add_executable(json_getters json_getters.c)
target_include_directories(json_getters PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_getters json)
add_test(NAME json_getters COMMAND json_getters)

add_executable(json_setters json_setters.c)
target_include_directories(json_setters PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_setters json)
add_test(NAME json_setters COMMAND json_setters)

add_executable(json_empty_strings json_empty_strings.c)
target_include_directories(json_empty_strings PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json_empty_strings json)
add_test(NAME json_empty_strings COMMAND json_empty_strings)
