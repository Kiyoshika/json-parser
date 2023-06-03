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
  // encountered unknown token
  UNKNOWN     = 0x100,  //0b100000000
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

static uint8_t 
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

    case QUOTE:
      return QUOTE | TEXT | COLON | CLOSE_BODY | COMMA;

    case COLON:
      return QUOTE | TEXT | NUMERIC | OPEN_BODY;

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

// iterate through a JSON string (or substring) to fetch the first
// complete JSON body. Useful for extracting nested JSON objects
static char*
_json_fetch_body(
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
  parse_info->parsed_value_type = NOTYPE;
}

// an internal version of add_item to perform type casting
static bool 
_json_add_item(
  struct json_t* const json,
  struct _json_parse_info_t* const parse_info)
{
  bool success = false;
  switch (parse_info->parsed_value_type)
  {

    case OBJECT:
    {
      char* nested_json_string 
        = _json_fetch_body(parse_info->json_string, parse_info->json_string_idx);

      if (!nested_json_string)
        return false;

      struct json_t* nested_json = json_parse_from_string(nested_json_string);
      if (!nested_json)
        return false;

      success = json_add_item(json, OBJECT, parse_info->parsed_key, nested_json);


      // after parsing, we need to move the index pointer to after the body
      parse_info->json_string_idx += strlen(nested_json_string);

      free(nested_json_string);
      break;
    }

    case STRING:
    {
      // NOTE: we make a copy since the original parsed_info->parsed_value gets
      // free'd after json_parse_...() ends
      char* parsed_value_copy = strdup(parse_info->parsed_value);
      if (!parsed_value_copy)
        return false;
      success = json_add_item(json, STRING, parse_info->parsed_key, parsed_value_copy);
      break;
    }

    case DECIMAL:
    {
      char* endptr;
      double cast_value = strtod(parse_info->parsed_value, &endptr);
      success = json_add_item(json, DECIMAL, parse_info->parsed_key, &cast_value);
      break;
    }

    case INT32:
    {
      char* endptr;
      int32_t cast_value = strtol(parse_info->parsed_value, &endptr, 10);
      success = json_add_item(json, INT32, parse_info->parsed_key, &cast_value);
      break;
    }

    case NOTYPE:
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
          parse_info->parsed_value_type = OBJECT;
          if (!_json_add_item(json, parse_info))
            return false;
      }
      break;
    }

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
        parse_info->parsed_value_type = STRING;

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
               && parse_info->parsed_value_type == STRING)
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
          && parse_info->parsed_value_type == NOTYPE 
          && value_len == 0)
      {
        if (current_char == '.')
          parse_info->parsed_value_type = DECIMAL;
        else
          parse_info->parsed_value_type = INT32;

        if (!_json_append_char_to_value(parse_info, current_char))
          return false;
      }

      else if (parse_info->parsing_value
               && (parse_info->parsed_value_type == DECIMAL || parse_info->parsed_value_type == INT32))
      {
        if (current_char == '.' && parse_info->parsed_value_type == INT32)
          parse_info->parsed_value_type = DECIMAL;

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
