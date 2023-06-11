#include "json.h"
#include "json_array.h"
#include "json_array_getters.h"
#include "json_internal.h"

void*
json_array_get(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

/* all of these getters are the same besides the return type (besides null)
 * could probably be made simpler with a macro maybe but it's
 * not really a big deal
 */
int32_t*
json_array_get_int32(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

double*
json_array_get_decimal(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

char*
json_array_get_string(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

struct json_t*
json_array_get_object(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

struct json_array_t*
json_array_get_array(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

bool*
json_array_get_bool(
  const struct json_array_t* const array,
  const size_t idx)
{
  return _json_get_item_value(&array->items[idx]);
}

bool
json_array_get_isnull(
  const struct json_array_t* const array,
  const size_t idx)
{
  return array->items[idx].type == JSON_NULL;
}
