#include "json.h"
#include "json_array.h"
#include "json_internal.h"

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

	if (_json_check_key_exists(json, key))
		return false;

  struct json_item_t* current_item
    = &json->items[json->n_items];

  current_item->type = type;
  _json_set_item_value(current_item, value); 

  strncpy(current_item->key, key, JSON_MAX_KEY_LEN - 1);
  current_item->key_len = strlen(current_item->key);

  json->n_items++;

  if (json->n_items >= json->capacity)
  {
    size_t new_capacity = json->capacity *= 2;
    void* alloc = realloc(json->items, new_capacity * sizeof(*json->items));
    if (!alloc)
      return false;
    json->capacity = new_capacity;
    json->items = alloc;
  }

  return true;

}

bool
json_add_int32(
	struct json_t* const json,
	const char* const key,
	int32_t value)
{
	return json_add_item(json, JSON_INT32, key, &value);
}

bool
json_add_decimal(
	struct json_t* const json,
	const char* const key,
	double value)
{
	return json_add_item(json, JSON_DECIMAL, key, &value);
}

bool
json_add_bool(
	struct json_t* const json,
	const char* const key,
	bool value)
{
	return json_add_item(json, JSON_BOOL, key, &value);
}

bool
json_add_string(
	struct json_t* const json,
	const char* const key,
	char* const value)
{
	char* value_cpy = strdup(value);
	if (!value_cpy)
		return false;
	return json_add_item(json, JSON_STRING, key, value_cpy);
}

bool
json_add_object(
	struct json_t* const json,
	const char* const key,
	struct json_t* const value)
{
	return json_add_item(json, JSON_OBJECT, key, value);
}

bool
json_add_array(
	struct json_t* const json,
	const char* const key,
	struct json_array_t* const value)
{
	return json_add_item(json, JSON_ARRAY, key, value);
}

bool
json_add_null(
	struct json_t* const json,
	const char* const key)
{
	bool value = true;
	return json_add_item(json, JSON_NULL, key, &value);
}
