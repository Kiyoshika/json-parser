#include "json.h"
#include "json_array.h"
#include "json_internal.h"
#include <stdio.h>

struct json_t*
json_create()
{
  struct json_t* json = malloc(sizeof(*json));
  if (!json)
    return NULL;

  json->n_items = 0;

  size_t capacity = 10;
  json->capacity = capacity;

  json->items = calloc(capacity, sizeof(*json->items));
  if (!json->items)
  {
    free(json);
    return NULL;
  }

  return json;

}

void
json_free(
  struct json_t** json)
{
  if (!*json)
    return;

  // for string types (which have a heap copy), free
  // the address it's pointing to (strdup'd)
  for (size_t i = 0; i < (*json)->n_items; ++i)
  {
    switch ((*json)->items[i].type)
    {
      case JSON_STRING:
      {
        char* item = (*json)->items[i].value.str;
        free(item);
        break;
      }

      case JSON_OBJECT:
      {
        struct json_t* item = (*json)->items[i].value.object;
        json_free(&item);
        break;
      }

      case JSON_ARRAY:
      {
        struct json_array_t* item = (*json)->items[i].value.array;
        json_array_free(&item);
        break;
      }

      case JSON_INT32:
      case JSON_DECIMAL:
      case JSON_BOOL:
      case JSON_NULL:
      case JSON_NOTYPE:
        break;
    }
  }

  free((*json)->items);
  (*json)->items = NULL;

  free(*json);
  *json = NULL;
}

bool
json_add_item(
  struct json_t* const json,
  const enum json_type_e type,
  const char* const key,
  void* value)
{
  if (!json)
    return false;

  if (strlen(key) == 0)
    return false;

  struct json_item_t new_item = {
    .type = type,
    .key = {0},
    .key_len = 0,
  };

  switch (type)
  {
    case JSON_INT32:
      new_item.value.int32 = *(int32_t*)value;
      break;
    case JSON_DECIMAL:
      new_item.value.decimal = *(double*)value;
      break;
    case JSON_STRING:
      new_item.value.str = value; // this is a heap copy
      break;
    case JSON_OBJECT:
      new_item.value.object = value; // this is a heap copy
      break;
    case JSON_ARRAY:
     new_item.value.array = value; // this is a heap copy
     break;
    case JSON_BOOL:
      new_item.value.boolean = *(bool*)value;
      break;
    case JSON_NULL:
      new_item.value.is_null = *(bool*)value;
    case JSON_NOTYPE:
      break;
  }

  strncpy(new_item.key, key, JSON_MAX_KEY_LEN - 1);
  new_item.key_len = strlen(new_item.key);

  struct json_item_t* current_item
    = &json->items[json->n_items];

  memcpy(current_item, &new_item, sizeof(new_item));
  json->n_items++;

  if (json->n_items >= json->capacity)
  {
    size_t new_capacity = json->capacity *= 2;
    void* alloc = realloc(json->items, new_capacity * sizeof(*json->items));
    if (!alloc)
      return false;
    json->capacity = new_capacity;
    json->items = alloc;
    // realloc doesn't guarantee nice clean zeros...so we
    // do it ourselves. technically this is not strictly
    // required, but sometimes it's helpful in debugging.
    memset(&json->items[json->n_items], 0, (json->capacity + 1 - json->n_items) * sizeof(*json->items));
  }

  return true;

}

struct json_t*
json_parse_from_string(
  const char* const json_string)
{
  struct json_t* json = json_create();
  if (!json)
    return NULL;

  // this is assuming the user supplies a null-terimated string;
  // not going to protect them from a bad input this time
  size_t string_len = strlen(json_string);

  // take values from enum _token_e in json_internal.c
  uint16_t expected_token = OPEN_BODY;
  uint16_t current_token = NONE;

  // an object containing all the information we need while parsing
  struct _json_parse_info_t parse_info = {
    .json_string = json_string,
    .json_string_idx = 0,
    .parsed_key = {0},
    .parsed_value = calloc(100, sizeof(char)),
    .parsed_value_len = 0,
    .parsed_value_capacity = 100,
    .parsed_value_type = JSON_NOTYPE,
    .parsing_key = false,
    .parsing_value = false,
    .inside_quotes = false,
    .previous_token = NONE,
    .expecting_delimiter = false
  };

  if (!parse_info.parsed_value)
    goto error;

  while (parse_info.json_string_idx < string_len)
  {
    char current_char = json_string[parse_info.json_string_idx];

    if (isspace(current_char) && !parse_info.inside_quotes)
    {
      parse_info.json_string_idx++;
      continue;
    }

    current_token = _json_get_token_type(current_char);

    if ((current_token & expected_token) == 0)
      goto error;

    if (!_json_perform_token_action(json, current_token, current_char, &parse_info))
      goto error;

    // in some casese the current_char/token may be updated after an action
    // e.g., parsing nested json body, so we just "refresh" them here
    current_char = json_string[parse_info.json_string_idx];
    current_token = _json_get_token_type(current_char);

    expected_token = _json_get_next_expected_token(current_token, parse_info.inside_quotes);

    parse_info.json_string_idx++;

    parse_info.previous_token = current_token;

  }

  if (current_token != CLOSE_BODY)
    goto error;

  if (strlen(parse_info.parsed_key) != 0
      && strlen(parse_info.parsed_value) != 0)
  {
    _json_add_item(
        json,
        &parse_info);
  }

  free(parse_info.parsed_value);
  return json;

error:
  json_free(&json);
  free(parse_info.parsed_value);
  return NULL;
}

size_t
json_type_to_size(
  const enum json_type_e type)
{
  switch (type)
  {
    case JSON_INT32:
      return sizeof(int32_t);
    case JSON_DECIMAL:
      return sizeof(double);
    case JSON_OBJECT:
      return sizeof(struct json_t);
    case JSON_STRING:
      return sizeof(char*);
    case JSON_ARRAY:
      return sizeof(struct json_array_t);
    case JSON_BOOL:
      return sizeof(bool);
    case JSON_NULL:
      return sizeof(bool);
    case JSON_NOTYPE:
      return 0;
  }

  return 0;
}
