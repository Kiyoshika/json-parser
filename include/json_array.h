#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include "json.h"

// JSON arrays can have mixed types, so we use a void*
struct json_array_t
{
  enum json_type_e* item_types;
  size_t n_items;
  size_t item_capacity;

  void* items;
  size_t current_bytes;
  size_t byte_capacity;
};

struct json_array_t*
json_array_create();

bool
json_array_append(
  struct json_array_t* array,
  enum json_type_e item_type,
  void* item,
  size_t sizeof_item);

// get item by computing the offset
// based off internal sizes.
// it's recommended to use this only for mixed-type arrays
void*
json_array_get_mixed(
  const struct json_array_t* const array,
  const size_t idx);

// get item by assuming all have same size.
// recommended for same-type arrays (e.g., all integers)
void*
json_array_get_fixed(
  const struct json_array_t* const array,
  const size_t idx,
  const enum json_type_e type);

void
json_array_free(
  struct json_array_t** array);

#endif
