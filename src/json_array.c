//#include "json.h"
#include "json_array.h"
#include "json_array_adders.h"
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

struct json_array_t*
json_parse_array_from_string(
  const char* const array_string)
{
  size_t len = strlen(array_string);

  struct json_array_t* array = json_array_create();
  if (!array)
    return NULL;

  char* endptr = NULL; // used for string to numeric conversions (can't declare inside switch)
  char* item_string = NULL;

  // starting at 1 to skip open bracket [
  // going to len - 1 (exclusive) to ignore closing bracket ]
  for (size_t i = 1; i < len - 1; ++i)
  {
    bool contains_decimal = false;
    item_string = _json_fetch_array_item_string(array_string, &i, &contains_decimal);

    if (!item_string)
      goto cleanup;

    enum json_type_e type = _json_get_item_type(item_string);

    switch (type)
    {
      case JSON_INT32:
      {
        if (!contains_decimal)
        {
          int32_t value = strtol(item_string, &endptr, 10);
          if (!json_array_append(array, type, &value))
            goto cleanup;
          break;
        }

        // not doing a fallthrough to avoid compiler warning, so using goto instead
        if (contains_decimal)
        {
          type = JSON_DECIMAL;
          goto decimal;
        }

        break;
      }

      decimal:
      case JSON_DECIMAL:
      {
        double value = strtod(item_string, &endptr);
        if (!json_array_append(array, type, &value))
          goto cleanup;
        break;
      }

      case JSON_STRING:
      {
        char* value = strdup(item_string);
        if (!json_array_append(array, type, value))
          goto cleanup;
        break;
      }

      case JSON_BOOL:
      {
        // bool must be "true" or "false" all lowercase
        bool value = false;
        if (strcmp(item_string, "true") == 0)
          value = true;
        else if (strcmp(item_string, "false") != 0)
          goto cleanup;

        if (!json_array_append(array, type, &value))
          goto cleanup;
        break;
      }

      case JSON_NULL:
      {
        if (!json_array_append_null(array))
          goto cleanup;
        break;
      }

      case JSON_OBJECT:
      {
        struct json_t* object = json_parse_from_string(item_string);
        if (!object)
          goto cleanup;
        if (!json_array_append(array, type, object))
          goto cleanup;
        break;
      }

      case JSON_ARRAY:
      {
        struct json_array_t* new_array = json_parse_array_from_string(item_string);
        if (!new_array)
          goto cleanup;
        if (!json_array_append(array, type, new_array))
          goto cleanup;
        break;
      }

      case JSON_NOTYPE:
        goto cleanup;
    }

    free(item_string);
  }

  goto createandreturn;

cleanup:
  free(item_string);
  json_array_free(&array);
  return NULL;

createandreturn:
  return array;
}

struct json_array_t*
json_parse_array_from_string_with_length(
  const char* const array_string,
  const size_t len)
{
  // read at most len characters
  // if we find \0 then we can terminate early
  size_t capacity = 100;
  char* new_str = calloc(capacity, sizeof(char));
  if (!new_str)
    return NULL;

  size_t i = 0;
  for (; i < len; ++i)
  {
    new_str[i] = array_string[i];
    if (array_string[i] == '\0')
      break;
    if (i + 1 == capacity)
    {
      size_t new_capacity = capacity * 2;
      // redundant multiplication but explicit
      void* alloc = realloc(new_str, new_capacity * sizeof(char));
      if (!alloc)
      {
        free(new_str);
        return NULL;
      }
      capacity = new_capacity;
      new_str = alloc;
    }
  }

  // if no terminator was found, append one to the end
  if (i == len)
    new_str[i] = '\0';


  struct json_array_t* array = json_parse_array_from_string(new_str);
  free(new_str);

  return array;
}

struct json_array_t*
json_parse_array_from_file(
  const char* const filepath)
{
  FILE* json_file = fopen(filepath, "r");
  if (!json_file)
    return NULL;

  rewind(json_file);
  if (fseek(json_file, 0, SEEK_END) == -1)
    goto failure;

  int size = ftell(json_file);
  
  rewind(json_file);

  char* file_string = calloc(size, sizeof(char));
  if (!file_string)
    goto failure;

  if (fread(file_string, sizeof(char), size, json_file) < size)
  {
    free(file_string);
    goto failure;
  }

  fclose(json_file);

  struct json_array_t* array = json_parse_array_from_string_with_length(file_string, size);
  free(file_string);

  return array;

failure:
  fclose(json_file);
  return NULL;
}
