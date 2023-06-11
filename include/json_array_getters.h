#ifndef JSON_ARRAY_GETTERS_H
#define JSON_ARRAY_GETTERS_H

void*
json_array_get(
  const struct json_array_t* const array,
  const size_t idx);

int32_t*
json_array_get_int32(
  const struct json_array_t* const array,
  const size_t idx);

double*
json_array_get_decimal(
  const struct json_array_t* const array,
  const size_t idx);

char*
json_array_get_string(
  const struct json_array_t* const array,
  const size_t idx);

struct json_t*
json_array_get_object(
  const struct json_array_t* const array,
  const size_t idx);

struct json_array_t*
json_array_get_array(
  const struct json_array_t* const array,
  const size_t idx);

bool*
json_array_get_bool(
  const struct json_array_t* const array,
  const size_t idx);

bool
json_array_get_isnull(
  const struct json_array_t* const array,
  const size_t idx);

#endif
