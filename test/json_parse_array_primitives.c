#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // simple key-value pairs for basic int and string types

  char* json_string = "{ \"array_int\": [1, 2, 3], \"array_double\": [1.1, 2.2, 3.3, 4.4, 5.5], \"array_mixed\": [1, 5.5, -25] }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  // all arrays are same type so we use get_fixed
  struct json_array_t* array_int = json_get(json, "array_int");
  int32_t expected[3] = { 1, 2, 3 };
  for (size_t i = 0; i < array_int->n_items; ++i)
  {
    int32_t actual = *(int32_t*)json_array_get_fixed(array_int, i, JSON_INT32);
    if (actual != expected[i])
    {
      fprintf(stderr, "array_int at index %zu expected %d but got %d.\n", i, expected[i], actual);
      goto cleanup;
    }
  }

  // all arrays are same type so we use get_fixed
  struct json_array_t* array_double = json_get(json, "array_double");
  double expected2[5] = { 1.1, 2.2, 3.3, 4.4, 5.5 };
  for (size_t i = 0; i < array_double->n_items; ++i)
  {
    double actual = *(double*)json_array_get_fixed(array_double, i, JSON_DECIMAL);
    if (fabs(actual - expected2[i]) > 0.001)
    {
      fprintf(stderr, "array_double at index %zu expected %f but got %f.\n", i, expected2[i], actual);
      goto cleanup;
    }
  }

  // this mixes integers and doubles so we use get_mixed to compute the offset internally
  struct json_array_t* array_mixed = json_get(json, "array_mixed");
  int32_t item1 = *(int32_t*)json_array_get_mixed(array_mixed, 0);
  if (item1 != 1)
  {
    fprintf(stderr, "array_mixed at index 0 expected %d but got %d.\n", 1, item1);
    goto cleanup;
  }

  double item2 = *(double*)json_array_get_mixed(array_mixed, 1);
  if (fabs(item2 - 5.5) > 0.0001)
  {
    fprintf(stderr, "array_mixed at index 1 expected %f but got %f.\n", 5.5, item2);
    goto cleanup;
  }

  int32_t item3 = *(int32_t*)json_array_get_mixed(array_mixed, 2);
  if (item3 != -25)
  {
    fprintf(stderr, "array_mixed at index 2 expected %d but got %d.\n", -25, item3);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
