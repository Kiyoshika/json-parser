#ifndef JSON_ARRAY_ADDERS_H
#define JSON_ARRAY_ADDERS_H

// well, technically these are "appenders" but
// wanted to keep the name consistent with json_adders.h

bool
json_array_append(
  struct json_array_t* array,
  const enum json_type_e item_type,
  void* value);

bool
json_array_append_int32(
  struct json_array_t* array,
  int32_t value);

bool
json_array_append_decimal(
  struct json_array_t* array,
  double value);

bool
json_array_append_bool(
  struct json_array_t* array,
  bool value);

bool
json_array_append_string(
  struct json_array_t* array,
  char* value);

bool
json_array_append_object(
  struct json_array_t* array,
  struct json_t* value);

bool
json_array_append_array(
  struct json_array_t* array,
  struct json_array_t* value);

bool
json_array_append_null(
  struct json_array_t* array);

#endif
