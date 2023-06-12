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
#include "json_array_adders.h"

struct json_array_t*
json_array_create();

void
json_array_free(
  struct json_array_t** array);

char*
json_array_to_string(
  const struct json_array_t* const array);

bool
json_array_to_file(
  const struct json_array_t* const array,
  const char* const filepath);

struct json_array_t*
json_parse_array_from_string(
  const char* const array_string);

struct json_array_t*
json_parse_array_from_file(
  const char* const filepath);

#endif
