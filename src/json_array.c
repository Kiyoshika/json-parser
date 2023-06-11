#include "json.h"
#include "json_array.h"
#include "json_internal.h"

struct json_array_t*
json_array_create()
{
  struct json_array_t* array = calloc(1, sizeof(*array));
  if (!array)
    return NULL;

  array->n_items = 0;
  array->item_capacity = 10;
  array->item_types = calloc(array->item_capacity, sizeof(*array->item_types));
  if (!array->item_types)
  {
    free(array);
    return NULL;
  }

  array->items = calloc(array->item_capacity, sizeof(*array->items));

  if (!array->items)
  {
    free(array->item_types);
    free(array);
    return NULL;
  }

  return array;
}

static bool
_json_array_append_item(
  struct json_array_t* array,
  const enum json_type_e type,
  void* item)
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
  _json_set_item_value(current_item, type, item); 

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
  void* item)
{
  // need to handle this special case properly.
  // this is to prevent the user doing something wierd like
  // json_array_append(array, JSON_NULL, false)
  if (item_type == JSON_NULL)
    return json_array_append_null(array);

  return _json_array_append_item(array, item_type, item);
}

void*
json_array_get(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

void
json_array_free(
  struct json_array_t** array)
{
  for (size_t i = 0; i < (*array)->n_items; ++i)
    _json_deallocate_item(&(*array)->items[i]);

  free((*array)->items);
  (*array)->items = NULL;

  free((*array)->item_types);
  (*array)->item_types = NULL;

  free(*array);
  *array = NULL;

}
