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

struct json_t*
json_create();

void
json_free(
  struct json_t** json);

bool
json_add_item(
  struct json_t* const json,
  const enum json_type_e type,
  const char* const key,
  void* value);

bool
json_add_int32(
  struct json_t* const json,
  const char* const key,
  int32_t value);

bool
json_add_decimal(
  struct json_t* const json,
  const char* const key,
  double value);

bool
json_add_bool(
  struct json_t* const json,
  const char* const key,
  bool value);

bool
json_add_string(
  struct json_t* const json,
  const char* const key,
  char* const value);

bool
json_add_object(
  struct json_t* const json,
  const char* const key,
  struct json_t* const value);

bool
json_add_array(
  struct json_t* const json,
  const char* const key,
  struct json_array_t* value);

bool
json_add_null(
  struct json_t* const json,
  const char* const key);

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

#endif
