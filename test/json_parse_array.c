#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // simple key-value pairs for basic int and string types

  char* json_string = "{ \"array_int\": [1, 2, 3], \"array_double\": [1.1, 2.2, 3.3, 4.4, 5.5], \"array_mixed\": [1, 5.5, 2] }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  // all arrays are same type so we use get_fixed
  struct json_array_t* array_int = json_get(json, "array_int");
  for (size_t i = 0; i < array_int->n_items; ++i)
    printf("%d ", *(int32_t*)json_array_get_fixed(array_int, i, sizeof(int32_t)));

  // all arrays are same type so we use get_fixed
  struct json_array_t* array_double = json_get(json, "array_double");
  for (size_t i = 0; i < array_double->n_items; ++i)
    printf("\n%f ", *(double*)json_array_get_fixed(array_double, i, sizeof(double)));

  // this mixes integers and doubles so we use get_mixed to compute the offset internally
  struct json_array_t* array_mixed = json_get(json, "array_mixed");
  int32_t item1 = *(int32_t*)json_array_get_mixed(array_mixed, 0);
  double item2 = *(double*)json_array_get_mixed(array_mixed, 1);
  int32_t item3 = *(int32_t*)json_array_get_mixed(array_mixed, 2);
  printf("%d %f %d\n", item1, item2, item3);

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
