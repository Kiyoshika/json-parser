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

  struct json_item_t* current_item = &json->items[idx];
  switch (current_item->type)
  {
    case JSON_INT32:
      return &current_item->value.int32;
    case JSON_DECIMAL:
      return &current_item->value.decimal;
      break;
    case JSON_STRING:
      return current_item->value.str;
    case JSON_OBJECT:
      return current_item->value.object;
    case JSON_ARRAY:
      return current_item->value.array;
    case JSON_BOOL:
      return &current_item->value.boolean;
    case JSON_NOTYPE:
      return NULL;
  }

  return NULL;
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
