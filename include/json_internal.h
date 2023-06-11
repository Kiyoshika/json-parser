#ifndef JSON_INTERNAL_H
#define JSON_INTERNAL_H

#include <ctype.h>
#include <stdio.h>

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
  bool expecting_delimiter;
};

enum _token_e
_json_get_token_type(
  const char current_char);

size_t
_json_get_key_index(
  const struct json_t* const json,
  const char* const key,
  bool* key_exists);

uint16_t 
_json_get_next_expected_token(
  const enum _token_e current_token,
  const bool inside_quotes);

bool
_json_array_item_cleanup(
  const char* const array_string,
  size_t* idx,
  const size_t len);

char*
_json_fetch_body_string(
  const char* const json_string,
  size_t* idx);

char*
_json_fetch_array_string(
  const char* const json_string,
  size_t* idx);

void
_json_reset_parse_info(
  struct _json_parse_info_t* const parse_info);

char* 
_json_fetch_quote_string(
  const char* const array_string,
  size_t* idx);

char* 
_json_fetch_numeric_string(
  const char* const array_string,
  size_t* idx,
  bool* contains_decimal);

char*
_json_fetch_array_item_string(
  const char* const array_string,
  size_t* idx,
  bool* contains_decimal);

enum json_type_e
_json_get_item_type(
  const char* const item_string);

struct json_array_t*
_json_parse_array(
  const char* const array_string);

bool
_json_check_key_exists(
  const struct json_t* const json,
  const char* const search_key);

bool 
_json_add_item(
  struct json_t* const json,
  struct _json_parse_info_t* const parse_info);

void
_json_append_char_to_key(
  struct _json_parse_info_t* const parse_info,
  const char current_char);

bool 
_json_append_char_to_value(
  struct _json_parse_info_t* const parse_info,
  const char current_char);

bool 
_json_perform_token_action(
  struct json_t* const json,
  const enum _token_e current_token,
  const char current_char,
  struct _json_parse_info_t* const parse_info);

void
_json_set_item_value(
  struct json_item_t* item,
  void* value);

void*
_json_get_item_value(
  struct json_item_t* const item);

void
_json_deallocate_item(
  struct json_item_t* item);

#endif
