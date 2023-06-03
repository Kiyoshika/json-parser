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

// if we ever get larger than 8-byte addresses...
#define JSON_ADDRESS_SIZE sizeof(void*)

enum json_type_e
{
  NOTYPE,
  INT32,
	STRING
};

struct json_item_t
{
	enum json_type_e type;
	char key[JSON_MAX_KEY_LEN];
  size_t key_len;
  uintptr_t value_address;
};

struct json_t
{
	struct json_item_t* items;
	size_t n_items;
  size_t capacity;
};

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
  const void* const value);

uintptr_t
json_get(
 const struct json_t* const json,
 const char* const key);

struct json_t*
json_parse(
  const char* const json_string);

#endif
