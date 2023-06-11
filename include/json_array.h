#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

// forward declaration
struct json_item_t;

struct json_array_t
{
  enum json_type_e* item_types;
  struct json_item_t* items;
  size_t n_items;
  size_t item_capacity;
};

struct json_array_t*
json_array_create();

bool
json_array_append(
  struct json_array_t* array,
  const enum json_type_e item_type,
  void* item);

bool
json_array_append_null(
  struct json_array_t* array);

void*
json_array_get(
  const struct json_array_t* const array,
  const size_t idx);

void
json_array_free(
  struct json_array_t** array);

#endif
