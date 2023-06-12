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

struct json_t*
json_parse_from_string_with_length(
  const char* const json_string,
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
    new_str[i] = json_string[i];
    if (json_string[i] == '\0')
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


  struct json_t* json = json_parse_from_string(new_str);
  free(new_str);

  return json;
}

struct json_t*
json_parse_from_file(
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

  struct json_t* json = json_parse_from_string_with_length(file_string, size);
  free(file_string);

  return json;

failure:
  fclose(json_file);
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

char*
json_to_string(
  const struct json_t* const json)
{
  size_t len = 0;
  size_t capacity = 100;

  char* to_string = calloc(capacity, sizeof(char));
  if (!to_string)
    return NULL;

  strncat(to_string, "{", 2);
  len = 1;

  for (size_t i = 0; i < json->n_items; ++i)
  {
    // +3 for wrapper quotes and colon
    if (len + json->items[i].key_len + 3 >= capacity
        && !_json_resize_string(&to_string, &capacity))
      goto failure;
    
    // write "key_name": (space)
    strncat(to_string, "\"", 2);
    strncat(to_string, json->items[i].key, json->items[i].key_len);
    strncat(to_string, "\":", 3);
    len += json->items[i].key_len + 3;

    // 256 is a little overkill but shouldn't be a noticeable problem
    char formatted_buffer[256] = {0};
    if (!_json_value_to_string(
          formatted_buffer,
          256,
          &json->items[i],
          &to_string,
          &len,
          &capacity))
      goto failure;
    
    // add comma
    if (i < json->n_items - 1
        && !_json_write_value_buffer_to_string(",", &to_string, &len, &capacity))
      goto failure;

    }

  // write closing body with null terminator
  if (!_json_write_value_buffer_to_string("}", &to_string, &len, &capacity))
    goto failure;

  return to_string;

failure:
  free(to_string);
  return NULL;
}

bool
json_to_file(
  const struct json_t* const json,
  const char* const filepath)
{
  char* json_string = json_to_string(json);
  if (!json_string)
    return false;
  size_t len = strlen(json_string);

  FILE* to_file = fopen(filepath, "w+");
  if (!to_file)
    return false;

  if (fwrite(json_string, sizeof(char), len, to_file) < len)
  {
    free(json_string);
    fclose(to_file);
    return false;
  }

  free(json_string);
  fclose(to_file);
  return true;
}
