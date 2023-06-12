//#include "json.h"
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

char*
json_array_to_string(
  const struct json_array_t* const array)
{
  size_t len = 0;
  size_t capacity = 100;

  char* to_string = calloc(capacity, sizeof(char));
  if (!to_string)
    return NULL;

  strncat(to_string, "[", 2);
  len = 1;

  for (size_t i = 0; i < array->n_items; ++i)
  {
    // 256 is a little overkill but shouldn't be a noticeable problem
    char formatted_buffer[256] = {0};
    if (!_json_value_to_string(
          formatted_buffer,
          256,
          &array->items[i],
          &to_string,
          &len,
          &capacity))
      goto failure;

    // add comma
    if (i < array->n_items - 1
        && !_json_write_value_buffer_to_string(",", &to_string, &len, &capacity))
      goto failure;
  }

  // write closing bracket with null terminator
  if (!_json_write_value_buffer_to_string("]", &to_string, &len, &capacity))
    goto failure;

  return to_string;

failure:
  free(to_string);
  return NULL;
}

bool
json_array_to_file(
  const struct json_array_t* const array,
  const char* const filepath)
{
  char* array_string = json_array_to_string(array);
  if (!array_string)
    return false;
  size_t len = strlen(array_string);

  FILE* to_file = fopen(filepath, "w+");
  if (!to_file)
    return false;

  if (fwrite(array_string, sizeof(char), len, to_file) < len)
  {
    free(array_string);
    fclose(to_file);
    return false;
  }

  free(array_string);
  fclose(to_file);
  return true;
}
