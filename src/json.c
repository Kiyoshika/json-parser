#include "json.h"
#include "json_internal.c"
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
  const void* const value)
{
  if (!json)
    return false;

  if (strlen(key) == 0)
    return false;

  // TODO: check duplicate keys
  // (create this in util.h)

  struct json_item_t new_item = {
    .type = type,
    .key = {0},
    .key_len = 0,
    .value_address = (uintptr_t)value
  };

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

uintptr_t
json_get(
  const struct json_t* const json,
  const char* const key)
{
  if (!json)
    return 0;

  // extra safety incase user passes a non-terimated string
  char _key[JSON_MAX_KEY_LEN] = {0};
  strncpy(_key, key, JSON_MAX_KEY_LEN - 1);

  for (size_t i = 0; i < json->n_items; ++i)
  {
    // not using const here incase user wants to modify item
    // after calling get()
    struct json_item_t* current_item = &json->items[i];

    if (current_item->key_len == strlen(_key) 
        && strncmp(current_item->key, _key, current_item->key_len) == 0)
      return current_item->value_address;
  }

  return 0;
}

struct json_t*
json_parse(
  const char* const json_string)
{
  struct json_t* json = json_create();
  if (!json)
    return NULL;

  size_t string_idx = 0;
  // this is assuming the user supplies a null-terimated string;
  // not going to protect them from a bad input this time
  size_t string_len = strlen(json_string);

  // take values from enum _token_e in json_internal.c
  uint8_t expected_token = OPEN_BODY;
  uint8_t current_token = NONE;

  // an object containing all the information we need while parsing
  struct _json_parse_info_t parse_info = {
    .parsed_key = {0},
    .parsed_value = {0},
    .parsed_value_type = NOTYPE,
    .parsing_key = false,
    .parsing_value = false,
    .inside_quotes = false
  };

  while (string_idx < string_len)
  {
    const char current_char = json_string[string_idx];

    // skip whitespace (not inside quotes)
    if ((current_char == ' ' || current_char == '\n' || current_char == '\t')
        && !parse_info.inside_quotes)
    {
      string_idx++;
      continue;
    }

    current_token = _get_token_type(current_char);

    if ((current_token & expected_token) == 0)
      goto error;

    if (!_perform_token_action(current_token, current_char, &parse_info))
      goto error;

    expected_token = _get_next_expected_token(current_token, parse_info.inside_quotes);

    string_idx++;

  }

  if (current_token != CLOSE_BODY)
    goto error;

  printf("KEY: %s\n", parse_info.parsed_key);
  printf("VALUE: %s\n", parse_info.parsed_value);

  return json;

error:
  json_free(&json);
  return NULL;
}
