#include "json.h"
#include "json_array.h"
#include "json_internal.h"

enum _token_e
_json_get_token_type(
  const char current_char)
{
  switch (current_char)
  {
    case '{':
      return OPEN_BODY;
    case '}':
      return CLOSE_BODY;
    case '\"':
      return QUOTE;
    case ',':
      return COMMA;
    case ':':
      return COLON;
    case ' ':
      return SPACE;
    case '[':
      return OPEN_ARRAY;
    case ']':
      return CLOSE_ARRAY;
  }

  // conditions that can't be put into switch:

  // NOTE: this condition must come first otherwise '.' will
  // be interpreted as TEXT type
  if (isdigit(current_char) || current_char == '.' || current_char == '-')
      return NUMERIC;

  if  (  isalpha(current_char) 
      || ispunct(current_char) 
      || current_char == ' '
      || current_char == '\n' 
      || current_char == '\t' 
      || current_char == '\r')
      return TEXT;

  return UNKNOWN;
}

size_t
_json_get_key_index(
  const struct json_t* const json,
  const char* const key,
  bool* key_exists)
{
  if (!json)
  {
    *key_exists = false;
    return 0;
  }

  for (size_t i = 0; i < json->n_items; ++i)
  {
    struct json_item_t* current_item = &json->items[i];

    if (strncmp(current_item->key, key, JSON_MAX_KEY_LEN) == 0)
    {
      *key_exists = true;
      return i;
    }
  }

  *key_exists = false;
  return 0;
}

uint16_t 
_json_get_next_expected_token(
  const enum _token_e current_token,
  const bool inside_quotes)
{
  // if we're inside quotes, we can really be next to ANY token
  if (inside_quotes || current_token == SPACE)
    return TEXT | NUMERIC | QUOTE | COMMA | OPEN_BODY | CLOSE_BODY | COLON | SPACE;

  switch (current_token)
  {
    case OPEN_BODY:
      return QUOTE | CLOSE_BODY;

    case CLOSE_BODY:
      return CLOSE_BODY | NONE;

    case OPEN_ARRAY:
      return QUOTE | NUMERIC | OPEN_ARRAY;

    case CLOSE_ARRAY:
      return COMMA | CLOSE_BODY | CLOSE_ARRAY;

    case QUOTE:
      return QUOTE | TEXT | COLON | CLOSE_BODY | COMMA;

    case COLON:
      return QUOTE | TEXT | NUMERIC | OPEN_BODY | OPEN_ARRAY;

    case COMMA:
      return TEXT | QUOTE | OPEN_BODY | OPEN_ARRAY;

    case TEXT:
      return TEXT | COMMA | CLOSE_BODY | CLOSE_ARRAY;

    case NUMERIC:
      return NUMERIC | TEXT | COMMA | CLOSE_BODY;
    
    case NONE:
    case UNKNOWN:
      return NONE;
    case SPACE: // already handled before switch
      break;
  }

  return NONE;
}

// general cleanup to do when parsing an array item.
// consumes any trailing whitespace and ensures no missing
// delimiters/closure
// e.g., to prevent things like [a b c] instead of [a, b, c]
bool
_json_array_item_cleanup(
  const char* const array_string,
  size_t* idx,
  const size_t len)
{
  // consume any trailing whitespace
  while (*idx < len && isspace(array_string[*idx]))
    (*idx)++;
  // double check we don't have a missing delimiter
  // e.g., to prevent things like [a b c] instead of [a, b, c]
  if (*idx < len - 1 
      && array_string[*idx] != ','
      && array_string[*idx] != ']')
    return false;

  return true;
}

char*
_json_fetch_body_string(
  const char* const json_string,
  size_t* idx)
{
  int32_t body_count = 0;
  size_t start_idx = *idx;
  size_t len = strlen(json_string);
  for (; *idx < len; ++(*idx))
  {
    switch (json_string[*idx])
    {
      case '{':
        body_count++;
        break;

      case '}':
        body_count--;
        break;
    }

    if (body_count < 0)
      return NULL;

    if (body_count == 0)
    {
      size_t substr_len = *idx - start_idx + 2;
      char* body_string = calloc(substr_len, sizeof(char));
      if (!body_string)
        return NULL;
      strncat(body_string, &json_string[start_idx], substr_len - 1);
      (*idx)++; // move past close brace }

      if (!_json_array_item_cleanup(json_string, idx, len))
      {
        free(body_string);
        return NULL;
      }

      return body_string;
    }
  }

  return NULL;
}

char*
_json_fetch_array_string(
  const char* const json_string,
  size_t* idx)
{
  int32_t body_count = 0;
  size_t start_idx = *idx;
  size_t len = strlen(json_string);
  for (; *idx < len; ++(*idx))
  {
    switch (json_string[*idx])
    {
      case '[':
        body_count++;
        break;

      case ']':
        body_count--;
        break;
    }

    if (body_count < 0)
      return NULL;

    if (body_count == 0)
    {
      size_t substr_len = *idx - start_idx + 2;
      char* body_string = calloc(substr_len, sizeof(char));
      if (!body_string)
        return NULL;
      strncat(body_string, &json_string[start_idx], substr_len - 1);
      (*idx)++; // move past close bracket ]

      if (!_json_array_item_cleanup(json_string, idx, len))
      {
        free(body_string);
        return NULL;
      }
    
      return body_string;
    }
  }

  return NULL;
}

void
_json_reset_parse_info(
  struct _json_parse_info_t* const parse_info)
{
  // this is called after comma, so we start parsing the next key
  memset(parse_info->parsed_key, 0, JSON_MAX_KEY_LEN);
  memset(parse_info->parsed_value, 0, parse_info->parsed_value_len);
  parse_info->parsed_value_len = 0; // keep the capacity as is, no point to realloc
  parse_info->parsing_key = true;
  parse_info->parsing_value = false;
  parse_info->inside_quotes = false;
  parse_info->parsed_value_type = JSON_NOTYPE;
  parse_info->expecting_delimiter = false;
}

char* 
_json_fetch_quote_string(
  const char* const array_string,
  size_t* idx)
{
  *idx += 1; // move index past initial open quote
  size_t start_idx = *idx;
  size_t len = strlen(array_string);
  for (; *idx < len; ++(*idx))
  {
    if (array_string[*idx] == '\"')
    {
      size_t substr_len = *idx - start_idx;
      char* substr = calloc(substr_len + 1, sizeof(char));
      if (!substr)
        return NULL;
      strncat(substr, &array_string[start_idx], substr_len);
      (*idx)++; // move past closing quote

      if (!_json_array_item_cleanup(array_string, idx, len))
      {
        free(substr);
        return NULL;
      }
      
      return substr;
    }
  }

  return NULL;
}

char* 
_json_fetch_numeric_string(
  const char* const array_string,
  size_t* idx,
  bool* contains_decimal)
{
  size_t start_idx = *idx;
  size_t len = strlen(array_string);
  size_t space_count = 0;
  for (; *idx < len; ++(*idx))
  {
    // whitespace prohibited in numerics
    // but we have to wait until we read the entire value
    // before determining if it's illegal or not
    if (isspace(array_string[*idx]))
      space_count++;

    if (!isdigit(array_string[*idx])
        && !isspace(array_string[*idx])
        && array_string[*idx] != '-'
        && array_string[*idx] != '.'
        && array_string[*idx] != ','
        && array_string[*idx] != ']'
        && array_string[*idx] != '}')
      return NULL;

    if (array_string[*idx] == '.')
      *contains_decimal = true;

    if (array_string[*idx] == ',' 
        || array_string[*idx] == ']'
        || array_string[*idx] == '}')
    {
      // ignore any trailing whitespace before closing token. e.g.,
      // { "key": 10    } <-- this whitespace is ok
      // { "key": 1 0   } <-- this whitespace is NOT ok
      // basically we iterate backwards again until finding a non-space token
      size_t reverse_idx = *idx - 1;
      while (isspace(array_string[reverse_idx]))
      {
        space_count--;
        reverse_idx--;
      }

      if (space_count > 0)
        return NULL;

      size_t substr_len = *idx - start_idx;
      char* substr = calloc(substr_len + 1, sizeof(char));

      if (!substr)
        return NULL;
      strncat(substr, &array_string[start_idx], substr_len);
      
      if (!_json_array_item_cleanup(array_string, idx, len))
      {
        free(substr);
        return NULL;
      }

      return substr;
    }
  }

  return NULL;
}

char*
_json_fetch_array_item_string(
  const char* const array_string,
  size_t* idx,
  bool* contains_decimal)
{
  size_t start_idx = *idx;
  size_t len = strlen(array_string);

  // ignore any initial whitespace
  for (size_t i = start_idx; i < len; ++i)
  {
    if (isspace(array_string[i]))
      start_idx++;
    else
      break;
  }
  *idx = start_idx;

  if (array_string[start_idx] == '{')
    return _json_fetch_body_string(array_string, idx);
  
  if (array_string[start_idx] == '[')
    return _json_fetch_array_string(array_string, idx);

  if (array_string[start_idx] == '\"')
    return _json_fetch_quote_string(array_string, idx);

  if (array_string[start_idx] == '.' 
      || array_string[start_idx] == '-'
      || (array_string[start_idx] >= '0' && array_string[start_idx] <= '9'))
      return _json_fetch_numeric_string(array_string, idx, contains_decimal);

  for (; *idx < len; ++(*idx))
    if (array_string[*idx] == ',' || array_string[*idx] == ']')
      break;

  size_t size = *idx - start_idx;
  char* item_string = calloc(size + 1, sizeof(char));
  if (!item_string)
    return NULL;

  strncat(item_string, &array_string[start_idx], size);
  return item_string;
}

enum json_type_e
_json_get_item_type(
  const char* const item_string)
{
  // ignore first set of whitespace (if any)
  size_t idx = 0;
  size_t len = strlen(item_string);
  for (size_t i = 0; i < len; ++i)
  {
    if (isspace(item_string[i]))
      idx++;
    else
      break;
  }

  // type will be updated to DECIMAL later if one is found while parsing
  if (item_string[idx] == '-' || (item_string[idx] >= '0' && item_string[idx] <= '9'))
    return JSON_INT32;

  if (item_string[idx] == '[')
    return JSON_ARRAY;

  if (item_string[idx] == '{')
    return JSON_OBJECT;

  // spcial cases not to be confused with JSON_STRING

  // TODO: add null here

  if (strcmp(item_string, "true") == 0
      || strcmp(item_string, "false") == 0)
    return JSON_BOOL;

  return JSON_STRING;
}

struct json_array_t*
_json_parse_array(
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
        if (!json_array_append(array, type, &value))
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
        struct json_array_t* new_array = _json_parse_array(item_string);
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

bool
_json_check_key_exists(
  const struct json_t* const json,
  const char* const search_key)
{
  for (size_t i = 0; i < json->n_items; ++i)
    if (strncmp(json->items[i].key, search_key, JSON_MAX_KEY_LEN) == 0)
      return true;

  return false;
}

// an internal version of add_item to perform type casting
bool 
_json_add_item(
  struct json_t* const json,
  struct _json_parse_info_t* const parse_info)
{
  if (_json_check_key_exists(json, parse_info->parsed_key))
    return false;

  // initially these special cases have JSON_NOTYPE,
  // so we check them here after extracting their values
  if (parse_info->parsed_value_type == JSON_NOTYPE)
  {
    // TODO: add null here
    if (strcmp(parse_info->parsed_value, "true") == 0
        || strcmp(parse_info->parsed_value, "false") == 0)
      parse_info->parsed_value_type = JSON_BOOL;
    else
      return false;
  }

  // TODO: check duplicate keys

  bool success = false;
  switch (parse_info->parsed_value_type)
  {

    case JSON_OBJECT:
    {
      char* nested_json_string 
        = _json_fetch_body_string(
            parse_info->json_string, 
            &parse_info->json_string_idx);

      if (!nested_json_string)
        return false;

      struct json_t* nested_json = json_parse_from_string(nested_json_string);
      if (!nested_json)
      {
        free(nested_json_string);
        return false;
      }

      success = json_add_item(json, JSON_OBJECT, parse_info->parsed_key, nested_json);

      free(nested_json_string);
      break;
    }

    case JSON_ARRAY:
    {
      char* array_string
        = _json_fetch_array_string(
            parse_info->json_string,
            &parse_info->json_string_idx);

      if (!array_string)
        return false;

      struct json_array_t* json_array = _json_parse_array(array_string);
      if (!json_array)
      {
        free(array_string);
        return false;
      }

      success = json_add_item(json, JSON_ARRAY, parse_info->parsed_key, json_array);

      free(array_string);
      break;
    }

    case JSON_STRING:
    {
      char* quote_string = _json_fetch_quote_string(parse_info->json_string, &parse_info->json_string_idx);

      if (!quote_string)
        return false;
      success = json_add_item(json, JSON_STRING, parse_info->parsed_key, quote_string);
      break;
    }

    case JSON_DECIMAL:
    {
      char* endptr = NULL;
      double cast_value = strtod(parse_info->parsed_value, &endptr);
      success = json_add_item(json, JSON_DECIMAL, parse_info->parsed_key, &cast_value);
      break;
    }

    case JSON_INT32:
    {
      char* endptr = NULL;
      int32_t cast_value = strtol(parse_info->parsed_value, &endptr, 10);
      success = json_add_item(json, JSON_INT32, parse_info->parsed_key, &cast_value);
      break;
    }

    case JSON_BOOL:
    {
      bool value = false;
      if (strcmp(parse_info->parsed_value, "true") == 0)
        value = true;
      else if (strcmp(parse_info->parsed_value, "false") != 0)
        return false;

      success = json_add_item(json, JSON_BOOL, parse_info->parsed_key, &value);
      break;
    }

    case JSON_NOTYPE:
      break;
  }

  _json_reset_parse_info(parse_info);

  return success;
}

void
_json_append_char_to_key(
  struct _json_parse_info_t* const parse_info,
  const char current_char)
{
  size_t key_len = strlen(parse_info->parsed_key);
  if (key_len == JSON_MAX_KEY_LEN - 1)
    return;
  parse_info->parsed_key[key_len] = current_char;
}

bool 
_json_append_char_to_value(
  struct _json_parse_info_t* const parse_info,
  const char current_char)
{
  parse_info->parsed_value[parse_info->parsed_value_len++] = current_char;
  if (parse_info->parsed_value_len == parse_info->parsed_value_capacity)
  {
    size_t new_capacity = parse_info->parsed_value_capacity * 2;
    void* alloc = realloc(parse_info->parsed_value, new_capacity * sizeof(char));
    if (!alloc)
      return false;
    parse_info->parsed_value = alloc;
    parse_info->parsed_value_capacity = new_capacity;
    memset(&parse_info->parsed_value[parse_info->parsed_value_len], 0, new_capacity + 1 - parse_info->parsed_value_len);
  }

  return true;
}

#include <stdio.h>

bool 
_json_perform_token_action(
  struct json_t* const json,
  const enum _token_e current_token,
  const char current_char,
  struct _json_parse_info_t* const parse_info)
{
  switch (current_token)
  {
    case OPEN_BODY:
    {
      parse_info->parsing_key = true;
      parse_info->parsing_value = false;

      if (parse_info->previous_token == COLON)
      {
          parse_info->parsed_value_type = JSON_OBJECT;
          if (!_json_add_item(json, parse_info))
            return false;
      }
      break;
    }

    case OPEN_ARRAY:
    {
      parse_info->parsed_value_type = JSON_ARRAY;
      if (!_json_add_item(json, parse_info))
        return false;
      break;
    }

    case CLOSE_ARRAY: // no-op
      break;

    case CLOSE_BODY:
      // if we still have content in our value when closing off JSON,
      // try to add the item
      if (parse_info->parsing_value && strlen(parse_info->parsed_value) > 0)
      {
        if (!_json_add_item(json, parse_info))
          return false;
      }
      parse_info->parsing_key = false;
      parse_info->parsing_value = false;
      break;

    case COLON:
    {
      // after reading a key, we set the expecting_delimiter flag.
      // if we encounter another key before this flag is cleared, then
      // we are missing a delimiter between keys (comma).
      // this flag is cleared after a successfull _json_add_item()
      parse_info->parsing_key = false;
      parse_info->parsing_value = true;
      parse_info->expecting_delimiter = true;
      break;
    }

    case QUOTE:
    {
      if (parse_info->parsing_key && parse_info->expecting_delimiter)
        return false;

      // a special case for above delimiter check for strings, e.g.,:
      // { "key1": "value" "key2": "value } <-- missing comma
      if (!parse_info->inside_quotes && strlen(parse_info->parsed_value) > 0)
        return false;

      // if value starts with a quote, it's a string type
      if (!parse_info->inside_quotes
          && parse_info->parsing_value 
          && strlen(parse_info->parsed_value) == 0)
      {
        parse_info->parsed_value_type = JSON_STRING;
        if (!_json_add_item(json, parse_info))
          return false;
        break;
      }

      parse_info->inside_quotes = !parse_info->inside_quotes;

      break;
    }

    // spaces and commas are technically different tokens, so
    // by default they would be ignored
    parse_string:
    case TEXT:
    {
      if (parse_info->parsing_key 
          && parse_info->inside_quotes)
      {
        _json_append_char_to_key(parse_info, current_char);
      }

      else if (parse_info->parsing_value
              && !_json_append_char_to_value(parse_info, current_char))
        return false;

      break;
    }

    case NUMERIC:
    {
      // a numeric value inside a string
      if (parse_info->parsed_value_type == JSON_STRING)
        goto parse_string;

      if (parse_info->parsing_key)
      {
        _json_append_char_to_key(parse_info, current_char);
        break;
      }

      bool contains_decimal = false;
      char* numeric_string 
        = _json_fetch_numeric_string(
            parse_info->json_string, 
            &parse_info->json_string_idx,
            &contains_decimal);
      if (!numeric_string)
        return false;

      if (contains_decimal)
        parse_info->parsed_value_type = JSON_DECIMAL;
      else
        parse_info->parsed_value_type = JSON_INT32;

      size_t len = strlen(numeric_string);
      strncat(parse_info->parsed_value, numeric_string, len);
      parse_info->parsed_value_len = len;

      if (!_json_add_item(json, parse_info))
        return false;
      
      free(numeric_string);
      _json_reset_parse_info(parse_info);
      
      break;
    }

    case COMMA:
    {
      if (parse_info->parsing_key 
          && parse_info->inside_quotes)
      {
        _json_append_char_to_key(parse_info, current_char);
      }

      else if (parse_info->parsing_value 
              && parse_info->inside_quotes)
      {
        _json_append_char_to_value(parse_info, current_char);
      }

      else if (strlen(parse_info->parsed_key) == 0
          || strlen(parse_info->parsed_value) == 0)
        return false;

      else if (!_json_add_item(
               json,
               parse_info))
       return false;

      break;
    }

    case SPACE:
    {
      if (parse_info->parsing_key
          && parse_info->inside_quotes)
      {
        _json_append_char_to_key(parse_info, current_char);
      }

      else if (parse_info->parsing_value
          && parse_info->inside_quotes)
      {
        _json_append_char_to_value(parse_info, current_char);
      }

      break;
    }

    case UNKNOWN:
      return false;

    case NONE: // no-op
      return true;

  }

  return true;
}
