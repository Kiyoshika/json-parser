#include "json.h"
#include "json_array.h"
#include "json_internal.h"

bool
json_set_int32(
  const struct json_t* const json,
  const char* const key,
  const int32_t value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  item->value.int32 = value;
  return true;
}

bool
json_set_decimal(
  const struct json_t* const json,
  const char* const key,
  const double value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  item->value.decimal = value;
  return true;
}

bool
json_set_string(
  const struct json_t* const json,
  const char* const key,
  const char* const value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  free(item->value.str);
  item->value.str = strdup(value);
  return true;
}

bool
json_set_object(
  const struct json_t* const json,
  const char* const key,
  struct json_t** value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  json_free(&item->value.object);
  item->value.object = *value;
  return true;
}

bool
json_set_array(
  const struct json_t* const json,
  const char* const key,
  struct json_array_t** value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  json_array_free(&item->value.array);
  item->value.array = *value;
  return true;
}

bool
json_set_bool(
  const struct json_t* const json,
  const char* const key,
  const bool value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  item->value.boolean = value;
  return true;
}
