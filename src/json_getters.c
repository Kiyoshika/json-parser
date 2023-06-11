#include "json.h"
#include "json_array.h"
#include "json_internal.h"

void*
json_get(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;

  return _json_get_item_value(&json->items[idx]);
}

int32_t*
json_get_int32(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;
  return &json->items[idx].value.int32;
}

double*
json_get_decimal(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;
  return &json->items[idx].value.decimal;
}

char*
json_get_string(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;
  return json->items[idx].value.str;
}

struct json_t*
json_get_object(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;
  return json->items[idx].value.object;
}

struct json_array_t*
json_get_array(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;
  return json->items[idx].value.array;
}

bool*
json_get_bool(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return NULL;
  return &json->items[idx].value.boolean;
}

bool
json_get_isnull(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return true; // technically...if it doesn't exist I guess it's null...
  return json->items[idx].type == JSON_NULL;
}
