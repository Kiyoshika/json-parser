#ifndef JSON_H
#define JSON_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef JSON_MAX_KEY_LEN
#define JSON_MAX_KEY_LEN 51
#endif

enum json_type_e 
{
  JSON_NOTYPE,
  JSON_INT32,
  JSON_DECIMAL,
  JSON_STRING,
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_BOOL,
  JSON_NULL
};

struct json_item_t
{
  enum json_type_e type;
  union value
  {
    int32_t int32;
    double decimal;
    char* str;
    struct json_t* object;
    struct json_array_t* array;
    bool boolean;
    bool is_null;
  } value;
  char key[JSON_MAX_KEY_LEN];
  size_t key_len;
};

struct json_t
{
  struct json_item_t* items;
  size_t n_items;
  size_t capacity;
};

#include "json_getters.h"
#include "json_setters.h"
#include "json_adders.h"

struct json_t*
json_create();

void
json_free(
  struct json_t** json);

struct json_t*
json_parse_from_string(
  const char* const json_string);

struct json_t*
json_parse_from_string_with_length(
  const char* const json_string,
  const size_t len);

struct json_t*
json_parse_from_file(
  const char* const filepath);

size_t
json_type_to_size(
  const enum json_type_e type);

char*
json_to_string(
  const struct json_t* const json);

bool
json_to_file(
  const struct json_t* const json,
  const char* const filepath);

#endif
