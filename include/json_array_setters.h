#ifndef JSON_ARRAY_SETTERS_H
#define JSON_ARRAY_SETTERS_H

void
json_array_set(
  struct json_array_t* const array,
  const size_t idx,
  const enum json_type_e type,
  void* value);

void
json_array_set_int32(
  struct json_array_t* const array,
  const size_t idx,
  int32_t value);

void
json_array_set_decimal(
  struct json_array_t* const array,
  const size_t idx,
  double value);

void
json_array_set_string(
  struct json_array_t* const array,
  const size_t idx,
  char* value);

void
json_array_set_object(
  struct json_array_t* const array,
  const size_t idx,
  struct json_t* value);

void
json_array_set_array(
  struct json_array_t* const array,
  const size_t idx,
  struct json_array_t* value);

void
json_array_set_bool(
  struct json_array_t* const array,
  const size_t idx,
  bool value);

void
json_array_set_null(
  struct json_array_t* const array,
  const size_t idx);

#endif
