#include <ctype.h>

// using bit masks to define token types so we can combine
// multiple tokens using | and compare using & bit operators
//
// apparently binary literals are a GCC extension so using
// hex literals instead (to try to be less compiler-specific)
enum _token_e 
{
  // NULL token, used to initialize
  NONE        = 0x00,   //0b00000000,       
  // {
  OPEN_BODY   = 0x01,   //0b00000001,
  // }
  CLOSE_BODY  = 0x02,   //0b00000010,
  // "
  QUOTE       = 0x04,   //0b00000100,
  // ,
  COMMA       = 0x08,   //0b00001000,
  // :
  COLON       = 0x10,   //0b00010000,
  // [A-Za-z] + punctuation
  TEXT        = 0x20,   //0b00100000,
  // [0-9] + decimal
  NUMERIC     = 0x40,   //0b01000000
  // ' '
  SPACE       = 0x80,   //0b10000000
  // [
  OPEN_ARRAY  = 0x100,  //0b100000000
  // ]
  CLOSE_ARRAY = 0x200,  //0b1000000000
  // encountered unknown token
  UNKNOWN     = 0x400,  //0b10000000000
};

struct _json_parse_info_t
{
  const char* const json_string;
  size_t json_string_idx;
  char parsed_key[JSON_MAX_KEY_LEN];
  char* parsed_value;
  size_t parsed_value_len;
  size_t parsed_value_capacity;
  enum json_type_e parsed_value_type;
  bool parsing_key;
  bool parsing_value;
  bool inside_quotes;
  enum _token_e previous_token;
};

static enum _token_e
_get_token_type(
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

static uint16_t 
_get_next_expected_token(
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
      return QUOTE;

    case TEXT:
      return NONE; // if text is found outside of quotes, this is an error
                   // UNLESS it's null but that's not implemented yet

    case NUMERIC:
      return NUMERIC | COMMA | CLOSE_BODY;
    
    case NONE:
    case UNKNOWN:
      return NONE;
    case SPACE: // already handled before switch
      break;
  }

  return NONE;
}

static char*
_json_fetch_body_string(
  const char* const json_string,
  size_t idx)
{
  int32_t body_count = 0;
  size_t start_idx = idx;
  for (; idx < strlen(json_string); ++idx)
  {
    switch (json_string[idx])
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
      size_t substr_len = idx - start_idx + 2;
      char* body_string = calloc(substr_len, sizeof(char));
      if (!body_string)
        return NULL;
      strncat(body_string, &json_string[start_idx], substr_len - 1);
      return body_string;
    }
  }

  return NULL;
}

static char*
_json_fetch_array_string(
  const char* const json_string,
  size_t idx)
{
  int32_t body_count = 0;
  size_t start_idx = idx;
  for (; idx < strlen(json_string); ++idx)
  {
    switch (json_string[idx])
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
      size_t substr_len = idx - start_idx + 2;
      char* body_string = calloc(substr_len, sizeof(char));
      if (!body_string)
        return NULL;
      strncat(body_string, &json_string[start_idx], substr_len - 1);
      return body_string;
    }
  }

  return NULL;
}

static void
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
}

static char*
_json_fetch_array_item_string(
  const char* const array_string,
  size_t idx,
  bool* contains_decimal)
{
  size_t start_idx = idx;
  size_t len = strlen(array_string);

  for (; idx < len; ++idx)
  {
    if (array_string[idx] == '.')
    {
      *contains_decimal = true;
      continue;
    }

    if (array_string[idx] == ',' || array_string[idx] == ']')
      break;
  }

  size_t size = idx - start_idx;
  char* item_string = calloc(size + 1, sizeof(char));
  if (!item_string)
    return NULL;

  strncat(item_string, &array_string[start_idx], size);
  return item_string;
}

static struct json_array_t*
_json_parse_array(
  const char* const array_string)
{
  size_t len = strlen(array_string);

  enum json_type_e type = JSON_NOTYPE;

  // type is determined from the first (non-bracket) character
  // (assuming it's not empty)
  if (array_string[1] != ']')
  {
    if (array_string[1] == '"')
      type = JSON_STRING;
    // type will be updated to DECIMAL later if one is found while parsing
    else if (array_string[1] >= '0' && array_string[1] <= '9')
      type = JSON_INT32;
    else if (array_string[1] == '[')
      type = JSON_ARRAY;
    else if (array_string[1] == '{')
      type = JSON_OBJECT;
  }

  union data 
  {
    int32_t* int32;
    double* decimal;
    char* str;
    struct json_t* object;
    struct json_array_t* array;
  } data;

  // start with a default of 10 items, reallocating later if needed
  size_t capacity = 10;
  switch (type)
  {
    case JSON_INT32:
      data.int32 = calloc(capacity, sizeof(int32_t));
      break;
    case JSON_DECIMAL:
      data.decimal = calloc(capacity, sizeof(double));
      break;
    // these other cases will be handled separately
    case JSON_STRING:
    case JSON_OBJECT:
    case JSON_ARRAY:
    case JSON_NOTYPE:
      break;
  }

  size_t current_array_idx = 0;
  char* endptr; // used for string to numeric conversions (can't declare inside switch)

  // starting at 1 to skip open bracket [
  // going to len - 1 (exclusive) to ignore closing bracket ]
  for (size_t i = 1; i < len - 1; ++i)
  {
    bool contains_decimal = false;
    char* item_string = _json_fetch_array_item_string(array_string, i, &contains_decimal);

    if (!item_string)
      goto cleanup;

    i += strlen(item_string);
    switch (type)
    {
      case JSON_INT32:
      {
        if (!contains_decimal)
        {
          int32_t value = strtol(item_string, &endptr, 10);
          data.int32[current_array_idx++] = value;
          if (current_array_idx == capacity)
          {
            capacity *= 2;
            void* alloc = realloc(data.int32, capacity * sizeof(int32_t));
            if (!alloc)
              goto cleanup;
            data.int32 = alloc;
            memset(&data.int32[current_array_idx], 0, capacity - current_array_idx);
          }
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
        data.decimal[current_array_idx++] = value;
        if (current_array_idx == capacity)
        {
          capacity *= 2;
          void* alloc = realloc(data.decimal, capacity * sizeof(double));
          if (!alloc)
            goto cleanup;
          data.decimal = alloc;
          memset(&data.int32[current_array_idx], 0, capacity - current_array_idx);
        }
        break;
      }
    }

    free(item_string);
  }

  goto createandreturn;

cleanup:
  switch (type)
  {
    case JSON_INT32:
      free(data.int32);
      break;
    case JSON_DECIMAL:
      free(data.decimal);
      break;
    case JSON_STRING:
    case JSON_OBJECT:
    case JSON_ARRAY:
    case JSON_NOTYPE:
      // not implemented yet
      break;
  }
  return NULL;

createandreturn:
  // realloc array back to smaller size (getting rid of extra-padded items)
  // since we are downsizing, I don't think it's possible to get NULL
  // (but...guess I could be wrong)
  switch (type)
  {
    case JSON_INT32:
      data.int32 = realloc(data.int32, current_array_idx * sizeof(int32_t));
      break;
    case JSON_DECIMAL:
      data.decimal = realloc(data.decimal, current_array_idx * sizeof(double));
      break;
    case JSON_STRING:
    case JSON_OBJECT:
    case JSON_ARRAY:
    case JSON_NOTYPE:
      // not implemented yet
      break;
  }

  struct json_array_t* json_array = calloc(1, sizeof(*json_array));
  if (!json_array)
    return NULL;
  json_array->type = type;
  json_array->length = current_array_idx;
  switch (type)
  {
    case JSON_INT32:
      json_array->contents.int32 = data.int32;
      break;
    case JSON_DECIMAL:
      json_array->contents.decimal = data.decimal;
      break;
    case JSON_STRING:
    case JSON_OBJECT:
    case JSON_ARRAY:
    case JSON_NOTYPE:
      // not implemented yet
      break;
  }

  return json_array;
}

// an internal version of add_item to perform type casting
static bool 
_json_add_item(
  struct json_t* const json,
  struct _json_parse_info_t* const parse_info)
{

  // TODO: check duplicate keys

  bool success = false;
  switch (parse_info->parsed_value_type)
  {

    case JSON_OBJECT:
    {
      char* nested_json_string 
        = _json_fetch_body_string(
            parse_info->json_string, 
            parse_info->json_string_idx);

      if (!nested_json_string)
        return false;

      struct json_t* nested_json = json_parse_from_string(nested_json_string);
      if (!nested_json)
        return false;

      success = json_add_item(json, JSON_OBJECT, parse_info->parsed_key, nested_json);

      // after parsing, we need to move the index pointer to after the body
      parse_info->json_string_idx += strlen(nested_json_string);

      free(nested_json_string);
      break;
    }

    case JSON_ARRAY:
    {
      char* array_string
        = _json_fetch_array_string(
            parse_info->json_string,
            parse_info->json_string_idx);

      if (!array_string)
        return false;

      struct json_array_t* json_array = _json_parse_array(array_string);
      if (!json_array)
        return false;

      success = json_add_item(json, JSON_ARRAY, parse_info->parsed_key, json_array);

      // after parsing, we need to move the index pointer to after the array
      parse_info->json_string_idx += strlen(array_string);

      free(array_string);
      break;
    }

    case JSON_STRING:
    {
      // NOTE: we make a copy since the original parsed_info->parsed_value gets
      // free'd after json_parse_...() ends
      char* parsed_value_copy = strdup(parse_info->parsed_value);
      if (!parsed_value_copy)
        return false;
      success = json_add_item(json, JSON_STRING, parse_info->parsed_key, parsed_value_copy);
      break;
    }

    case JSON_DECIMAL:
    {
      char* endptr;
      double cast_value = strtod(parse_info->parsed_value, &endptr);
      success = json_add_item(json, JSON_DECIMAL, parse_info->parsed_key, &cast_value);
      break;
    }

    case JSON_INT32:
    {
      char* endptr;
      int32_t cast_value = strtol(parse_info->parsed_value, &endptr, 10);
      success = json_add_item(json, JSON_INT32, parse_info->parsed_key, &cast_value);
      break;
    }

    case JSON_NOTYPE:
      break;
  }

  _json_reset_parse_info(parse_info);

  return success;
}

static void
_json_append_char_to_key(
  struct _json_parse_info_t* const parse_info,
  const char current_char)
{
  size_t key_len = strlen(parse_info->parsed_key);
  if (key_len == JSON_MAX_KEY_LEN - 1)
    return;
  parse_info->parsed_key[key_len] = current_char;
}

static bool 
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

static bool 
_perform_token_action(
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
      parse_info->parsing_key = false;
      parse_info->parsing_value = false;
      break;

    case COLON:
      parse_info->parsing_key = false;
      parse_info->parsing_value = true;
      break;

    case QUOTE:
    {
      // if value starts with a quote, it's a string type
      if (!parse_info->inside_quotes
          && parse_info->parsing_value 
          && strlen(parse_info->parsed_value) == 0)
        parse_info->parsed_value_type = JSON_STRING;

      parse_info->inside_quotes = !parse_info->inside_quotes;

      break;
    }

    // spaces and commas are technically different tokens, so
    // by default they would be ignored
    case TEXT:
    {
      if (parse_info->parsing_key 
          && parse_info->inside_quotes)
      {
        _json_append_char_to_key(parse_info, current_char);
      }

      else if (parse_info->parsing_value 
               && parse_info->inside_quotes 
               && parse_info->parsed_value_type == JSON_STRING)
      {
        if (!_json_append_char_to_value(parse_info, current_char))
          return false;
      }

      else
        return false;

      break;
    }

    case NUMERIC:
    {
      size_t value_len = strlen(parse_info->parsed_value);

      // if value starts with numeric, it's a numeric type
      // (I'm using INT32 as a placeholder for numeric but it works
      // with doubles/floats as well)
      if (parse_info->parsing_value
          && parse_info->parsed_value_type == JSON_NOTYPE 
          && value_len == 0)
      {
        if (current_char == '.')
          parse_info->parsed_value_type = JSON_DECIMAL;
        else
          parse_info->parsed_value_type = JSON_INT32;

        if (!_json_append_char_to_value(parse_info, current_char))
          return false;
      }

      else if (parse_info->parsing_value
               && (parse_info->parsed_value_type == JSON_DECIMAL || parse_info->parsed_value_type == JSON_INT32))
      {
        if (current_char == '.' && parse_info->parsed_value_type == JSON_INT32)
          parse_info->parsed_value_type = JSON_DECIMAL;

        if (!_json_append_char_to_value(parse_info, current_char))
          return false;
      }

      else
        return false;

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
