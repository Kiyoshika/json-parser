#include "json.h"
#include "json_array.h"
#include "json_internal.h"

// when setting a new item, need to check if the
// current item is heap-allocated before overwriting it
static void
_json_deallocate_item(
  struct json_item_t* item)
{
  switch (item->type)
  {
    case JSON_OBJECT:
      json_free(&item->value.object);
      break;
    case JSON_ARRAY:
      json_array_free(&item->value.array);
      break;
    case JSON_STRING:
      free(item->value.str);
      break;
    // no-ops
    case JSON_INT32:
    case JSON_DECIMAL:
    case JSON_BOOL:
    case JSON_NULL:
    case JSON_NOTYPE:
      break;
  }
}

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
  _json_deallocate_item(item);
  item->type = JSON_INT32;
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
  _json_deallocate_item(item);
  item->type = JSON_DECIMAL;
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
  _json_deallocate_item(item);
  item->type = JSON_STRING;
  item->value.str = strdup(value);
  if (!item->value.str)
    return false;
  return true;
}

bool
json_set_object(
  const struct json_t* const json,
  const char* const key,
  struct json_t* value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  _json_deallocate_item(item);
  item->type = JSON_OBJECT;
  item->value.object = value;
  return true;
}

bool
json_set_array(
  const struct json_t* const json,
  const char* const key,
  struct json_array_t* value)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  _json_deallocate_item(item);
  item->type = JSON_ARRAY;
  item->value.array = value;
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
  _json_deallocate_item(item);
  item->type = JSON_BOOL;
  item->value.boolean = value;
  return true;
}

bool
json_set_null(
  const struct json_t* const json,
  const char* const key)
{
  bool key_exists = false;
  size_t idx = _json_get_key_index(json, key, &key_exists);
  if (!key_exists)
    return false;
  struct json_item_t* item = &json->items[idx];
  _json_deallocate_item(item);
  item->type = JSON_NULL;
  item->value.is_null = true;
  return true;
}
