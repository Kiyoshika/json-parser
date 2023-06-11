#include "json.h"
#include "json_array.h"
#include "json_array_setters.h"
#include "json_internal.h"

void
json_array_set(
  struct json_array_t* const array,
  const size_t idx,
  const enum json_type_e type,
  void* value)
{
  struct json_item_t* current_item = &array->items[idx];
  _json_deallocate_item(current_item);
  current_item->type = type;
  _json_set_item_value(current_item, value);
}

void
json_array_set_int32(
  struct json_array_t* const array,
  const size_t idx,
  int32_t value)
{
  json_array_set(array, idx, JSON_INT32, &value);
}

void
json_array_set_decimal(
  struct json_array_t* const array,
  const size_t idx,
  double value)
{
  json_array_set(array, idx, JSON_DECIMAL, &value);
}

void
json_array_set_string(
  struct json_array_t* const array,
  const size_t idx,
  char* value)
{
  json_array_set(array, idx, JSON_STRING, value);
}

void
json_array_set_object(
  struct json_array_t* const array,
  const size_t idx,
  struct json_t* value)
{
  json_array_set(array, idx, JSON_OBJECT, value);
}

void
json_array_set_array(
  struct json_array_t* const array,
  const size_t idx,
  struct json_array_t* value)
{
  json_array_set(array, idx, JSON_ARRAY, value);
}

void
json_array_set_bool(
  struct json_array_t* const array,
  const size_t idx,
  bool value)
{
  json_array_set(array, idx, JSON_BOOL, &value);
}

void
json_array_set_null(
  struct json_array_t* const array,
  const size_t idx)
{
  bool value = true;
  json_array_set(array, idx, JSON_NULL, &value);
}
