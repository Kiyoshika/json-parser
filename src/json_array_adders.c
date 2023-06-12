#include "json.h"
#include "json_array.h"
#include "json_internal.h"

static bool
_json_array_append_item(
  struct json_array_t* array,
  const enum json_type_e type,
  void* value)
{
  array->item_types[array->n_items] = type;
  if (array->n_items + 1 == array->item_capacity)
  {
    size_t new_item_capacity = array->item_capacity * 2;
    void* alloc = realloc(array->item_types, new_item_capacity * sizeof(*array->item_types));
    if (!alloc)
      return false;
    array->item_capacity = new_item_capacity;
    array->item_types = alloc;

    void* alloc2 = realloc(array->items, new_item_capacity * sizeof(*array->items));
    if (!alloc2)
      return false;
    array->items = alloc;
  }

  struct json_item_t* current_item = &array->items[array->n_items];
  current_item->type = type;
  _json_set_item_value(current_item, value); 

  array->n_items++;
  return true;
}

bool
json_array_append_null(
  struct json_array_t* array)
{
  bool value = true;
  return _json_array_append_item(array, JSON_NULL, &value);
}

bool
json_array_append(
  struct json_array_t* array,
  enum json_type_e item_type,
  void* value)
{
  // need to handle this special case properly.
  // this is to prevent the user doing something wierd like
  // json_array_append(array, JSON_NULL, false)
  if (item_type == JSON_NULL)
    return json_array_append_null(array);

  return _json_array_append_item(array, item_type, value);
}

bool
json_array_append_int32(
  struct json_array_t* array,
  int32_t value)
{
  return _json_array_append_item(array, JSON_INT32, &value);
}

bool
json_array_append_decimal(
  struct json_array_t* array,
  double value)
{
  return _json_array_append_item(array, JSON_DECIMAL, &value);
}

bool 
json_array_append_bool(
  struct json_array_t* array,
  bool value)
{
  return _json_array_append_item(array, JSON_BOOL, &value);
}

bool
json_array_append_string(
  struct json_array_t* array,
  char* value)
{
  return _json_array_append_item(array, JSON_STRING, value);
}

bool
json_array_append_object(
  struct json_array_t* array,
  struct json_t* value)
{
  return _json_array_append_item(array, JSON_OBJECT, value);
}

bool
json_array_append_array(
  struct json_array_t* array,
  struct json_array_t* value)
{
  return _json_array_append_item(array, JSON_ARRAY, value);
}
