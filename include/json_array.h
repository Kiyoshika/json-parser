#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include "json.h"

// forward declaration
struct json_item_t;

struct json_array_t
{
  enum json_type_e* item_types;
  struct json_item_t* items;
  size_t n_items;
  size_t item_capacity;
};

#include "json_array_getters.h"
#include "json_array_setters.h"

struct json_array_t*
json_array_create();

bool
json_array_append(
  struct json_array_t* array,
  const enum json_type_e item_type,
  void* value);

bool
json_array_append_int32(
  struct json_array_t* array,
  int32_t value);

bool
json_array_append_decimal(
  struct json_array_t* array,
  double value);

bool
json_array_append_bool(
  struct json_array_t* array,
  bool value);

bool
json_array_append_string(
  struct json_array_t* array,
  char* value);

bool
json_array_append_object(
  struct json_array_t* array,
  struct json_t* value);

bool
json_array_append_array(
  struct json_array_t* array,
  struct json_array_t* value);

bool
json_array_append_null(
  struct json_array_t* array);

void
json_array_free(
  struct json_array_t** array);

#endif
