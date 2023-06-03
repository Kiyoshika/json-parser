#include <ctype.h>

// using bit masks to define token types so we can combine
// multiple tokens using | and compare using & bit operators
//
// apparently binary literals are a GCC extension so using
// hex literals instead (to try to be less compiler-specific)
enum _token_e 
{
  // NULL token, used to initialize
  NONE        = 0x00, //0b00000000,       
  // {
  OPEN_BODY   = 0x01, //0b00000001,
  // }
  CLOSE_BODY  = 0x02, //0b00000010,
  // "
  QUOTE       = 0x04, //0b00000100,
  // ,
  COMMA       = 0x08, //0b00001000,
  // :
  COLON       = 0x10, //0b00010000,
  // [A-Za-z] + punctuation
  TEXT        = 0x20, //0b00100000,
  // [0-9] + decimal
  NUMERIC     = 0x40, //0b01000000
  // encountered unknown token
  UNKNOWN     = 0x80, //0b10000000
};

struct _json_parse_info_t
{
  char parsed_key[JSON_MAX_KEY_LEN];
  char parsed_value[JSON_MAX_KEY_LEN];
  enum json_type_e parsed_value_type;
  bool parsing_key;
  bool parsing_value;
  bool inside_quotes;
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
  }

  // conditions that can't be put into switch:

  // NOTE: this condition must come first otherwise '.' will
  // be interpreted as TEXT type
  if (isdigit(current_char) || current_char == '.')
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
  if (inside_quotes)
    return TEXT | NUMERIC | QUOTE | COMMA | OPEN_BODY | CLOSE_BODY | COLON;

  switch (current_token)
  {
    case OPEN_BODY:
      return QUOTE | CLOSE_BODY;

    case CLOSE_BODY:
      return NONE;

    case QUOTE:
      return QUOTE | TEXT | COLON | CLOSE_BODY;

    case COLON:
      return QUOTE | TEXT | NUMERIC;

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
  }

  return NONE;
}

static bool 
_perform_token_action(
  const enum _token_e current_token,
  const char current_char,
  struct _json_parse_info_t* const parse_info)
{
  switch (current_token)
  {
    case OPEN_BODY:
      parse_info->parsing_key = true;
      parse_info->parsing_value = false;
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
        parse_info->parsed_value_type = STRING;

      parse_info->inside_quotes = !parse_info->inside_quotes;

      break;
    }

    case TEXT:
    {
      if (parse_info->parsing_key 
          && parse_info->inside_quotes)
      {
        size_t key_len = strlen(parse_info->parsed_key);
        if (key_len == JSON_MAX_KEY_LEN - 1)
            break;
        parse_info->parsed_key[key_len] = current_char;
      }

      else if (parse_info->parsing_value 
               && parse_info->inside_quotes 
               && parse_info->parsed_value_type == STRING)
      {
        size_t value_len = strlen(parse_info->parsed_value);
        if (value_len == JSON_MAX_KEY_LEN - 1)
          break;
        parse_info->parsed_value[value_len] = current_char;
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
        parse_info->parsed_value_type = INT32;
        parse_info->parsed_value[0] = current_char;
      }

      else if (parse_info->parsing_value
               && parse_info->parsed_value_type == INT32)
      {
        if (value_len == JSON_MAX_KEY_LEN - 1)
          break;

        parse_info->parsed_value[value_len] = current_char;
      }

      else
        return false;

      break;
    }

    case UNKNOWN:
      return false;

    case NONE: // no-op
      return true;

  }

  return true;
}
