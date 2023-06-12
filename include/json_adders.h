#ifndef JSON_ADDERS_H
#define JSON_ADDERS_H

bool
json_add_item(
  struct json_t* const json,
  const enum json_type_e type,
  const char* const key,
  void* value);

bool
json_add_int32(
  struct json_t* const json,
  const char* const key,
  int32_t value);

bool
json_add_decimal(
  struct json_t* const json,
  const char* const key,
  double value);

bool
json_add_bool(
  struct json_t* const json,
  const char* const key,
  bool value);

bool
json_add_string(
  struct json_t* const json,
  const char* const key,
  char* const value);

bool
json_add_object(
  struct json_t* const json,
  const char* const key,
  struct json_t* const value);

bool
json_add_array(
  struct json_t* const json,
  const char* const key,
  struct json_array_t* value);

bool
json_add_null(
  struct json_t* const json,
  const char* const key);

#endif
