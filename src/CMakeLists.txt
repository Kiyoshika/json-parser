add_library(json 
  json.c 
  json_getters.c
  json_setters.c
  json_internal.c 
  json_array.c 
  json_array_getters.c
  json_array_setters.c)
target_include_directories(json PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(json m)
